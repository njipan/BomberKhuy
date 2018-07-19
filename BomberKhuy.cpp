#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include<time.h>
#include<vector>
#include "khuy.h"

using namespace std;

int enemy_count = 100;
int delay_enemy_bomb = 150;
int total_enemy = 10;

class Enemy;
class Player;
class Node;
class Collection;
class Bomb;

Collection *collect;

class Node{
public:
	int x;
	int y;	
	int cost;
	int distance;
	bool isVisited;
	Node *parent;
	char symbol;
	
	Node(int x,int y){
		this->x = x;
		this->y = y;
		this->symbol = WALL;
		reset();
	}
	
	void reset(){
		this->cost = WALL_COST;
		this->distance = 0;
		this->isVisited = false;
		this->parent = NULL;
		
	}
	
	void copy(Node *temp){
		this->x = temp->x;
		this->y = temp->y;	
		this->cost = temp->cost;
		this->distance = temp->distance;
		this->isVisited = temp->isVisited;
		this->parent = temp->parent;
		this->symbol = temp->symbol;
	}
};

class VectorMinHeap{
private:
		
	int getParent(int child){
		return child / 2;
	}
	
	int getLeft(int parent){
		return parent * 2 + 1;
	}
	
	int getRight(int parent){
		return parent * 2 + 2;
	}
	
public:
	
	vector<Node*> nodes;
	
	void swap(Node *a, Node *b){
		Node c = *a;
		*a = *b;
		*b = c;
	}
	
	void printAll(){
		for(int i=0;i<nodes.size();i++){
			printf("distance: %d x: %d y: %d cost: %d\n",nodes[i]->distance,nodes[i]->x,nodes[i]->y,nodes[i]->cost);
		}
	}
	
	void heapify(){
		int size = nodes.size()-1;
		if(size <= 0)return;
		for(int i=size/2;i>=0;i--){
			int parent = i;
			int l = getLeft(parent);
			int r = getRight(parent);
			int idx = parent;

			if(l <= size && this->nodes[parent]->distance > this->nodes[l]->distance)
				idx = l;
			if(r <= size && idx >= 0 && this->nodes[idx]->distance > this->nodes[r]->distance)
				idx = r;
			if(idx >= 0)
				swap(nodes[parent],nodes[idx]);
		}
	}
	
	void sort(){
		for(int i=0;i<nodes.size()-1;i++){
			for(int j=0;j<nodes.size()-i-1;j++){
				if(nodes[j]->distance > nodes[j+1]->distance){
					swap(nodes[j],nodes[j+1]);
				}
			}
		}
	}
};

class Map{
private:
	vector < Node* > gens;
	void drawWall(Node *curr, bool save = false){
		curr->cost = 1;
		curr->symbol = ' ';
		
		if(save)gens.push_back(curr);
	}
	
	void isDraw(Node *curr1, Node *curr2){
		if(curr1->symbol == ' ' || curr2->symbol == ' ')return;
		
		drawWall(curr1);
		drawWall(curr2,true);
	}
	
	void checkMaze(Node *curr){
		if(curr->y - 1 > 0 && curr->y - 2 > 0) 
			isDraw(nodes[curr->y-1][curr->x],nodes[curr->y-2][curr->x]);
		
			
		if(curr->y + 1 < this->height && curr->y + 2 < this->height)
			isDraw(nodes[curr->y+1][curr->x],nodes[curr->y+2][curr->x]);
		
		
		if(curr->x - 1 > 0 && curr->x - 2 > 0)
			isDraw(nodes[curr->y][curr->x-1],nodes[curr->y][curr->x-2]);
		
		
		if(curr->x + 1 < this->width && curr->x + 2 < this->width)
			isDraw(nodes[curr->y][curr->x+1],nodes[curr->y][curr->x+2]);
	}
	
	void maze(Node* curr){
		curr->cost = 1;
		curr->symbol = ' ';
		
		gens.push_back(curr);
		int size = gens.size();
		
		while(size > 0){
			int r = random(0,size);
			checkMaze(gens[r]);
			gens.erase(gens.begin()+r);
			size = gens.size();
		}
	}

public:
	int width,height;
	vector < vector< Node* > > nodes;
	Map(int width,int height){
		this->height = height;
		this->width = width;
		for(int i=0;i<this->height;i++){
			vector <Node*> row;
			for(int j=0;j<this->width;j++){
				row.push_back(new Node(j,i));
			}
			nodes.push_back(row);
		}
	}
	
