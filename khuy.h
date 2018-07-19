#define MAX_RANGE_BOMB 5 
#define WALL_COST 88888
#define STRONG_WALL_COST 99999

#define COLOR_WALL 240 
#define COLOR_STRONG_WALL 243
#define COLOR_EXPLOSION 252
#define COLOR_BLUE 249
#define COLOR_RED 244

#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_BOTTOM 80
#define KEY_SPACE 32

#define WALL 178
#define BOMB 235
#define BOMB_EXPLOSION 176
#define PLAYER 1
#define ENEMY 2
#define ENEMY_COST 999999
#define BOMB_COST 999999

int SPACE_VERTICAL = 11;
int dirs[8][2]={
				{0,-1},
				{1,0},
				{0,1},
				{-1,0},
				{-1,-1},
				{1,1},
				{1,-1},
				{-1,1}
				};
						
//Flag
//bool isBombExplode = false;
int time_bomb = 1;
int time_global = 0;

void colorText(int c){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void gotopos(int y, int x){
	COORD coord = {x, y};
	SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}
int random(int min, int max){
	return rand()%(max - min) + min;
}
bool probability(int percentage){
	bool probs[100];
	for(int i=0;i<100;i++)probs[i] = false;
	
	while(percentage > 0){
		int a = random(0,100);
		if(!probs[a]){
			probs[a] = true;
			--percentage;
		}
	}
	
	return probs[random(0,100)];
	
}
