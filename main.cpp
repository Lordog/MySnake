#include <iostream>
#include <conio.h>
#include <graphics.h>
#include <cstdlib>
#include <windows.h>
#include <ctime>
using namespace std;
enum Mode { Easy, Medium, Difficult };//选择障碍物数量
enum Pace{Low, Average, High};//选择速度模式
enum WallOrNot {wall,wudi};//能否穿墙
//考虑增加穿墙模式
//自定义背景和蛇颜色
bool Win = 1;
int Points = 0;
int width = 15;
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
						if(!map[x + j][y])fillrectangle((x + j) * width, y * width, (x + j + 1) * width, (y + 1) * width);
						map[x + j][y] = 1;
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
						if(!map[x][y+j])fillrectangle(x * width, (y + j) * width, (x + 1) * width, (y + j + 1) * width);
						map[x][y + j] = 1;
					}
				}
		}
	}
	void judge()
	{

	}
private:
};
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
		map[x][y] = 1;
		Body* p=head;
		Body* pp;
		for (int i = 1; i < length - 1; i++)
		{
			switch (direction)
			{
			case 1:
				pp = new Body(x + i, y, direction, p);
				map[x + i][y] = 1;
				p->next = pp;
				p = pp;
				break;
			case 2:
				pp = new Body(x - i, y, direction, p);
				map[x - i][y] = 1;
				p->next = pp;
				p = pp;
				break;
			case 3:
				pp = new Body(x, y + i, direction, p);
				map[x][y + i] = 1;
				p->next = pp;
				p = pp;
				break;
			case 4:
				pp = new Body(x, y - i, direction, p);
				map[x][y - i] = 1;
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
			map[x + (length - 1)][y] = 1;
			p->next = pp;
			rear = pp;
			break;
		case 2:
			pp = new Body(x - (length - 1), y, direction, p);
			map[x - (length - 1)][y] = 1;
			p->next = pp;
			rear = pp;
			break;
		case 3:
			pp = new Body(x, y + (length - 1), direction, p);
			map[x][y + (length - 1)] = 1;
			p->next = pp;
			rear = pp;
			break;
		case 4:
			pp = new Body(x, y - (length - 1), direction, p);
			map[x][y - (length - 1)] = 1;
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
	void examine()
	{
		int x = head->x;
		int y = head->y;
		Body* p = head->next;
		for (int i = 0; i < length - 1; i++)
		{
			if (x == p->x && y == p->y)
			{
				Sleep(2000);
				break;
			}
			p = p->next;
		}
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
		cnt = rand() % 4 + 8;
		if (rowOrCol==1)//生成行块
		{
			x = rand() % (65 - cnt);
			y = rand() % 48;
			setfillcolor(LIGHTGRAY);
			setlinecolor(DARKGRAY);
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
			setfillcolor(LIGHTGRAY);
			setlinecolor(DARKGRAY);
			for (int j = 0; j < cnt; j++)
			{
				if (!map[x][y + j])fillrectangle(x * width, (y + j) * width, (x + 1) * width, (y + j + 1) * width);
				map[x][y + j] = 1;
			}
		}
	}
}


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
void game()
{
	Snake s;
	barrier();
	Food f;
	//s.draw1();
	BeginBatchDraw();
	int cnt= 1;
	int time = 200;
	while (Win)
	{
		c = '0';
		if (_kbhit())
			c = _getch();
		s.changeDirection(c);
		cnt++;

		s.move();


		//judge();
		s.examine();

		if (f.radius == 0.5 && f.x - f.radius == s.head->x && f.y - f.radius == s.head->y)
		{
			f.clear();
			s.grow();
			f.reset();
		}
		else if (f.radius == 1)
		{
			if (f.x - f.radius == s.head->x && f.y - f.radius == s.head->y || f.x == s.head->x && f.y == s.head->y || f.x - f.radius == s.head->x && f.y == s.head->y || f.x == s.head->x && f.y - f.radius == s.head->y)
			{
				f.clear();
				s.grow();
				f.reset();
			}
		}
		
		
		s.draw();
		//if (n % 6 == 0)s.grow();
		f.draw();
		//if(n % 12==0)f.clear();
		FlushBatchDraw();
		
		Sleep(time);
		
		s.clear();
	}
}
int main()
{
    initgraph(960, 720);
	setbkcolor(DARKGRAY);
	cleardevice();
	srand(time(0));
	//Food f;
	//f.draw();
	game();
	//fillcircle(62*15, 45*15,  0.5* width);
	//cout << "???" << endl;*/
	Sleep(2000);
    //f.clear();
	//EndBatchDraw();
    closegraph();
    return 0;
}