	void copyFrom(vector <vector<Node*> > source){
		for(int i=0;i<this->height;i++){
			for(int j=0;j<this->width;j++){
				this->nodes[i][j]->copy(source[i][j]);
			}
		}
	}
	
	void render(){
		for(int i=0;i<SPACE_VERTICAL;i++)puts("");
			 
		for(int i=0;i<this->height;i++){
			for(int j=0;j<this->width;j++){
				if(i == 0 || i == this->height - 1 || j == 0 || j == this->width - 1 || (nodes[i][j]->cost == WALL_COST && probability(20))){
					this->setSymbol(i,j,WALL);
					this->setCost(i,j,STRONG_WALL_COST);
					this->draw(i,j,COLOR_STRONG_WALL);
				}
				else this->draw(i,j);
			}
		}
		gotopos(0,0);
	}
	
	void reset(){
		for(int i=0;i<this->height;i++){
			for(int j=0;j<this->width;j++){
				this->nodes[i][j]->distance = 0;
				this->nodes[i][j]->parent = 0;
				this->nodes[i][j]->isVisited = false;
			}
		}
	}
	
	void generateWall(){
		int r1 = random(1,this->height-1);
		int r2 = random(1,this->width-1);
		r1 = (r1 % 2 == 0) ? --r1 : r1;
		r2 = (r2 % 2 == 0) ? --r2 : r2;
		maze(nodes[r1][r2]); 
	}
	
	void setSymbol(int y, int x, char symbol){
		this->nodes[y][x]->symbol = symbol;
	}
	
	void setCost(int y, int x, int cost){
		this->nodes[y][x]->cost = cost;
	}
	
	void draw(int y, int x,int color = COLOR_WALL){
		gotopos(SPACE_VERTICAL+y,x);
		colorText(color);
		putchar(this->nodes[y][x]->symbol);
		gotopos(0,0);
	}
	
};

class Console{
public:
	int width;
	int height;
	int WALL_WIDTH;
	int WALL_HEIGHT;
	char logo[15][100] = 	{" ____    ___   ___ ___  ____     ___  ____  ",
							"|    \\  /   \\ |   |   ||    \\   /  _]|    \\ ",
							"|  o  )|     || _   _ ||  o  ) /  [_ |  D  )",
							"|     ||  O  ||  \\_/  ||     ||    _]|    / ",
							"|  O  ||     ||   |   ||  O  ||   [_ |    \\ ",
							"|     ||     ||   |   ||     ||     ||  .  \\",
							"|_____| \\___/ |___|___||_____||_____||__|\\_|",
							"                                            ",
							"         __  _  __ __  __ __  __ __         ",
							"        |  |/ ]|  |  ||  |  ||  |  |        ",
							"        |  ' / |  |  ||  |  ||  |  |        ",
							"        |    \\ |  _  ||  |  ||  ~  |        ",
							"        |     ||  |  ||  :  ||___, |        ",
							"        |  .  ||  |  ||     ||     |        ",
							"        |__|\\_||__|__| \\__,_||____/         "};
	Map *map;
	
	Console(){
		config();
	}
	
	void printLogo(); 
	
	void startGame(){
		map = new Map(this->WALL_WIDTH,this->WALL_HEIGHT);
		map->generateWall();
		map->render();
	}
	
	void config(){
		
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	
	
	    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	    this->width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	    this->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	    this->WALL_WIDTH = this->width;
	    this->WALL_HEIGHT = this->height - SPACE_VERTICAL;
	    
	    if(this->WALL_WIDTH % 2 == 0)this->WALL_WIDTH--;
	    if(this->WALL_HEIGHT % 2 == 0)SPACE_VERTICAL--;
	}
};
Console *console;

class Bomb{
private:
	bool isCanExplode(int y, int x){
		if(x <= 0 || y <= 0)return false;
		if(x >= map->width - 1 || y >= map->height-1)return false; 
		
		return true;
	}
public:
	int id;
	int x;
	int y;
	Map *map;
	bool isBombExplode = true;
	bool isToggle = true,isAnim=false,isAnim_1=false;
	int time_bomb;
	int count_step;
	int delay_time;
	vector <Node *> areas;
	
