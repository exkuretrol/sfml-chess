#include <SFML/Graphics.hpp>
#include <time.h>
#include "Connector.hpp"
using namespace sf;
using namespace std;

// 定義變數size，56 剛好是 Spirit 的大小
int size = 56;

// 圖片偏移量
Vector2f offset(28, 28);

// 定義了一個 Sprite 型別的陣列 f，長度為 32，用於儲存棋子狀態
Sprite f[32];

// 第一到八列的棋子數值
int board[8][8] = {
    {-1, -2, -3, -4, -5, -3, -2, -1},
    {-6, -6, -6, -6, -6, -6, -6, -6},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {6, 6, 6, 6, 6, 6, 6, 6},
    {1, 2, 3, 4, 5, 3, 2, 1}};

// 初始化位置紀錄
// 儲存方式：d2d4 g8f6 c2c4 e7e6 g1f3 d7d5 g2g3 d5c4 f1g2 f8b4 c1d2 a7a5...
// 其中 d2d4 由前一步位置與移動位置所組成
// UCI -> univarsal chess interface
//   a b c d e f g h
// 8 R K B Q K B K R
// 7 p p p p p p p p
// 6
// 5
// 4
// 3
// 2 p p p p p p p p
// 1 R K B Q K B K R
string position = "";

// 將 Spirit 座標轉換成棋譜座標
string toChessNote(Vector2f p)
{
    string s = "";
    s += char(p.x / size + 97);
    s += char(7 - p.y / size + 49);
    return s;
}

// 將棋譜座標轉換成螢幕上實際的座標（Spirit 座標）
Vector2f toCoord(char a, char b)
{
    // eg a8 -> x: 0, y: 48 * 56
    int x = int(a) - 97;
    int y = 7 - int(b) + 49;
    return Vector2f(x * size, y * size);
}

// 移動棋子
void moveChess(string str)
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

    // castling       
    //if the king didn't move
    // 王車易位
    if (str == "e1g1")
        if (position.find("e1") == string::npos)
            moveChess("h1f1");
    if (str == "e8g8")
        if (position.find("e8") == string::npos)
            moveChess("h8f8");
    if (str == "e1c1")
        if (position.find("e1") == string::npos)
            moveChess("a1d1");
    if (str == "e8c8")
        if (position.find("e8") == string::npos)
            moveChess("a8d8");
}

void loadPosition()
{
    // 初始化計數器 k （陣列 f 索引）為 0
    int k = 0;

    // 遍歷整個棋盤
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            // 獲取棋盤位置 (i,j) 的棋子類別
            int n = board[i][j];

            // 如果棋子數值為空（0）則跳過此次迴圈
            if (!n)
                continue;

            // 計算棋子的索引值
            int x = abs(n) - 1;

            // 計算棋子的顏色
            // 負值為白方
            // 正值為黑方
            int y = n > 0 ? 1 : 0;

            // 使用 Spirit 的 setTextureRect 函數設置棋子的紋理矩形位置大小
            f[k].setTextureRect(IntRect(size * x, size * y, size, size));

            // 使用 Spirit 的 setPosition 函數設置棋子在棋盤上的位置
            f[k].setPosition(size * j, size * i);

            // 增加計數器 k 的值
            k++;
        }

    // 遍歷 position 字串
    // 移動棋子
    //      1234512345
    // e.g. d2d4 g8f6 
    for (unsigned long i = 0; i < position.length(); i += 5)
        moveChess(position.substr(i, 4));
}

