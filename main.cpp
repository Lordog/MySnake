#include <iostream>
#include <conio.h>
#include <graphics.h>
#include <cstdlib>
#include <windows.h>
#include <ctime>
using namespace std;
enum Mode { Easy, Medium, Difficult };//选择障碍物数量
enum Pace{Slow, Average, High};//选择速度模式
enum Wall {Yes,No};//能否穿墙

bool Two = false;//true for 双人， false for 单人
int result=3;//双人游戏的结果
Mode mode = Medium;
Pace pace = Average;
Wall wall = Yes;

int Points = 0;//单人模式的分数/双人模式中彩蛇的分数
int Points1 = 0;//双人模式中白蛇的分数
int width = 15;//规定的像素值
clock_t start, stop;//计时器
int map[64][48] = {0};//画布，用以标记障碍坐标，判断蛇是否碰到
//构成蛇身的节点，结构体
struct Body
{
	int x;
	int y;
	int dir;//每个节点都有一个方向

	Body* front;//指向前一节点的指针
	Body* next;//指向后一节点的指针
	
    //默认构造函数
	Body(int a = 0, int b = 0, int d = 1, Body* pf = NULL, Body* pn = NULL) :x(a), y(b), dir(d), front(pf), next(pn) {}

	//绘制节点（白色）
	void draw1()
	{
		setcolor(WHITE);
		rectangle(x * width, y * width, (x + 1) * width, (y + 1) * width);
	}

	//绘制节点（闪烁彩色）
	void draw2()
	{
		int a = rand() % 256;
		int b = rand() % 256;
		int c = rand() % 256;
		setfillcolor(RGB(a, b, c));
		fillrectangle(x * width, y * width, (x + 1) * width, (y + 1) * width);
	}
	void clear1()
	{
		setcolor(DARKGRAY);
		rectangle(x * width, y * width, (x + 1) * width, (y + 1) * width);
	}
	void clear2()
	{
		setfillcolor(DARKGRAY);
		setlinecolor(DARKGRAY);
		fillrectangle(x * width, y * width, (x + 1) * width, (y + 1) * width);
	}
};
//基类/抽象类，，，其实没必要
class Object
{
public:
	Object() {}
	virtual void draw() = 0;
};

//食物类
class Food : public Object
{
public:
	Food()
	{
		reset();
	}
	void clear()
	{
		setfillcolor(DARKGRAY);
		fillcircle(x * width, y * width, radius * width);
	}
	void draw()
	{
		int a = rand() % 256;
		int b = rand() % 256;
		int c = rand() % 256;
		setfillcolor(RGB(a, b, c));
		setlinecolor(RGB(a, b, c));
		fillcircle(x * width, y * width, radius * width);
	}
	void reset()
	{
		srand(time(0));
		int frequency = rand() % 5;
		int randx, randy;
		if (frequency == 0)
		{
			radius = 1;
			do
			{
				randx = rand() % 63 + 1;
				randy = rand() % 47 + 1;
			} while (map[randx][randy] && map[randx - 1][randy - 1] && map[randx - 1][randy] && map[randx][randy - 1]);
			x = randx;
			y = randy;
			lastingTime = 60;
		}
		else
		{
			radius = 0.5;
			do
			{
				randx = rand() % 64;
				randy = rand() % 48;
			} while (map[randx][randy]);
			x = 0.5 + randx;
			y = 0.5 + randy;
			lastingTime = 100;
		}
	}



	//持续时间判断，如果持续时间到了，食物重置
	void last(int& step)
	{
		if (step == lastingTime)
		{
			step = 0;
			clear();
			reset();
		}
	}
	//返回食物的左上角点对应x坐标
	int getx()
	{
		return x - radius;
	}
	//返回食物的左上角点对应y坐标
	int gety()
	{
		return y - radius;
	}
	double getRadius()
	{
		return radius;
	}
private:
	int lastingTime;
	double radius;
	double x;//圆心x
	double y;//圆心y
};

