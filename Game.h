#ifndef GAME_H
#define GAME_H
#include <iostream>
#include <QThread>
#include <QProgressDialog>
#include "Status.h"
#include <stack>

using namespace std;

class Game : public QThread
{

    Q_OBJECT

private:
    Status startStatus;
    bool **wall;
    bool **isAim;
    Status player;
public:
    int **mp;
    int NowPoint;
    bool isGold;
    bool isBomb;
    bool isyellow;
    Point YellowP;
    Point GoldP;
    Point BombP;
    static const int WALL;
    static const int AIM;
    static const int MAN;
    static const int BOX;
    static const int YWall;
    static const int Bomb;
    static const int GOLD;
    static const int RIGHT;
    static const int LEFT;
    static const int DOWN;
    static const int UP;
	static const int mx[];
	static const int my[];
    static const char mv[];
    int minGH;
    const int n;
    const int m;
    vector<Point> aim;
    stack<Status> oldStatus;
    Game(int N,int M,string s);
    ~Game();
    vector<int> ans;
    const vector<int>& solve();
    void restart();
    int getH();
    bool isWin();
    void move(int k);
    void goBack();
    vector<int> move(int x,int y);
    void run();
    int getStep();
    int getNowP();
};

#endif // GAME_H