int main()
{
    // 設定遊戲視窗的大小
    RenderWindow window(VideoMode(504, 504), "The Chess!");

    // 連接西洋棋引擎（定義在 Connector.hpp）
    ConnectToEngine();

    // 初始化棋子紋理
    Texture t1, t2;

    // 載入棋子圖片
    t1.loadFromFile("images/figures.png");

    // 載入棋盤圖片
    t2.loadFromFile("images/board.png");

    for (int i = 0; i < 32; i++)
    {
        // 將 f 陣列中的每個 Sprite 物件設置為 t1 的紋理
        f[i].setTexture(t1);
    }

    // 定義了一個名為 sboard 的 Sprite 物件，並用 t2 紋理來初始化它
    Sprite sBoard(t2);

    // 呼叫 loadPosition 函數，載入棋盤上的棋子的位置和相關圖形
    loadPosition();

    // 將可移動的變數起始值設為 false
    bool isMove = false;

    float dx = 0, dy = 0;
    Vector2f oldPos, newPos;
    // 暫存棋子前一步與後一步的座標 
    // e.g. d2d4
    string str;
    int n = 0;

    // 如果視窗開著
    while (window.isOpen())
    {
        // 取得滑鼠在視窗中的位置
        Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);

        // 初始化事件為變數 e
        Event e;

        // 當遊戲產生事件時
        while (window.pollEvent(e))
        {
            // 如果遊戲關閉被點擊則關閉視窗
            if (e.type == Event::Closed)
                window.close();

            //// 退回上一步 ////
            // 如果按下BackSpace
            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::BackSpace)
                // 將目前棋盤的位置資料清除，並且從陣列 position 重新讀取所有棋子的移動軌跡
                {
                    // 如果步數 >= 2 才做
                    if (position.length() > 6)
                        position.erase(position.length() - 6, 5);
                    loadPosition();
                }

            ///// 滑鼠移動旗子 /////
            // 如果滑鼠按鈕被按下
            if (e.type == Event::MouseButtonPressed)
                // 如果按下的是左鍵
                if (e.mouseButton.button == Mouse::Left)
                    for (int i = 0; i < 32; i++)
                        // 如果棋子 Spirit 四周邊界包含了滑鼠座標
                        if (f[i].getGlobalBounds().contains(pos.x, pos.y))
                        {
                            // 設置 isMove 為真，令棋子變得可移動
                            isMove = true;

                            // 複製一份棋子索引
                            n = i;

                            // 計算滑鼠位置和物體 f[i] 的位置得差值作為 dx
                            dx = pos.x - f[i].getPosition().x;

                            // 計算滑鼠位置和物體 f[i] 的位置得差值作為 dy
                            dy = pos.y - f[i].getPosition().y;

                            // 設定物體的原位置
                            oldPos = f[i].getPosition();
                        }

            // 如果滑鼠按鈕被釋放
            if (e.type == Event::MouseButtonReleased)
                // 如果釋放的是左鍵
                if (e.mouseButton.button == Mouse::Left)
                {
                    // 設置isMove為假
                    isMove = false;

                    // 計算釋放的位置在哪裡，並轉換成實際座標
                    Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);

                    // 將旗子擺放至新的位置
                    newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));

                    // 將這次的移動製作成新的移動字串
                    str = toChessNote(oldPos) + toChessNote(newPos);

                    // 移動棋子
                    moveChess(str);
                    
                    // 如果棋子有移動，寫入 position 字串
                    if (oldPos != newPos)
                        position += str + " ";

                    // 印出本自移動位置
                    cout << "Move: " << str << endl;

                    // 設置 Spirit 座標
                    f[n].setPosition(newPos);
                }
        }

        // 讓 Stockfish 幫你動下一步
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            // 根據目前 position 產生下一步
            str = getNextMove(position);

            // 將棋譜轉換成 Spirit 看得懂的座標
            oldPos = toCoord(str[0], str[1]);
            newPos = toCoord(str[2], str[3]);

            // 遍歷 f 陣列取得移動的棋子索引
            for (int i = 0; i < 32; i++)
                if (f[i].getPosition() == oldPos)
                    n = i;

            ///// 棋子移動動畫 ///////
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

            // 移動棋子
            moveChess(str);

            // 在 position 中添加移動紀錄
            position += str + " ";

            // 將目標棋子移動
            f[n].setPosition(newPos);
        }

        // 如果isMove為真 則將 Spirit f[n] 的位置設為滑鼠位置減去偏移量 (dx 和 dy)
        if (isMove)
            f[n].setPosition(pos.x - dx, pos.y - dy);

        //// draw ////
        // 進行重製
        window.clear();

        // 繪製棋盤
        window.draw(sBoard);

        // 調整棋子偏移位置
        for (int i = 0; i < 32; i++)
            f[i].move(offset);

        // 將 f 陣列中的 32 個 Sprite 物件分別繪製到視窗上
        for (int i = 0; i < 32; i++)
            window.draw(f[i]);

        // 調整棋子偏移位置
        for (int i = 0; i < 32; i++)
            f[i].move(-offset);

        // 顯示繪製完成的作品
        window.display();
    }

    // 關閉與西洋棋引擎的連結
    CloseConnection();

    return 0;
}