	Bomb(int y,int x, Map *map, int id){
		this->id = id;
		this->count_step = this->time_bomb = 0;
		this->map = map;
		this->x = x;
		this->y = y; 
		this->delay_time = 30;
		this->map->setCost(this->y,this->x,BOMB_COST);
		this->map->setSymbol(y,x,BOMB);
		this->map->draw(y,x,COLOR_EXPLOSION);
		this->isBombExplode = true;
		this->time_bomb = this->count_step = 0;
		this->explode();
	} 
	
	void checkAdj(int y, int x,int dy,int dx){
		while(this->isCanExplode(y,x)){
			bool isEnd = false;
			
			if(map->nodes[y][x]->cost == WALL_COST)isEnd = true;
			else if(map->nodes[y][x]->cost == STRONG_WALL_COST)break;
			
			areas.push_back(map->nodes[y][x]);
			y+=dy;
			x+=dx;
			
			if(isEnd)break;
		}
	}
	
	void explode(){
		areas.push_back(map->nodes[this->y][this->x]);
		for(int i=0;i<4;i++){
			int dX = dirs[i][0];
			int dY = dirs[i][1];
			
			checkAdj(this->y+dY,this->x+dX,dY,dX);
		}
	}
	void showArea(bool is_show, Collection *c);
	bool checkBomb(Collection *c);
	
};

class Player{
private:
	bool isSpace(int dY, int dX){
		if(dX <= 0 || dY <= 0)return false;
		if(dX >= this->map->width - 1 || dY >= this->map->height-1)return false;
		if(this->map->nodes[dY][dX]->symbol != ' ')return false;
		
		return true;
	}
	
	bool isEdge(int dY, int dX){
		if(dX <= 0 || dY <= 0)return false;
		if(dX >= this->map->width - 1 || dY >= this->map->height-1)return false;
		
		if(this->map->nodes[dY][dX]->isVisited)return false;
		if(this->map->nodes[dY][dX]->cost == WALL_COST || this->map->nodes[dY][dX]->cost == STRONG_WALL_COST)return false;
		return true;
	}
	
public:
	char *name;
	int id;
	int x;
	int y;		
	int hp;
	int bomb;
	int score;
	int dir;
	Map *map;
	vector <Bomb*> bombs;
	VectorMinHeap *vecD;
	
	Player(char *name,int y,int x,Map *main,int id){
		this->id = id;
		vecD = new VectorMinHeap;
		this->map = main;
		this->name = name;
		this->y = y;
		this->x = x;
		this->bomb = 20;
		this->score = 0;
		this->dir = -1;
		this->draw(9999,PLAYER,COLOR_BLUE);
	}
	
	void draw(int cost, char symbol,int color=COLOR_BLUE){
		this->map->setCost(this->y,this->x,cost);
		this->map->setSymbol(this->y,this->x,symbol);
		this->map->draw(this->y,this->x,color);
	} 
	
	void move(int key){
		int curr = -1;
		if(key == KEY_UP)
			curr = 0;
		else if(key == KEY_RIGHT)
			curr = 1;
		else if(key == KEY_BOTTOM)
			curr = 2;
		else if(key == KEY_LEFT)
			curr = 3;
		else return;
		
		if(curr < 0 || curr > 3)return;
		int dX = x+dirs[curr][0];
		int dY = y+dirs[curr][1];
		
		if(!isSpace(dY,dX))return;
		this->draw(1,' ');
		
		this->x = dX;
		this->y = dY;
		this->draw(9999,1);
		this->dir = curr;
	}
	
	void throwBomb(){
		if(dir < 0 || dir > 3)return;
		bombs.push_back(new Bomb(this->y,this->x,map,this->id));
	}
	
	void checkEdgePath(Node *curr,Collection *c);
	
	void doPath(Collection *c);
	
	void checkingBomb(Collection *c);
	
};

class Enemy{
private:
	
	bool isFoundPlayer(int dY, int dX){
		if(dX <= 0 || dY <= 0)return false;
		if(dX >= map->width - 1 || dY >= map->height-1)return false;
		if(map->nodes[dY][dX]->cost == WALL_COST || map->nodes[dY][dX]->cost == WALL_COST || map->nodes[dY][dX]->cost == ENEMY_COST)return false;
		
		return true;
	}
	
