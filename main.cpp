#include <SFML/Graphics.hpp>
#include <time.h>
#include "Connector.hpp"
using namespace sf;
using namespace std;

// 定義變數size,值為56
int size = 56;

Vector2f offset(28, 28);

// 定義了一個Sprite型別的數組f,長度為32,用於儲存棋子
Sprite f[32];

// 添加位置紀錄
string position = "";

// 第一到八列的棋子數值
int board[8][8] = {
    -1, -2, -3, -4, -5, -3, -2, -1,
    -6, -6, -6, -6, -6, -6, -6, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    1, 2, 3, 4, 5, 3, 2, 1};

// 添加小黑中棋子移動的提示
string toChessNote(Vector2f p)
{
    string s = "";
    s += char(p.x / size + 97);
    s += char(7 - p.y / size + 49);
    return s;
}

// 增加吃子功能
Vector2f toCoord(char a, char b)
{
    int x = int(a) - 97;
    int y = 7 - int(b) + 49;
    return Vector2f(x * size, y * size);
}

// 為添加吃子功能而修改移動方式
void move(string str)
{
    Vector2f oldPos = toCoord(str[0], str[1]);
    Vector2f newPos = toCoord(str[2], str[3]);

    // 移動後該格子上有棋子則直接覆蓋數據，並將原先的棋子設置在視窗外
    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == newPos)
            f[i].setPosition(-100, -100);

    // 移動後該位置上沒有棋子，則將移動的棋子的數據覆蓋在該格上
    for (int i = 0; i < 32; i++)
        if (f[i].getPosition() == oldPos)
            f[i].setPosition(newPos);

    // castling       //if the king didn't move
    if (str == "e1g1")
        if (position.find("e1") == -1)
            move("h1f1");
    if (str == "e8g8")
        if (position.find("e8") == -1)
            move("h8f8");
    if (str == "e1c1")
        if (position.find("e1") == -1)
            move("a1d1");
    if (str == "e8c8")
        if (position.find("e8") == -1)
            move("a8d8");
}

void loadPosition()
{
    // 初始化計數器k為0
    int k = 0;

    // 遍歷整個棋盤
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            // 獲取棋盤位置(i,j)的棋子數值
            int n = board[i][j];
            // 如果棋子數值為0,則跳過此次迴圈
            if (!n)
                continue;
            // 計算棋子的索引值
            int x = abs(n) - 1;
            // 計算棋子的顏色
            int y = n > 0 ? 1 : 0;
            // 使用setTextureRect函數設置棋子的紋理矩形位置大小
            f[k].setTextureRect(IntRect(size * x, size * y, size, size));
            // 使用setPosition函數設置棋子在棋盤上的位置
            f[k].setPosition(size * j, size * i);
            // 增加計數器k的值
            k++;
        }

    for (int i = 0; i < position.length(); i += 5)
        move(position.substr(i, 4));
}

