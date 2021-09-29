#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include<conio.h>
#include<stdlib.h>

#define BLOCK_CREATE_POS_X 5
#define BLOCK_CREATE_POS_Y 1

typedef struct Block{
	int x;
	int y;
	int code;
}block;

void settingGame();										//게임 실행시 초기 세팅 
void printBoard(int blockCode, int x, int y);			//blockCode값에 따라 지정된 문자를 출력하는 함수 
int updateGame();										//지속적으로 호출되는 함수 
void goto_xy(int x, int y);								//커서위치를 옮기는 함수  
void createBlock();										//블록을 생성하는 함수 
void settingTetromino();								//미노 모양을 설정 
void fallingBlock();									//일정 시간마다 블록이 낙하하는 것을 제어하는 함수  
void softDrop();										//소프트 드롭을 실행하는 함수 

int board[24][12] = {0};								//게임 보드판 변수 
int blockExistence = 0;									//현재 플레이어가 조종하는 블록이 존재하는지 판단하는 변수 
int tetromino[8][4][4][4] = {0};						//테트리스의 미노 모양을 저장하는 변수 
block blockQueue[4] = {0};								//현재 플레이어가 조작하고 있는 블록의 정보를 담은 변수 
time_t gameStartTime = 0;
time_t criteriaTime = 0;
time_t nowTime = 0;
time_t delayTime = 1000;

int main(void)
{
	int playing = 1;
	settingGame();
	while(playing)
	{
		playing = updateGame();
	}
	scanf("%d",&playing);
	
	return 0;
}

int updateGame()
{
	createBlock();
	fallingBlock();
	
	return 1;
}

void settingGame()
{
	for(int row=2; row<=23; row++)
	{
		board[row][0] = -1;
		board[row][11] = -1;
	}
	for(int column=0; column<=11; column++)
	{
		//board[0][column] = -1;
		board[23][column] = -1;
	}
	
	for(int row=0; row<=23; row++)
	{
		for(int column=0; column<=11; column++)
		{
			printBoard(board[row][column], column, row);
		}
		printf("\n");
	}
	settingTetromino();
	gameStartTime = clock();
	criteriaTime = clock();
	
	
	
	return;
}

void printBoard(int blockCode, int x, int y)
{
	x*=2;
	goto_xy(x, y); 
	
	if(blockCode == -1)
	{
		printf("□");
	}
	else if(blockCode == 0)
	{
		printf("  ");
	}
	else
	{
		printf("■");
	}
	
	return;
}


void createBlock()
{
	int blockCode = 0;
	int remainBlock = 3;
	int X = 0;
	int Y = 0;
	
	if(blockExistence == 1)
	{
		return;
	}
	srand((unsigned int)time(NULL));
	blockCode = ((int)rand() % 6) + 1;
	printf("\n blockCode : %d\n", blockCode);
	for(int y = 0; y <= 3; y++)
	{
		for(int x = 0; x <= 3; x++)
		{
			if(tetromino[blockCode][0][y][x] != 0)
			{
				X = BLOCK_CREATE_POS_X - 1 + x;
				Y = BLOCK_CREATE_POS_Y - 1 + y;
				board[Y][X] = blockCode;
				printBoard(blockCode, X, Y);
				blockQueue[remainBlock].x = X;
				blockQueue[remainBlock].y = Y;
				blockQueue[remainBlock].code = blockCode;
				remainBlock--;
			}
		}
	}
	blockExistence = 1;
	
	return;
}

void fallingBlock()
{
	nowTime = clock();
	if(nowTime - criteriaTime >= delayTime && delayTime > 0)
	{
		criteriaTime = clock();
		delayTime -= 5;
		softDrop();
	} 
}

void softDrop()
{
	int X = 0;
	int Y = 0;
	int CODE = 0;
	
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		CODE = blockQueue[i].code;
		
		board[Y+1][X] = CODE;
		printBoard(CODE, X, Y+1);
		printBoard(0, X, Y);
		blockQueue[i].y = Y+1;
	}
} 

void goto_xy(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle, pos);
}