	bool isSpace(int dY, int dX){
		if(dX <= 0 || dY <= 0)return false;
		if(dX >= map->width - 1 || dY >= map->height-1)return false;
		if(map->nodes[dY][dX]->symbol != ' ')return false;
		
		return true;
	}
public:
	int id;
	int x;
	int y;		
	int hp;
	int dir;
	int time_enemy;
	bool isFound = false,isPlant = false,isDead=false;
	Map *map;
	vector <Bomb*> bombs;
	
	Enemy(int y, int x, Map *mainmap, int id){
		this->id = id;
		this->time_enemy = 1;
		this->map = mainmap;
		this->x = x;
		this->y = y;
		this->hp = 100;
		this->dir = -1;
		
		this->map->setSymbol(this->y,this->x,ENEMY);
		this->map->setCost(this->y,this->x,1);
		this->draw();
	}
	
	void checkTime(){
		++this->time_enemy;
		if(this->time_enemy % delay_enemy_bomb == 0){
			this->isPlant = false;
			this->time_enemy = 1;
		}
	}
	
	void draw(){
		this->map->draw(this->y,this->x,COLOR_RED);
	}
	
	void moveTo(int y,int x){
		this->map->setSymbol(this->y,this->x,' ');
		this->map->setCost(this->y,this->x,1);
		this->draw();
		
		this->x= x;
		this->y= y;
		
		this->map->setSymbol(this->y,this->x,ENEMY);
		this->map->setCost(this->y,this->x,ENEMY_COST);
		this->draw();
	}
	
	void randomMove(){
		vector<int> tos;
		int dy,dx;
		for(int i=0;i<4;i++){
			dx = this->x + dirs[i][0];
			dy = this->y + dirs[i][1];
			if(isSpace(dy,dx)){
				int a = i;
				tos.push_back(a);
			}
		}
		if(tos.size() < 1)return;
		
		int dir = tos[rand()%tos.size()];
		dx = this->x + dirs[dir][0];
		dy = this->y + dirs[dir][1];
		
		moveTo(dy,dx);
	}
	
	bool isThrowBomb(Node *curr, Player *p);
	void throwBomb(int dy,int dx);
	void checkingBomb(Collection *c);
};

class Collection{
public:
	Player *player;
	vector<Enemy*> enemies;
	Console *console;
	
	bool notInPlayer(int y, int x){
		if((x >= player->x && x <= player->x + 10) && (y >= player->y && y <= player->y + 10))return false;
		
		return true;
	}
	
	Collection(Player *p,Console *c){
		this->console = c;
		this->player = p;
		
		for(int i=0;i<this->console->WALL_HEIGHT;i++){
			for(int j=0;j<this->console->WALL_WIDTH;j++){
				if(this->console->map->nodes[i][j]->symbol == ' ' && probability(1) && notInPlayer(i,j)){
					this->enemies.push_back(new Enemy(i,j,this->console->map,enemy_count));
					--total_enemy;
				}
				if(total_enemy <= 0)return;
			}
		}
	}
	
	void draw(){
		this->player->draw(9999,PLAYER);
		for(int i=0;i<this->enemies.size();i++){
			if(this->enemies[i]->isDead)continue;
			this->enemies[i]->draw();
		}
	}
	
	void allBombs(){
		this->player->checkingBomb(collect);
		for(int i=0;i<enemies.size();i++){
			this->enemies[i]->checkingBomb(collect);
		}
	}
	
	void checkTime(){
		for(int i=0;i<enemies.size();i++){
			if(this->enemies[i]->isDead)continue;
			this->enemies[i]->checkTime();
		}
	}
	
	int getTotalEnemies(){
		int i = 0;
		int ret = 0;
		while(i < this->enemies.size()){
			if(!enemies[i]->isDead)++ret;
			i--;
		}
		return ret;
	}
	
};

void Enemy::throwBomb(int dy, int dx){
	if(!isSpace(dy,dx))return;
	bombs.push_back(new Bomb(dy,dx,this->map,this->id));
	isPlant = true;
}

bool Enemy::isThrowBomb(Node *curr, Player *p){
	
	if(this->isPlant)return false;
	for(int i=0;i<4;i++){
		int dx = dirs[i][0];
		int dy = dirs[i][1];
		int cx = curr->x, cy = curr->y;
		while(true){
			cx+=dx;
			cy+=dy;
			if(console->map->nodes[cy][cx]->symbol == PLAYER)return true;
			if(console->map->nodes[cy][cx]->cost == BOMB_COST || console->map->nodes[cy][cx]->cost == STRONG_WALL_COST || console->map->nodes[cy][cx]->cost == WALL_COST || console->map->nodes[cy][cx]->symbol == ENEMY)break;
		}
	}
	return false;
}