int main()
{
    // 設定棋子的外觀大小
    RenderWindow window(VideoMode(504, 504), "The Chess!");

    ConnectToEngine("./stockfish");

    // 設定棋子變數
    Texture t1, t2;
    // 載入棋子模型
    t1.loadFromFile("images/figures.png");
    // 載入棋盤模型
    t2.loadFromFile("images/board.png");

    // 定義了一個名為s的Sprite物件,並用t1紋理來初始化它
    // Sprite s(t1);

    for (int i = 0; i < 32; i++)
    {
        // 將f數組中的每個Sprite物件設置為t1的紋理
        f[i].setTexture(t1);
    }

    // 定義了一個名為sboard的Sprite物件,並用t2紋理來初始化它
    Sprite sBoard(t2);

    // 呼叫loadPosition函數,用於載入棋盤上的棋子的位置和相關圖形
    loadPosition();

    // 將起始值設為false
    bool isMove = false;

    float dx = 0, dy = 0;
    Vector2f oldPos, newPos;
    string str;
    int n = 0;
    // 如果視窗開著繼續執行
    while (window.isOpen())
    {
        // 取滑鼠在視窗中的位453, 453置
        // Vector2i pos = Mouse::getPosition(window);
        Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);

        // 令變數e可以進行點擊
        Event e;
        while (window.pollEvent(e))
        {
            // 如果變數e被點擊則關閉視窗
            if (e.type == Event::Closed)
                window.close();

            ////move back////
            // 如果按下BackSpace
            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::BackSpace)
                // 將目前棋盤的位置資料清除，並且重新讀取上一步的位置資料
                {
                    // added
                    if (position.length() > 6)
                        position.erase(position.length() - 6, 5);
                    loadPosition();
                }

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::V)
                    cout << position << endl;

            /////drag and drop/////
            // 如果滑鼠按鈕被按下
            if (e.type == Event::MouseButtonPressed)
                // 如果按下的是左鍵
                if (e.key.code == Mouse::Left)
                    // 如果物體s包含滑鼠位置(pos.x,pos.y)
                    for (int i = 0; i < 32; i++)
                        // modified
                        // if (s.getGlobalBounds().contains(pos.x, pos.y))
                        if (f[i].getGlobalBounds().contains(pos.x, pos.y))
                        {
                            cout << "is clicked" << endl;
                            // 設置isMove為真
                            isMove = true;
                            n = i;
                            // 計算滑鼠位置和物體f[i]的位置得差值作為dx
                            dx = pos.x - f[i].getPosition().x;
                            // 計算滑鼠位置和物體f[i]的位置得差值作為dy
                            dy = pos.y - f[i].getPosition().y;
                            // 設定物體的原位置
                            oldPos = f[i].getPosition();
                        }

            // 如果滑鼠按鈕被釋放
            if (e.type == Event::MouseButtonReleased)
                // 如果釋放的是左鍵
                if (e.key.code == Mouse::Left)
                {
                    // 設置isMove為假
                    isMove = false;
                    // 計算釋放的位置在哪裡並將旗子擺放至新的位置
                    Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
                    newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
                    // 在小黑中增加棋子移動的提示
                    str = toChessNote(oldPos) + toChessNote(newPos);
                    move(str);
                    if (oldPos != newPos)
                        position += str + " ";
                    // comment
                    cout << str << endl;
                    f[n].setPosition(newPos);
                }
        }

        // comp move
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            str = getNextMove(position);

            oldPos = toCoord(str[0], str[1]);
            newPos = toCoord(str[2], str[3]);

            for (int i = 0; i < 32; i++)
                if (f[i].getPosition() == oldPos)
                    n = i;

            /////animation///////
            for (int k = 0; k < 50; k++)
            {
                Vector2f p = newPos - oldPos;
                f[n].move(p.x / 50, p.y / 50);
                window.draw(sBoard);
                for (int i = 0; i < 32; i++)
                    f[i].move(offset);
                for (int i = 0; i < 32; i++)
                    window.draw(f[i]);
                window.draw(f[n]);
                for (int i = 0; i < 32; i++)
                    f[i].move(-offset);
                window.display();
            }

            move(str);
            position += str + " ";
            f[n].setPosition(newPos);
        }

        // 如果isMove為真 則將物體f[n]的位置設為滑鼠位置減去偏移量(dx和dy)
        if (isMove)
            f[n].setPosition(pos.x - dx, pos.y - dy);

        /////////////////////////draw/////////////////////////
        // 進行重製
        window.clear();
        // 繪製棋子
        window.draw(sBoard);

        for (int i = 0; i < 32; i++)
            f[i].move(offset);
        // 將f數組中的32個Sprite物件分別繪製到視窗上
        for (int i = 0; i < 32; i++)
            window.draw(f[i]);
        for (int i = 0; i < 32; i++)
            f[i].move(-offset);
        // 顯示繪製完成的作品
        window.display();
    }

    CloseConnection();

    return 0;
}