//class 蛇
class Snake : public Object
{
public:
	Snake()
	{
		length = rand() % 3 + 3;//随机生成长度
		direction = rand() % 4 + 1;//随机生成方向						   
        //随机生成蛇头坐标
		int x, y;
		switch (direction)
		{
		case 1:
			x = rand() % (63 - length) + 1;
			y = rand() % 48;
			break;
		case 2:
			x = rand() % (62 - length);
			y = rand() % 48;
			break;
		case 3:
			x = rand() % 64;
			y = rand() % (47 - length) + 1;
			break;
		case 4:
			x = rand() % 64;
			y = rand() % (46 - length);
			break;
		default:
			break;
		}

		head = new Body(x, y, direction);
		map[x][y] = 2;//将生成节点的位置记为2，保证障碍不会生成在蛇的初始位置，下同
		Body* p = head;
		Body* pp;
		for (int i = 1; i < length - 1; i++)
		{
			switch (direction)
			{
			case 1:
				pp = new Body(x + i, y, direction, p);
				map[x + i][y] = 2;
				p->next = pp;
				p = pp;
				break;
			case 2:
				pp = new Body(x - i, y, direction, p);
				map[x - i][y] = 2;
				p->next = pp;
				p = pp;
				break;
			case 3:
				pp = new Body(x, y + i, direction, p);
				map[x][y + i] = 2;
				p->next = pp;
				p = pp;
				break;
			case 4:
				pp = new Body(x, y - i, direction, p);
				map[x][y - i] = 2;
				p->next = pp;
				p = pp;
				break;
			default:
				break;
			}
		}

		switch (direction)
		{
		case 1:
			pp = new Body(x + (length - 1), y, direction, p);
			map[x + (length - 1)][y] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 2:
			pp = new Body(x - (length - 1), y, direction, p);
			map[x - (length - 1)][y] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 3:
			pp = new Body(x, y + (length - 1), direction, p);
			map[x][y + (length - 1)] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 4:
			pp = new Body(x, y - (length - 1), direction, p);
			map[x][y - (length - 1)] = 2;
			p->next = pp;
			rear = pp;
			break;
		default:
			break;
		}
	}

	//清楚蛇的上一状态，实现移动效果
	void clear()
	{
		Body* pdraw = head;
		for (int i = 0; i < length; i++)
		{
			pdraw->clear2();
			pdraw = pdraw->next;
		}
	}
	//绘制蛇
	void draw()
	{
		Body* pdraw = head;
		for (int i = 0; i < length; i++)
		{
			pdraw->draw2();
			pdraw = pdraw->next;
		}
	}
	/*void draw1()
	{
		Body* pdraw = head;
		setcolor(WHITE);
		for (int i = 0; i < length; i++)
		{
			pdraw->draw();
			pdraw = pdraw->next;
		}
	}*/

	//移动的实现，核心代码
	//头节点根据方向direction移动,某节点的状态等于前一节点的上一状态
	void move()
	{
		Body* p = rear;
		for (int i = 0; i < length - 1; i++)
		{
			p->x = p->front->x;
			p->y = p->front->y;
			p->dir = p->front->dir;
			p = p->front;
		}
		switch (direction)
		{
		case 1:
			(head->x)--;
			break;
		case 2:
			(head->x)++;
			break;
		case 3:
			(head->y)--;
			break;
		case 4:
			(head->y)++;
			break;
		default:
			break;
		}
		if (wall == Yes)
		{
			if (head->x == 65)head->x = 0;
			if (head->y == 49)head->y = 0;
			if (head->x == -1)head->x = 63;
			if (head->y == -1)head->y = 47;
		}
	}