void Bomb::showArea(bool is_show, Collection *c){
	char a = ' ';
	int costa = 1;
	if(is_show){
		a = BOMB_EXPLOSION;
		costa = BOMB_COST;	
	}
	else{
		a = ' ';	
		costa = 1;	
	}
	int size = areas.size();
	for(int i=0;i<size;i++){
		if(!is_show){
			for(int j=0;j<c->enemies.size();j++){
				if(c->enemies[j]->isDead)continue;
				if(areas[i]->x == c->enemies[j]->x && areas[i]->y == c->enemies[j]->y && c->enemies[j]->id != this->id){
					c->enemies[j]->isDead = true;
				}
				if(areas[i]->x == c->player->x && areas[i]->y == c->player->y && c->player->id != this->id){
					//kurang darah
				}
			}	
		}
		this->map->setSymbol(areas[i]->y,areas[i]->x,a);
		this->map->setCost(areas[i]->y,areas[i]->x,costa);
		this->map->draw(areas[i]->y,areas[i]->x,COLOR_EXPLOSION);	
	}
}

bool Bomb::checkBomb(Collection *c){
	if(this->isBombExplode){
		this->time_bomb+=1;
		if(time_bomb % 10 == 0){
			if(isToggle && !isAnim){
				this->map->setSymbol(this->y,this->x,' ');
				this->map->draw(this->y,this->x,COLOR_EXPLOSION);
				isToggle = !isToggle;
				++count_step;
				if(count_step == 3){
					this->isAnim = true;
					time_bomb = count_step = 0;
				}
			}
			else if(!isToggle && !isAnim){
				this->map->setSymbol(this->y,this->x,BOMB);
				this->map->draw(this->y,this->x,COLOR_EXPLOSION);						
				isToggle = !isToggle;
			}
			else if(isAnim && !isAnim_1){
				this->showArea(true,c);
				time_bomb = count_step = 0;
				isAnim_1  = true;
			}
			else{
				this->map->setCost(this->y,this->x,1);
				this->showArea(false,c);
				time_bomb = count_step = 0;
				isBombExplode = false;
				isAnim = isAnim_1 = false;
				isToggle = true;
				areas.clear();
				return true;
			}
		}
	}
	else this->time_bomb = this->count_step = 0;
	return false;
}

void Player::checkingBomb(Collection *c){
	for(int i=0;i<this->bombs.size();i++){
		if(this->bombs[i]->checkBomb(c)){
			delete this->bombs[i];
			this->bombs.erase(this->bombs.begin()+i);
		}
	}
	
}

void Enemy::checkingBomb(Collection *c){
	for(int i=0;i<this->bombs.size();i++){
		if(this->bombs[i]->checkBomb(c)){
			delete this->bombs[i];
			this->bombs.erase(this->bombs.begin()+i);
		}
	}
	
}

void Player::checkEdgePath(Node *curr,Collection *c){
	for(int i=0;i<4;i++){
		int dx = curr->x + dirs[i][0];
		int dy = curr->y + dirs[i][1];
		
		if(isEdge(dy,dx)){
			this->map->nodes[dy][dx]->isVisited = true;
			this->map->nodes[dy][dx]->distance = (this->map->nodes[dy][dx]->cost + curr->distance);
			this->map->nodes[dy][dx]->parent = curr;
			this->vecD->nodes.push_back(this->map->nodes[dy][dx]);
		}
	}
}

