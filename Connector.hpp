#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

#define READ_END 0
#define WRITE_END 1

// process id
pid_t pid;

// pipline input / output
int pipin_w, pipin_r, pipout_w, pipout_r;

// string buffer
char buffer[2048];

// times of read from buffer
ssize_t numRead;

void ConnectToEngine()
{
    // file describer
    int inpipe[2];
    int outpipe[2];

    // if pipe success
    // inpipe[0] will be the read end of the pipe
    // data can be read from inpipe[0]
    // inpipe[1] will be the write end of the pipe
    // data can be write from inpipe[1]
    pipe(inpipe);
    pipe(outpipe);

    // create children process
    pid = fork();

    if (pid == 0)
    {
        // child process

        // close the unused end of the pipe.
        close(inpipe[WRITE_END]);
        close(outpipe[READ_END]);

        // redirect standard input to input pipeline
        dup2(inpipe[READ_END], STDIN_FILENO);
        // redirect standard output to output pipeline
        dup2(outpipe[WRITE_END], STDOUT_FILENO);
        // redirect standard error to output pipeline
        dup2(outpipe[WRITE_END], STDERR_FILENO);

        execl("/opt/homebrew/bin/stockfish", "stockfish", (char *) NULL);
    }
    else if (pid > 0)
    {
        // parent process
        
        // close the unused end of the pipe.
        close(inpipe[READ_END]);
        close(outpipe[WRITE_END]);

        pipin_w = inpipe[WRITE_END];
        pipin_r = outpipe[READ_END];
    }
    // failed
    // else 
    // {
    //     // fork failed
    // }
}

// get the next move which generated by stockfish
string getNextMove(string position)
{
    string str;
    // stockfish command 
    position = "position startpos moves " + position + "\ngo\n";

    cout << position << endl;

    // write to input pipeline
    write(pipin_w, position.c_str(), position.length());
    // suspend seconds in microseconds
    // 500000 microseconds = 0.5 second
    // 程式執行 0.5 秒後終止。
    usleep(500000);

    // interrupt infinite loop
    position = "stop\n";

    // write to input pipeline
    write(pipin_w, position.c_str(), position.length());

    // Read from the pipe until no more data is available
    while ((numRead = read(pipin_r, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[numRead] = '\0'; // Null-terminate the string
        str += buffer;
        // find "bestmove" string from str
        unsigned long n = str.find("bestmove");

        // if find
        if (n != string::npos)
            return str.substr(n + 9, 4);
    }

    return "error";
}

// close connection
void CloseConnection()
{
    // send quit to input pipeline
    write(pipin_w, "quit\n", 5);
    close(pipin_w);
    close(pipin_r);

    int status;
    waitpid(pid, &status, 0);
}

#endif // CONNECTOR_H