	//生长发育
	void grow(int n = 1)
	{
		length += n;
		for (int i = 0; i < n; i++)
		{
			Body* p;
			switch (direction)
			{
			case 1://x-1
				p = new Body(rear->x + 1, rear->y, rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 2://x+1
				p = new Body(rear->x - 1, rear->y, rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 3://y-1
				p = new Body(rear->x, (rear->y + 1), rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 4://y+1
				p = new Body(rear->x, (rear->y - 1), rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			default:
				break;
			}
		}
	}

	//改变方向
	void changeDirection(char qaq)
	{
		int dir;
		switch (qaq)
		{
		case 'A':
		case 'a':
			dir = 1;
			break;
		case 'D':
		case 'd':
			dir = 2;
			break;
		case 'W':
		case 'w':
			dir = 3;
			break;
		case 'S':
		case 's':
			dir = 4;
			break;
		case 27:
			_getch();
			dir = 0;
			return;
		default:
			return;
		}
		if (dir == direction)return;
		if (direction == 1 && dir == 2)return;
		if (direction == 2 && dir == 1)return;
		if (direction == 3 && dir == 4)return;
		if (direction == 4 && dir == 3)return;
		else
		{
			direction = dir;
			head->dir = direction;
		}
	}
	//检验，是否撞墙，是否吃到自己
	bool examine()
	{
		int x = head->x;
		int y = head->y;
		Body* p = head->next;
		//检验是否吃到自己
		for (int i = 0; i < length - 1; i++)
		{
			if (x == p->x && y == p->y)
			{
				_getch();
				return false;
			}
			p = p->next;
		}

		//检验是否撞到障碍
		if (map[x][y] == 1)
		{
			_getch();
			return false;
		}

		//能否穿墙
		if (wall == No)
		{
			if (head->x == 65 || head->y == 49 || head->x == -1 || head->y == -1)
			{
				_getch();
				return false;
			}
		}
		return true;
	}
	//判断是否吃到食物
	void examineEat(Food& f)
	{
		double r = f.getRadius();
		int x = f.getx();
		int y = f.gety();
		if (r == 0.5 && x == head->x && y == head->y)
		{
			Points++;
			f.clear();
			grow();
			f.reset();
		}
		else if (r == 1)
		{
			if (x == head->x && y == head->y || x + r == head->x && y + r == head->y || x == head->x && y + r == head->y || x + r == head->x && y == head->y)
			{
				Points += 2;
				f.clear();
				grow();
				f.reset();
			}
		}
	}
	//提供一个函数设定屏幕上蛇的移动速度（停顿时间），瞎写的，分数越高，速度越快，但速度增加的速度减慢
	int timeVersusPoints()
	{
		//提供一个函数设定屏幕上蛇的移动速度（停顿时间）
		int time;
		//为不同模式提供不同的函数
		if (pace == Slow)
		{
			if (Points < 30)
				time = -10 / 3 * Points + 300;
			else if (Points < 100)
			{
				time = -1.5 * Points + 245;
			}
			else
			{
				time = 10000 / Points;
			}
		}
		if (pace == Average)
		{
			if (Points < 50)
				time = -2 * Points + 200;
			else if (Points < 100)
			{
				time = -1 / 2 * Points + 125;
			}
			else
			{
				time = 7500 / Points;
			}
		}
		if (pace == High)
		{
			if (Points < 100)
				time = -1 / 2 * Points + 100;
			else
			{
				time = 5000 / Points;
			}
		}
		return time;
	}
	//返回蛇身长度
	int getLength()
	{
		return length;
	}

	//头节点的指针
	Body* head;
	//尾节点的指针
	Body* rear;
private:
	int length;
	int direction;//1← 2→ 3↑ 4↓
};

//双人模式中的白蛇，基本同Snake一样
class Snake1 : public Object
{
public:
	Snake1()
	{
		length = rand() % 3 + 3;//随机生成长度
		direction = rand() % 4 + 1;//随机生成方向
		int x, y;
		switch (direction)
		{
		case 1:
			x = rand() % (63 - length) + 1;
			y = rand() % 48;
			break;
		case 2:
			x = rand() % (62 - length);
			y = rand() % 48;
			break;
		case 3:
			x = rand() % 64;
			y = rand() % (47 - length) + 1;
			break;
		case 4:
			x = rand() % 64;
			y = rand() % (46 - length);
			break;
		default:
			break;
		}
		head = new Body(x, y, direction);
		map[x][y] = 2;
		Body* p = head;
		Body* pp;
		for (int i = 1; i < length - 1; i++)
		{
			switch (direction)
			{
			case 1:
				pp = new Body(x + i, y, direction, p);
				map[x + i][y] = 2;
				p->next = pp;
				p = pp;
				break;
			case 2:
				pp = new Body(x - i, y, direction, p);
				map[x - i][y] = 2;
				p->next = pp;
				p = pp;
				break;
			case 3:
				pp = new Body(x, y + i, direction, p);
				map[x][y + i] = 2;
				p->next = pp;
				p = pp;
				break;
			case 4:
				pp = new Body(x, y - i, direction, p);
				map[x][y - i] = 2;
				p->next = pp;
				p = pp;
				break;
			default:
				break;
			}
		}

		switch (direction)
		{
		case 1:
			pp = new Body(x + (length - 1), y, direction, p);
			map[x + (length - 1)][y] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 2:
			pp = new Body(x - (length - 1), y, direction, p);
			map[x - (length - 1)][y] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 3:
			pp = new Body(x, y + (length - 1), direction, p);
			map[x][y + (length - 1)] = 2;
			p->next = pp;
			rear = pp;
			break;
		case 4:
			pp = new Body(x, y - (length - 1), direction, p);
			map[x][y - (length - 1)] = 2;
			p->next = pp;
			rear = pp;
			break;
		default:
			break;
		}
	}
	void clear()
	{
		Body* pdraw = head;
		for (int i = 0; i < length; i++)
		{
			pdraw->clear2();
			pdraw = pdraw->next;
		}
	}
	void draw()
	{
		Body* pdraw = head;
		for (int i = 0; i < length; i++)
		{
			pdraw->draw1();
			pdraw = pdraw->next;
		}
	}
	/*void draw1()
	{
		Body* pdraw = head;
		setcolor(WHITE);
		for (int i = 0; i < length; i++)
		{
			pdraw->draw();
			pdraw = pdraw->next;
		}
	}*/
	void move()
	{
		Body* p = rear;
		for (int i = 0; i < length - 1; i++)
		{
			p->x = p->front->x;
			p->y = p->front->y;
			p->dir = p->front->dir;
			p = p->front;
		}
		switch (direction)
		{
		case 1:
			(head->x)--;
			break;
		case 2:
			(head->x)++;
			break;
		case 3:
			(head->y)--;
			break;
		case 4:
			(head->y)++;
			break;
		default:
			break;
		}
		if (wall == Yes)
		{
			if (head->x == 65)head->x = 0;
			if (head->y == 49)head->y = 0;
			if (head->x == -1)head->x = 63;
			if (head->y == -1)head->y = 47;
		}
	}
	void grow(int n = 1)
	{
		length += n;
		for (int i = 0; i < n; i++)
		{
			Body* p;
			switch (direction)
			{
			case 1://x-1
				p = new Body(rear->x + 1, rear->y, rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 2://x+1
				p = new Body(rear->x - 1, rear->y, rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 3://y-1
				p = new Body(rear->x, (rear->y + 1), rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			case 4://y+1
				p = new Body(rear->x, (rear->y - 1), rear->dir, rear);
				rear->next = p;
				rear = p;
				break;
			default:
				break;
			}
		}
	}
	void changeDirection(char p)
	{
		int dir;
		switch (p)
		{
		case '1':
			dir = 1;
			break;
		case '3':
			dir = 2;
			break;
		case '5':
			dir = 3;
			break;
		case '2':
			dir = 4;
			break;
		case 27:
			_getch();
			dir = 0;
			return;
		default:
			return;
		}
		if (dir == direction)return;
		if (direction == 1 && dir == 2)return;
		if (direction == 2 && dir == 1)return;
		if (direction == 3 && dir == 4)return;
		if (direction == 4 && dir == 3)return;
		else
		{
			direction = dir;
			head->dir = direction;
		}
	}
	bool examine()
	{
		int x = head->x;
		int y = head->y;
		Body* p = head->next;
		for (int i = 0; i < length - 1; i++)
		{
			if (x == p->x && y == p->y)
			{
				_getch();
				return false;
			}
			p = p->next;
		}
		if (map[x][y] == 1)
		{
			_getch();
			return false;
		}
		if (wall == No)
		{
			if (head->x == 65 || head->y == 49 || head->x == -1 || head->y == -1)
			{
				_getch();
				return false;
			}
		}
		return true;
	}
	void examineEat(Food& f)
	{
		double r = f.getRadius();
		int x = f.getx();
		int y = f.gety();
		if (r == 0.5 && x == head->x && y == head->y)
		{
			Points++;
			clear();
			grow();
			f.reset();
		}
		else if (r == 1)
		{
			if (x == head->x && y == head->y || x + r == head->x && y + r == head->y || x == head->x && y + r == head->y || x + r == head->x && y == head->y)
			{
				Points += 2;
				clear();
				grow();
				f.reset();
			}
		}
	}
	int timeVersusPoints()
	{
		//提供一个函数设定屏幕上蛇的移动速度（停顿时间）
		int time;
		if (pace == Slow)
		{
			if (Points < 30)
				time = -10 / 3 * Points + 300;
			else if (Points < 100)
			{
				time = -1.5 * Points + 245;
			}
			else
			{
				time = 10000 / Points;
			}
		}
		if (pace == Average)
		{
			if (Points < 50)
				time = -2 * Points + 200;
			else if (Points < 100)
			{
				time = -1 / 2 * Points + 125;
			}
			else
			{
				time = 7500 / Points;
			}
		}
		if (pace == High)
		{
			if (Points < 100)
				time = -1 / 2 * Points + 100;
			else
			{
				time = 5000 / Points;
			}
		}
		return time;
	}
	int getLength()
	{
		return length;
	}

	Body* head;
	Body* rear;
private:
	int length;
	int direction;//1← 2→ 3↑ 4↓
};



//障碍类
class Barrier : public Object
{
public:
	Barrier()
	{
		//Mode a = Medium;
		//Easy生成4-6组 3
		//Medium生成8-10组 3
		//Difficult生成11-15；5
		int a = 8;
		int b = 6;
		if (mode == Easy)
		{
			return;
		}
		else if (mode == Average)
		{
		    a = 10;
			b = 6;
		}
		else if (mode == Difficult)
		{
			a = 14;
			b = 8;
		}
		int n = rand() % 3 + a;//生成n组Barrier
		int cnt;//每组生成几个块 5-8
		int x;
		int y;
		int rowOrCol;//生成列块还是行块
		for (int i = 0; i < n; i++)
		{
			rowOrCol = rand() % 2;
			cnt = rand() % 6 + b;
			if (rowOrCol == 1)//生成行块
			{
				x = rand() % (65 - cnt);
				y = rand() % 48;
				setfillcolor(RGB(30, 30, 30));
				setlinecolor(RGB(50, 50, 50));
				for (int j = 0; j < cnt; j++)
				{
					if (!map[x + j][y])fillrectangle((x + j) * width, y * width, (x + j + 1) * width, (y + 1) * width);
					map[x + j][y] = 1;
				}
			}
			else
			{
				x = rand() % 64;
				y = rand() % (49 - cnt);
				setfillcolor(RGB(30, 30, 30));
				setlinecolor(RGB(50, 50, 50));
				for (int j = 0; j < cnt; j++)
				{
					if (!map[x][y + j])fillrectangle(x * width, (y + j) * width, (x + 1) * width, (y + j + 1) * width);
					map[x][y + j] = 1;
				}
			}
		}
	}
	//两个无用函数，但是不重写的话，Barrier就是抽象类，无法实例化
	void judge()
	{

	}
	void draw()
	{
	}
private:
	int number;//障碍有几组
};

//判断两条蛇是否相碰
//1表示 彩蛇Win
//2表示 白蛇Win
//3表示 都活着
//4表示 同归于尽
int judge(Snake s1, Snake1 s2)
{
	int x1 = s1.head->x;
	int y1 = s1.head->y;
	int x2 = s2.head->x;
	int y2 = s2.head->y;
	Body* p1 = s1.head->next;
	Body* p2 = s2.head->next;
	if (x1 == x2 && y1 == y2)return 4;
	for (int i = 0; i < s1.getLength() - 1; i++)
	{
		if (x2 == p1->x && y2 == p1->y)
		{
			return 1;
		}
		p1 = p1->next;
	}
	for (int i = 0; i < s2.getLength() - 1; i++)
	{
		if (x1 == p2->x && y1 == p2->y)
		{
			return 2;
		}
		p2 = p2->next;
	}
	return 3;
}

//在右上角显示  分数、长度、模式、速度、能否穿墙
//单人模式print
void print(Snake s)
{
	TCHAR s1[] = _T("Points: ");
	outtextxy(876, 10, s1);
	TCHAR x1[5];
	_stprintf_s(x1, _T("%d"), Points);
	outtextxy(928, 10, x1);

	//长度
	TCHAR s2[] = _T("Length: ");
	outtextxy(876, 30, s2);
	TCHAR x2[5];
	_stprintf_s(x2, _T("%d"), s.getLength());
	outtextxy(928, 30, x2);

	//模式
	TCHAR s3[] = _T("Mode: ");
	outtextxy(876, 70, s3);

	if (mode == 0)
	{
		TCHAR x3[] = _T("Easy");
		outtextxy(920, 70, x3);
	}
	else if (mode == 1)
	{
		TCHAR x3[] = _T("Medi");
		outtextxy(920, 70, x3);
	}
	else if (mode == 2)
	{
		TCHAR x3[] = _T("Diff");
		outtextxy(920, 70, x3);
	}

	//速度
	TCHAR s4[] = _T("Pace: ");
	outtextxy(876, 90, s4);

	if (pace == Slow)
	{
		TCHAR x4[] = _T("Slow");
		outtextxy(920, 90, x4);
	}
	else if (pace == Average)
	{
		TCHAR x4[] = _T("Avera");
		outtextxy(920, 90, x4);
	}
	else if (mode == High)
	{
		TCHAR x4[] = _T("High");
		outtextxy(920, 90, x4);
	}
}
//双人模式print
void print1(Snake ss,Snake1 s)
{
	TCHAR s1[] = _T("Points: ");
	outtextxy(876, 10, s1);
	TCHAR x1[5];
	_stprintf_s(x1, _T("%d"), Points1);
	outtextxy(928, 10, x1);

	//长度
	TCHAR s2[] = _T("Length: ");
	outtextxy(876, 30, s2);
	TCHAR x2[5];
	_stprintf_s(x2, _T("%d"), s.getLength());
	outtextxy(928, 30, x2);

	TCHAR s5[] = _T("Points: ");
	outtextxy(10, 10, s5);
	TCHAR x5[5];
	_stprintf_s(x5, _T("%d"), Points);
	outtextxy(62, 10, x5);

	//长度
	TCHAR s6[] = _T("Length: ");
	outtextxy(10, 30, s6);
	TCHAR x6[5];
	_stprintf_s(x6, _T("%d"), ss.getLength());
	outtextxy(62, 30, x6);


	//模式
	TCHAR s3[] = _T("Mode: ");
	outtextxy(876, 70, s3);

	if (mode == 0)
	{
		TCHAR x3[] = _T("Easy");
		outtextxy(920, 70, x3);
	}
	else if (mode == 1)
	{
		TCHAR x3[] = _T("Medi");
		outtextxy(920, 70, x3);
	}
	else if (mode == 2)
	{
		TCHAR x3[] = _T("Diff");
		outtextxy(920, 70, x3);
	}

	//速度
	TCHAR s4[] = _T("Pace: ");
	outtextxy(876, 90, s4);

	if (pace == Slow)
	{
		TCHAR x4[] = _T("Slow");
		outtextxy(920, 90, x4);
	}
	else if (pace == Average)
	{
		TCHAR x4[] = _T("Avera");
		outtextxy(920, 90, x4);
	}
	else if (mode == High)
	{
		TCHAR x4[] = _T("High");
		outtextxy(920, 90, x4);
	}
}


//在右上角显示已用时间
void printTime()
{
	stop = clock();
	int duration = (int)(stop - start) / CLOCKS_PER_SEC;
	int minute = duration / 60;
	int	second = duration % 60;

	TCHAR s1[] = _T("Time: ");
	outtextxy(876, 50, s1);
	TCHAR x1[3];
	_stprintf_s(x1, _T("%d"), minute);
	outtextxy(916, 50, x1);

	TCHAR s2[] = _T(":");
	outtextxy(933, 50, s2);

	TCHAR x2[3];
	_stprintf_s(x2, _T("%d"), second);
	outtextxy(938, 50, x2);

}
//game单人
void game()
{
	Snake s;
	Barrier b;
	Food f;

	//蜜汁操作，体现多态性能
	Object* pob[2];
	pob[0] = &s;
	pob[1] = &f;

	BeginBatchDraw();
	int step = 1;//步数差计算食物持续时间

	start = clock();
	char c;
	while (1)
	{
		c = '0';
		if (_kbhit())
			c = _getch();
		s.changeDirection(c);

		step++;
		s.move();

		if (!s.examine())
			return;
		//f.examine(s);
		s.examineEat(f);
		f.last(step);
		
		print(s);
		printTime();
		//s.draw();
		pob[0]->draw();
		//f.draw();
		pob[1]->draw();
		FlushBatchDraw();

		Sleep(s.timeVersusPoints());

		s.clear();
	}
}
//game1双人
void game1()
{
	Snake s1;
	Snake1 s2;
	//barrier();
	Barrier b;
	Food f;
	//s.draw1();
	BeginBatchDraw();
	int step = 1;//步数差计算食物持续时间
	//int time = 100;
	start = clock();
	char c;
	while (1)
	{
		c = '0';
		if (_kbhit())
			c = _getch();


		s1.changeDirection(c);
		s2.changeDirection(c);

		step++;
		s1.move();
		s2.move();

		if (!s1.examine() && !s2.examine())
		{
			result = 4;
		}
		else if (!s1.examine())
		{
			result = 2;
			return;
		}
		else if (!s2.examine())
		{
			result = 1;
			return;
		}

		s1.examineEat(f);
		s2.examineEat(f);

		
		result = judge(s1, s2);
		if (result != 3)return;
		f.last(step);
		print1(s1,s2);
		printTime();
		s1.draw();
		f.draw();
		s2.draw();
		FlushBatchDraw();

		Sleep(s1.timeVersusPoints());

		s1.clear();
		s2.clear();
	}
}
//选择模式
void choose()
{
	TCHAR s0[] = _T("Rules:");
	outtextxy(420, 240, s0);

	

	TCHAR s1[] = _T("Mode (define the amount of barriers, 1 for easy, 2 for medium, 3 for difficult)");
	outtextxy(230, 260, s1);
	TCHAR s2[] = _T("Pace (define the speed of snake, 4 for slow, 5 for average, 6 for high)");
	outtextxy(230, 280, s2);
	TCHAR s3[] = _T("Wall (Can the snake cross boarder, 7 for yes, 8 for no)");
	outtextxy(250, 300, s3);

	TCHAR s6[] = _T("9 for single, 10 for double");
	outtextxy(320, 320, s6);

	TCHAR s4[] = _T("Cin in order with keyboard ( for example: 256 ), then click Enter");
	outtextxy(240, 340, s4);

	TCHAR s5[] = _T("In the game, click Esc for Pulse");
	outtextxy(320, 360, s5);


	char mmode, ppace, wwall,two;
		mmode = _getch();
		ppace = _getch();
		wwall = _getch();
		two = _getch();
		switch (mmode)
		{
		case '1':
			mode = Easy;
			break;
		case '2':
			mode = Medium;
			break;
		case '3':
			mode = Difficult;
			break;
		default:
			break;
		}
		switch (ppace)
		{
		case '4':
			pace = Slow;
			break;
		case '5':
			pace = Average;
			break;
		case '6':
			pace = High;
			break;
		default:
			break;
		}
		switch (wwall)
		{
		case '7':
			wall = Yes;
			break;
		case '8':
			wall = No;
			break;
		default:
			break;
		}
		if (two == '9')Two = false;
		else if (two == '0')Two = true;
		two = _getch();
		cleardevice();
}
//游戏结束界面的显示（单人）
void theEnd()
{
	settextstyle(25, 0, _T("黑体"));
	TCHAR s1[] = _T("Sorry, you died!");
	outtextxy(400, 340, s1);
}
//双人
void theEnd1()
{
	settextstyle(25, 0, _T("黑体"));
	TCHAR s1[] = _T("The left snake win!");
	TCHAR s2[] = _T("The right snake win!");
	TCHAR s3[] = _T("Sorry, both of you died!");
	switch (result)
	{
	case 1:
		outtextxy(400, 340, s1);
		break;
	case 2:
		outtextxy(400, 340, s2);
		break;
	case 4:
		outtextxy(400, 340, s3);
		break;
	default:

		break;
	}
}
int main()
{
    initgraph(960, 720);
	setbkcolor(DARKGRAY);
	cleardevice();

	choose();

	srand(time(0));
	if (Two)
	{
		game1();
		_getch();
		EndBatchDraw();
		cleardevice();
		theEnd1();
	}
	else
	{
		game();
		_getch();
		EndBatchDraw();
		cleardevice();
		theEnd();
	}

	_getch();
    closegraph();
    return 0;
}