void Player::doPath(Collection *c){
	this->map->reset();
	vecD->nodes.clear();
	
	vecD->nodes.push_back(this->map->nodes[this->y][this->x]);
	
	while(vecD->nodes.size() > 0){
		int idx = 0;
		for(int i=0;i<vecD->nodes.size();i++)
			if(vecD->nodes[i]->distance < vecD->nodes[idx]->distance) idx = i;
		
		checkEdgePath(this->vecD->nodes[idx],c);
		vecD->nodes.erase(vecD->nodes.begin()+idx);
	}
	
	for(int i=0;i<c->enemies.size();i++){
		if(c->enemies[i]->isDead)continue;
		Node *curr = this->map->nodes[c->enemies[i]->y][c->enemies[i]->x];
		
		//temp for calculate range to player
		Node *temp = &(*curr);
		int size = 0;
		
		if(!curr->parent)continue;
		curr = curr->parent;
		temp = temp->parent;
		
		//calculate range
		while(temp){
			if(temp->x == this->x && temp->y == this->y)break;
			temp = temp->parent;
			++size;
		}
		
		if(size > 10){
			c->enemies[i]->randomMove();
			continue;	
		}
		if(curr->x == this->x && curr->y == this->y)continue;
		
		bool isDir = c->enemies[i]->isThrowBomb(this->map->nodes[c->enemies[i]->y][c->enemies[i]->x], c->player);
		if(!isDir){
			c->enemies[i]->moveTo(curr->y,curr->x);
			continue;	
		}
		else c->enemies[i]->throwBomb(curr->y,curr->x);
		
	}
	
	
}

void mainRender(char name[]){
	int count_bomb = 0;
	collect = new Collection(new Player(name,1,1,console->map,1),console);
	
	while(true){ 
		Sleep(20);
		collect->draw();
		if(kbhit()){
			gotopos(0,0);
			int c = getch();
			if(c == 27){
				return;
			}
			else if((char)c == 'b'){
				gotopos(0,0);
				int y,x;
				printf("input y : ");
				scanf("%d",&y);fflush(stdin);
				printf("input x : ");
				scanf("%d",&x);fflush(stdin);
			}
			else if(c == KEY_SPACE)
				collect->player->throwBomb();
			else
				collect->player->move(c);	
			
			c = 0;
		}			
		collect->allBombs();
		collect->checkTime();
		
		if(++time_global % 20 == 0) {
			collect->player->doPath(collect);
		}
		
	}
}

void centerAlign(char text[], int y){
	colorText(COLOR_BLUE);
	int size = strlen(text);
	int sx = console->width / 2 - (size/2);
	gotopos(y,sx);
	printf("%s",text);
}

void moveMenu(char menus[3][100], int choose, char c, int y){
	int size ,sx;
	size = strlen(menus[choose]);
	sx = console->width / 2 - (size/2);
	gotopos(y + (choose*3),sx-2);
	putchar(c);
	gotopos(0,0);
}

int curr_y = 0;
int main(){
	HWND wh = GetConsoleWindow();
    MoveWindow(wh, 0, 0, 850, 700, TRUE);
	system("COLOR F0");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_WALL);
	srand(time(NULL)); 
	console = new Console();
	char name[100];
	int choose = 0;
	char menus[3][100] = {"Let's Start","High Scores","Exit"};
	
	do{
		system("cls");
		console->printLogo();
		curr_y += 6;
		for(int i=0;i<3;i++){
			centerAlign(menus[i],curr_y + (i*3));
		}
		
		moveMenu(menus,choose,'>',curr_y);
		while(true){
			if(kbhit()){
				int c = getch();
				if(c == 13){
					if(choose == 0){
						bool isV = true;
						do{
							isV = true;
							system("cls");
							centerAlign("Type your name [3..20] : ",console->height/2+1);
							scanf("%[^\n]",name);fflush(stdin);
							if(strlen(name) < 3 || strlen(name) > 20){
								isV = false;
								system("cls");
								centerAlign("Name must be 3-20 characters",console->height/2+1);
								centerAlign("Enter to continue ..",console->height/2+3);getchar();
							}
						}while(!isV);
						
						system("cls");
						console->startGame();
						mainRender(name);
						break;
					}
					else if(choose == 1){
						system("cls");
						break;
					}
					else if(choose == 2){
						system("cls");
						break;
					}
				}
				if(c != KEY_UP && c != KEY_BOTTOM)continue;
							
				moveMenu(menus,choose,' ',curr_y);
				
				if(c == KEY_UP)choose--;
				else if(c==KEY_BOTTOM)choose++;
				
				if(choose < 0)choose = 0;
				else if(choose > 2)choose = 2;
				
				c = -1;
				
				moveMenu(menus,choose,'>',curr_y);
				
			}		
		}
		
	}while(choose != 2);
	
	
	getchar();
	
	return 0;
}

void Console::printLogo(){
		for(int i=0;i<15;i++){
			centerAlign(logo[i],i+5);
			curr_y = i+5;
		}
	}