void settingTetromino()
{
	//Z미노  
	tetromino[1][0][0][0] = 1;
	tetromino[1][0][0][1] = 1;
	tetromino[1][0][1][1] = 1;
	tetromino[1][0][1][2] = 1;

	tetromino[1][1][0][2] = 1;
	tetromino[1][1][1][1] = 1;
	tetromino[1][1][1][2] = 1;
	tetromino[1][1][2][1] = 1;
	
	tetromino[1][2][1][0] = 1;
	tetromino[1][2][1][1] = 1;
	tetromino[1][2][2][1] = 1;
	tetromino[1][2][2][2] = 1;
	
	tetromino[1][3][0][1] = 1;
	tetromino[1][3][1][0] = 1;
	tetromino[1][3][1][1] = 1;
	tetromino[1][3][2][0] = 1;
	
	//L미노  
	tetromino[2][0][0][2] = 1;
	tetromino[2][0][1][0] = 1;
	tetromino[2][0][1][1] = 1;
	tetromino[2][0][1][2] = 1;
	
	tetromino[2][1][0][1] = 1;
	tetromino[2][1][1][1] = 1;
	tetromino[2][1][2][1] = 1;
	tetromino[2][1][2][2] = 1;

	tetromino[2][2][1][0] = 1;
	tetromino[2][2][1][1] = 1;
	tetromino[2][2][1][2] = 1;
	tetromino[2][2][2][0] = 1;
	
	tetromino[2][3][0][0] = 1;
	tetromino[2][3][0][1] = 1;
	tetromino[2][3][1][1] = 1;
	tetromino[2][3][2][1] = 1;
	
	//O미노  
	tetromino[3][0][0][1] = 1;
	tetromino[3][0][0][2] = 1;
	tetromino[3][0][1][1] = 1;
	tetromino[3][0][1][2] = 1;
	
	tetromino[3][1][0][1] = 1;
	tetromino[3][1][0][2] = 1;
	tetromino[3][1][1][1] = 1;
	tetromino[3][1][1][2] = 1;
	
	tetromino[3][2][0][1] = 1;
	tetromino[3][2][0][2] = 1;
	tetromino[3][2][1][1] = 1;
	tetromino[3][2][1][2] = 1;
	
	tetromino[3][3][0][1] = 1;
	tetromino[3][3][0][2] = 1;
	tetromino[3][3][1][1] = 1;
	tetromino[3][3][1][2] = 1;

	//S미노  
	tetromino[4][0][0][1] = 1;
	tetromino[4][0][0][2] = 1;
	tetromino[4][0][1][0] = 1;
	tetromino[4][0][1][1] = 1;
	
	tetromino[4][1][0][0] = 1;
	tetromino[4][1][1][0] = 1;
	tetromino[4][1][1][1] = 1;
	tetromino[4][1][2][1] = 1;
	
	tetromino[4][2][1][1] = 1;
	tetromino[4][2][1][2] = 1;
	tetromino[4][2][2][0] = 1;
	tetromino[4][2][2][1] = 1;
	
	tetromino[4][3][0][0] = 1;
	tetromino[4][3][1][0] = 1;
	tetromino[4][3][1][1] = 1;
	tetromino[4][3][2][1] = 1;
	
	//J미노  
	tetromino[5][0][0][0] = 1;
	tetromino[5][0][1][0] = 1;
	tetromino[5][0][1][1] = 1;
	tetromino[5][0][1][2] = 1;
	
	tetromino[5][1][0][1] = 1;
	tetromino[5][1][0][2] = 1;
	tetromino[5][1][1][1] = 1;
	tetromino[5][1][2][1] = 1;
	
	tetromino[5][2][1][0] = 1;
	tetromino[5][2][1][1] = 1;
	tetromino[5][2][1][2] = 1;
	tetromino[5][2][2][2] = 1;
	
	tetromino[5][3][0][1] = 1;
	tetromino[5][3][1][1] = 1;
	tetromino[5][3][2][0] = 1;
	tetromino[5][3][2][1] = 1;
	
	//T미노  
	tetromino[6][0][0][1] = 1;
	tetromino[6][0][1][0] = 1;
	tetromino[6][0][1][1] = 1;
	tetromino[6][0][1][2] = 1;
	
	tetromino[6][1][0][1] = 1;
	tetromino[6][1][1][1] = 1;
	tetromino[6][1][1][2] = 1;
	tetromino[6][1][2][1] = 1;
	
	tetromino[6][2][1][0] = 1;
	tetromino[6][2][1][1] = 1;
	tetromino[6][2][1][2] = 1;
	tetromino[6][2][2][1] = 1;
	
	tetromino[6][3][0][1] = 1;
	tetromino[6][3][1][0] = 1;
	tetromino[6][3][1][1] = 1;
	tetromino[6][3][2][1] = 1;
	
	//I미노  
	tetromino[7][0][1][0] = 1;
	tetromino[7][0][1][1] = 1;
	tetromino[7][0][1][2] = 1;
	tetromino[7][0][1][3] = 1;
	
	tetromino[7][1][0][2] = 1;
	tetromino[7][1][1][2] = 1;
	tetromino[7][1][2][2] = 1;
	tetromino[7][1][3][2] = 1;
	
	tetromino[7][2][2][0] = 1;
	tetromino[7][2][2][1] = 1;
	tetromino[7][2][2][2] = 1;
	tetromino[7][2][2][3] = 1;
	
	tetromino[7][3][0][1] = 1;
	tetromino[7][3][1][1] = 1;
	tetromino[7][3][2][1] = 1;
	tetromino[7][3][3][1] = 1;
	
	return;
}

