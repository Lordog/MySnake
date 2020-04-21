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
//考虑增加穿墙模式
//自定义背景和蛇颜色
bool Win = 1;
Mode mode = Medium;
Pace pace = Average;
Wall wall = Yes;
int Points = 10;
int width = 15;
clock_t start, stop;
int map[64][48] = {0};//方便整坐标
class Object
{
public:
	Object(){}
	virtual void draw() = 0;
private:
};
/*class Wall : public Object
{
public:
private:
};*/
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
		fillcircle(x*width, y*width, radius*width);
	}
	void reset()
	{
		srand(time(0));
		int frequency = rand() % 5;
		int randx, randy;
		if (frequency==0)
		{
			radius = 1;
			do
			{
				randx = rand() % 63 + 1;
				randy = rand() % 47 + 1;
			} while (map[randx][randy]&& map[randx-1][randy-1]&& map[randx-1][randy]&& map[randx][randy-1]);
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
	int lastingTime;
	double radius;
	double x;
	double y;
};
/*class Bonus : public Food
{
public:
	Bonus(){}
};*/
class Barrier : public Object
{
public:
	Barrier()
	{
		//Mode a = Medium;
		//Easy生成4-6组 3
		//Medium生成8-10组 3
		//Difficult生成11-15；5
		int n = rand() % 3 + 4;//生成n组Barrier
		int cnt;//每组生成几个块 5-8
		int x;
		int y;
		int rowOrCol;//生成列块还是行块
		for (int i = 0; i < n; i++)
		{
				rowOrCol = rand() % 2;
				cnt = rand() % 4 + 5;
				if (rowOrCol)//生成行块
				{
					x = rand() % (65 - cnt);
					y = rand() % 48;
					setfillcolor(BLACK);
					setlinecolor(BLACK);
					for (int j = 0; j < cnt; j++)
					{
						if (map[x + j][y] != 2)
						{
							fillrectangle((x + j) * width, y * width, (x + j + 1) * width, (y + 1) * width);
							map[x + j][y] = 1;
						}
					}
				}
				else
				{
					x = rand() % 64;
					y = rand() % (49 - cnt);
					setfillcolor(BLACK);
					setlinecolor(BLACK);
					for (int j = 0; j < cnt; j++)
					{
						if (!map[x][y + j])
						{
							fillrectangle(x * width, (y + j) * width, (x + 1) * width, (y + j + 1) * width);
							map[x][y + j] = 1;
						}
					}
				}
		}
	}
	void judge()
	{

	}
private:
};
//先生成蛇，再生成Barrier,判断，以避开蛇
void barrier()
{
	//Mode a = Medium;
		//Easy生成4-6组 3
		//Medium生成8-10组 3
		//Difficult生成11-15；5
	int n = rand() % 3 + 15;//生成n组Barrier
	int cnt;//每组生成几个块 5-8
	int x;
	int y;
	int rowOrCol;//生成列块还是行块
	for (int i = 0; i < n; i++)
	{
		rowOrCol = rand() % 2;
		cnt = rand() % 6 + 10;
		if (rowOrCol == 1)//生成行块
		{
			x = rand() % (65 - cnt);
			y = rand() % 48;
			setfillcolor(RGB(30,30,30));
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
class Space : public Object
{
public:
private:
};
/*class Body : public Object
{
public:
private:
	int x;
	int y;
};*/
struct Body
{
	int x;
	int y;
	int dir;

	Body* front;
	Body* next;
	Body(int a=0,int b=0,int d=1,Body* pf=NULL,Body* pn=NULL):x(a),y(b),dir(d),front(pf),next(pn){}

	void draw1()
	{
		setcolor(WHITE);
		rectangle(x*width,y*width, (x+1)*width, (y+1) * width);
	}

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

class Snake : public Object
{
public:
    Snake()
    {
		length = rand() % 3 + 3;
		direction = rand() % 4 + 1;
		int x, y;
		switch (direction)
		{
		case 1:
			x = rand() % (63 - length)+1;
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
		head = new Body(x,y,direction);
		map[x][y] = 2;
		Body* p=head;
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
			pp = new Body(x + (length-1), y, direction, p);
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
		if (head->x == 65)head->x = 0;
		if (head->y == 49)head->y = 0;
		if (head->x == -1)head->x = 63;
		if (head->y == -1)head->y = 47;
	}
	void grow(int n=1)
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
		return true;
	}
    Body* head;
	Body* rear;
    int speed;
    int length;
	//int x;
	//int y;
	int direction;//1← 2→ 3↑ 4↓
};


void init()
{

}//随机初始化界面

 


void draw(Object* pOb)
{
	pOb->draw();
}
/*void judge()
{
	//蛇身
	s.examine();
		//Food
	if (f.radius==0.5&&f.x - f.radius == s.head->x && f.y - f.radius == s.head->y)
	{
		f.clear();
		s.grow();
		f.reset();
	}
	else if (f.radius == 1)
	{
		if (f.x - f.radius == s.head->x && f.y - f.radius == s.head->y|| f.x == s.head->x && f.y == s.head->y|| f.x - f.radius == s.head->x && f.y == s.head->y|| f.x == s.head->x && f.y - f.radius == s.head->y)
		{
			f.clear();
			s.grow();
			f.reset();
		}
	}
	

	//Barrier
}*/
int timeVersusPoints()
{
	//提供一个函数
	return 1;
}
char c;
void print()
{
	TCHAR s1[] = _T("Points: ");
	outtextxy(876, 10, s1);
	TCHAR x1[5];
	_stprintf_s(x1, _T("%d"), Points);   
	outtextxy(928, 10, x1);
	

	TCHAR s2[] = _T("Length: ");
	outtextxy(876, 30, s2);
	TCHAR x2[5];
	_stprintf_s(x2, _T("%d"), Points);
	outtextxy(928, 30, x2);


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


	TCHAR s4[] = _T("Pace: ");
	outtextxy(876, 90, s4);

	if (pace == 0)
	{
		TCHAR x4[] = _T("Slow");
		outtextxy(920, 90, x4);
	}
	else if (pace == 1)
	{
		TCHAR x4[] = _T("Avera");
		outtextxy(920, 90, x4);
	}
	else if (mode == 2)
	{
		TCHAR x4[] = _T("High");
		outtextxy(920, 90, x4);
	}
	
}

void printTime()
{
	stop = clock();
	int duration = (int)(stop - start) / CLOCKS_PER_SEC;
	int minute = duration / 60;
	int	second = duration % 60;

	TCHAR s1[] = _T("Time: ");
	outtextxy(876, 50, s1);
	TCHAR x1[3];
	_stprintf_s(x1, _T("%d"), 12);
	outtextxy(916, 50, x1);

	TCHAR s2[] = _T(":");
	outtextxy(933, 50, s2);

	TCHAR x2[3];
	_stprintf_s(x2, _T("%d"), 54);
	outtextxy(938, 50, x2);

}
void game()
{
	Snake s;
	barrier();
	Food f;
	//s.draw1();
	BeginBatchDraw();
	int cnt = 1;
	int time = 100;
	start = clock();
	while (Win)
	{
		c = '0';
		if (_kbhit())
			c = _getch();
		s.changeDirection(c);
		cnt++;

		s.move();


	
		if (!s.examine())
			return;
		
		if (f.radius == 0.5 && f.x - f.radius == s.head->x && f.y - f.radius == s.head->y)
		{
			Points++;
			f.clear();
			s.grow();
			f.reset();
		}
		else if (f.radius == 1)
		{
			if (f.x - f.radius == s.head->x && f.y - f.radius == s.head->y || f.x == s.head->x && f.y == s.head->y || f.x - f.radius == s.head->x && f.y == s.head->y || f.x == s.head->x && f.y - f.radius == s.head->y)
			{
				Points += 2;
				f.clear();
				s.grow();
				f.reset();
			}
		}
		print();
		printTime();
		s.draw();
		f.draw();
		FlushBatchDraw();

		Sleep(time);

		s.clear();
	}
}
void choose()
{
	TCHAR s1[] = _T("Mode (define the amount of barriers, 1 for easy, 2 for medium, 3 for difficult)");
	outtextxy(200, 260, s1);
	TCHAR s2[] = _T("Pace (define the speed of snake, 4 for slow, 5 for average, 6 for high)");
	outtextxy(200, 280, s2);
	TCHAR s3[] = _T("Wall (Can the snake cross boarder, 7 for yes, 8 for no)");
	outtextxy(220, 300, s3);

	TCHAR s4[] = _T("Cin in order with keyboard ( for example: 256 ), then click Enter");
	outtextxy(210, 320, s4);

	char mmode, ppace, wwall;
		mmode = _getch();
		ppace = _getch();
		wwall = _getch();
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
		wwall = _getch();
		cleardevice();
}
int main()
{
    initgraph(960, 720);
	setbkcolor(DARKGRAY);
	cleardevice();

	choose();

	srand(time(0));
	game();
    //f.clear();
	EndBatchDraw();

	cleardevice();
	settextstyle(25, 0, _T("黑体"));
	TCHAR s1[] = _T("Sorry, you died!");
	outtextxy(400, 340, s1);
	_getch();
    closegraph();
    return 0;
}
