#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include<conio.h>
#include<stdlib.h>

#define BLOCK_CREATE_POS_X 5
#define BLOCK_CREATE_POS_Y 1
#define KEY_DOWN 0x8000
#define KEY_UP 0x0001
#define KEY_KEEP 0x8001

typedef struct Block{
	int x;
	int y;
	int code;
}block;

void settingGame();														//게임 실행시 초기 세팅 
void printBoard(int blockCode, int x, int y);							//blockCode값에 따라 지정된 문자를 출력하는 함수 
int updateGame();														//지속적으로 호출되는 함수 
void goto_xy(int x, int y);												//커서위치를 옮기는 함수  
void createBlock();														//블록을 생성하는 함수 
void settingTetromino();												//미노 모양을 설정 
void fallingBlock();													//일정 시간마다 블록이 낙하하는 것을 제어하는 함수  
void softDrop();														//소프트 드롭을 실행하는 함수 
int blockMoveSimulation(block blockQueue[4], int moveX, int moveY); 	//블록이 움직이는 것을 시뮬레이션해서 불가능한 경우 0, 가능한 경우 1을 리턴하고 preloadBlockQueue를 업데이트 해주는 함수  
void inputPreloadBlockQueue(int index, int code, int x, int y);			//preloadBlockQueue에 정보를 입력하는 함수
void reloadBlock();														//현재 조종중인 블록의 좌표를 최신화해주는 함수 
int blockRotationSimulation(block blockQueue[4], int rotDir);			//블록을 회전시키는 것을 시뮬레이션해주는 함수 
void inputKey();														//키 입력을 받는 함수 
void landBlock(block blockQueue[4], int decreseY);						//블록을 설치하는 함수
void checkLine(int line);												//y번째 줄이 가득 찼는지 확인하는 함수 

int board[24][12] = {0};												//게임 보드판 변수 
int blockExistence = 0;													//현재 플레이어가 조종하는 블록이 존재하는지 판단하는 변수 
block tetromino[8][4][4] = {0};											//테트리스의 미노 모양을 저장하는 변수 [블록코드][회전][블록] 
block blockQueue[4] = {0};												//현재 플레이어가 조작하고 있는 블록의 정보를 담은 변수 
block preloadBlockQueue[4] = {0};										//블록의 이동예정인 좌표의 정보를 담고 있는 변수 
time_t gameStartTime = 0;												//게이을 시작한 시각을 저장하는 변수 
time_t fallingCriteriaTime = 0;											//낙하 딜레의 기준이 되어주는 변수 
time_t moveCriteriaTime = 0;											//블록제어 딜레이의 기준이 되어주는 변수  
time_t rotCriteriaTime = 0;												//블록회전 딜레이의 기준이 되어주는 변수 
time_t nowTime = 0;														//현재시간 즉, 게임 시작 후 얼마나 지났는지를 저장하는 변수 
time_t fallingDelayTime = 150;											//블록낙하 딜레이 시간  
time_t moveDelayTime = 100;												//블록이동 딜레이 시간 
time_t rotDelayTime = 100;												//블록회전 딜레이 시간  
time_t fallingDelayDecreaseTime = 0;									//블록낙하 딜레이 감소량
int blockRot = 0;														//블록의 회전 상태를 저장하는 변수			
int playing = 1;

int main(void)
{
	
	settingGame();
	while(playing)
	{
		updateGame();
	}
	scanf("%d",&playing);
	
	return 0;
}

int updateGame()
{
	nowTime = clock();
	createBlock();
	fallingBlock();
	inputKey();

	
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
	fallingCriteriaTime = clock();
	moveCriteriaTime = clock();
	rotCriteriaTime = clock();
	
	srand((unsigned int)time(NULL));
	
	
	
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
	int X = 0;
	int Y = 0;
	
	if(blockExistence == 1)
	{
		return;
	}
	blockCode = ((int)rand() % 7) + 1;
	
	for(int i=0; i<=3; i++)
	{
		X = BLOCK_CREATE_POS_X + tetromino[blockCode][0][i].x;
		Y = BLOCK_CREATE_POS_Y + tetromino[blockCode][0][i].y;
		board[Y][X] = blockCode;
		blockQueue[i].x = X;
		blockQueue[i].y = Y;
		blockQueue[i].code = blockCode;
	}
	blockExistence = 1;
	blockRot = 0; 
	
	return;
}

void fallingBlock()
{
	if(blockExistence == 0)
	{
		return;
	}
	if(nowTime - fallingCriteriaTime >= fallingDelayTime && fallingDelayTime > 0)
	{
		fallingCriteriaTime = clock();
		moveDelayTime -= fallingDelayDecreaseTime;
		softDrop();
	} 
	
	return;
}

void softDrop()
{
	
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;
	int temp = blockMoveSimulation(blockQueue, 0, 1);
	if(temp == 0)
	{
		landBlock(blockQueue, 0);
		
		return;
	}
	else
	{
		reloadBlock();
	}
} 

int blockMoveSimulation(block blockQueue[4], int moveX, int moveY)
{
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;;
	
	if(moveX != 0)
	{
		for(int i=0; i<=3; i++)
		{
			X = blockQueue[i].x;
			Y = blockQueue[i].y;
			
			//goto_xy(0, 25+i);
			//printf("                                                                                          ");
			//goto_xy(0, 25+i);
			//printf("기존 x : %d, 기존 y : %d, moveX : %d, moveY : %d / 이동 x값 : %d, 이동 y값 : %d\n", X, Y, moveX, moveY, X + moveX, Y + moveY);
			
			if(board[Y][X + moveX] != 0 && board[Y][X + moveX] != CODE)
			{
				return 0;
			}
			inputPreloadBlockQueue(i, CODE, X + moveX, Y);
		}
		
	}
	else if(moveY != 0)
	{
		for(int i=0; i<=3; i++)
		{
			X = blockQueue[i].x;
			Y = blockQueue[i].y;
			
			//goto_xy(0, 25+i);
			//printf("                                                                                          ");
			//goto_xy(0, 25+i);
			//printf("기존 x : %d, 기존 y : %d, moveX : %d, moveY : %d / 이동 x값 : %d, 이동 y값 : %d\n", X, Y, moveX, moveY, X + moveX, Y + moveY);
			
			if(board[Y + moveY][X] != 0 && board[Y + moveY][X] != CODE)
			{
				return 0;
			}
			inputPreloadBlockQueue(i, CODE, X, Y + moveY);
		}
	}
	
	return 1;
}

void inputPreloadBlockQueue(int index, int code, int x, int y)
{
	preloadBlockQueue[index].code = code;
	preloadBlockQueue[index].x = x;
	preloadBlockQueue[index].y = y;
}

void reloadBlock()
{
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;
	
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		
		board[Y][X] = 0;
		printBoard(0, X, Y); 
	}
	
	CODE = preloadBlockQueue[0].code;
	for(int i=0; i<=3; i++)
	{
		X = preloadBlockQueue[i].x;
		Y = preloadBlockQueue[i].y;
		
		board[Y][X] = CODE;
		printBoard(CODE, X, Y); 
		blockQueue[i] = preloadBlockQueue[i];
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

void inputKey()
{
	if(blockExistence == 0)
	{
		return;
	}
	
	
	int temp = 0;
	int X = 0;
	int Y = 0;
	int CODE = 0;
	int keycode_z = 0X5a;
	int keycode_x = 0x58;
	if(GetAsyncKeyState(VK_LEFT))
	{
		if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} 
		moveCriteriaTime = clock();
		temp = blockMoveSimulation(blockQueue, -1, 0);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
	if(GetAsyncKeyState(VK_RIGHT))
	{
		if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} 
		moveCriteriaTime = clock();
		temp = blockMoveSimulation(blockQueue, 1, 0);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
	else if(GetAsyncKeyState(VK_DOWN))
	{
		if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} 
		moveCriteriaTime = clock();
		softDrop();
	}
	if(GetAsyncKeyState(keycode_z))
	{
		if(nowTime - rotCriteriaTime < rotDelayTime)
		{
			return;
		} 
		rotCriteriaTime = clock();
		temp = blockRotationSimulation(blockQueue, -1);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
	if(GetAsyncKeyState(keycode_x))
	{
		if(nowTime - rotCriteriaTime < rotDelayTime)
		{
			return;
		} 
		rotCriteriaTime = clock();
		temp = blockRotationSimulation(blockQueue, 1);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
}

int blockRotationSimulation(block blockQueue[4], int rotDir)
{
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;
	
	for(int i=0; i<=3; i++)
	{
		X = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].x - tetromino[CODE][blockRot][i].x;
		Y = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].y - tetromino[CODE][blockRot][i].y;
		
		if(board[blockQueue[i].y + Y][blockQueue[i].x + X] != 0 && board[blockQueue[i].y + Y][blockQueue[i].x + X] != CODE)
		{
			return 0;
		}
		inputPreloadBlockQueue(i, CODE, blockQueue[i].x + X, blockQueue[i].y + Y);
	}
	blockRot = (blockRot + 4 + rotDir) % 4;
	
	return 1;
} 

void landBlock(block blockQueue[4], int decreaseY)
{
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;
	int line[24] = {0};
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		inputPreloadBlockQueue(i,CODE + 7, X, Y - decreaseY);
		line[Y - decreaseY] = 1;
	}
	reloadBlock();
	blockExistence = 0;
	for(int i=0; i<24; i++)
	{
		if(line[i] != 0)
		{
			checkLine(i);
		}
	}
		
	return;
}

void checkLine(int line)
{
	if(line <=1)
	{
		playing = 0;
		return;
	}
	for(int x=1; x<=10; x++)
	{
		if(board[line][x] == 0)
		{
			return;
		}
	}
	for(int x=1; x<=10; x++)
	{
		board[line][x] = 0;
		printBoard(0, x, line);
	}
	for(int y = line - 1; y>=0; y--)
	{
		for(int x = 1; x<=10; x++)
		{
			board[y+1][x] = board[y][x];
			printBoard(board[y+1][x], x, y+1);
			board[y][x] = 0;
			printBoard(board[y][x], x, y);
		}
	}
	
	return;
}

void settingTetromino()
{
	//Z미노
	tetromino[1][0][0].x = -1;
	tetromino[1][0][0].y = -1;
	tetromino[1][0][0].code = 1;
	tetromino[1][0][1].x = 0;
	tetromino[1][0][1].y = -1;
	tetromino[1][0][1].code = 1;
	tetromino[1][0][2].x = 0;
	tetromino[1][0][2].y = 0;
	tetromino[1][0][2].code = 1;
	tetromino[1][0][3].x = 1;
	tetromino[1][0][3].y = 0;
	tetromino[1][0][3].code = 1;
	
	tetromino[1][1][0].x = 1;
	tetromino[1][1][0].y = -1;
	tetromino[1][1][0].code = 1;
	tetromino[1][1][1].x = 1;
	tetromino[1][1][1].y = 0;
	tetromino[1][1][1].code = 1;
	tetromino[1][1][2].x = 0;
	tetromino[1][1][2].y = 0;
	tetromino[1][1][2].code = 1;
	tetromino[1][1][3].x = 0;
	tetromino[1][1][3].y = 1;
	tetromino[1][1][3].code = 1;
	
	tetromino[1][2][0].x = 1;
	tetromino[1][2][0].y = 1;
	tetromino[1][2][0].code = 1;
	tetromino[1][2][1].x = 0;
	tetromino[1][2][1].y = 1;
	tetromino[1][2][1].code = 1;
	tetromino[1][2][2].x = 0;
	tetromino[1][2][2].y = 0;
	tetromino[1][2][2].code = 1;
	tetromino[1][2][3].x = -1;
	tetromino[1][2][3].y = 0;
	tetromino[1][2][3].code = 1;
	
	tetromino[1][3][0].x = -1;
	tetromino[1][3][0].y = 1;
	tetromino[1][3][0].code = 1;
	tetromino[1][3][1].x = -1;
	tetromino[1][3][1].y = 0;
	tetromino[1][3][1].code = 1;
	tetromino[1][3][2].x = 0;
	tetromino[1][3][2].y = 0;
	tetromino[1][3][2].code = 1;
	tetromino[1][3][3].x = 0;
	tetromino[1][3][3].y = -1;
	tetromino[1][3][3].code = 1;
	
	//L미노
	
	tetromino[2][0][0].x = -1;
	tetromino[2][0][0].y = 0;
	tetromino[2][0][0].code = 2; 
	tetromino[2][0][1].x = 0;
	tetromino[2][0][1].y = 0;
	tetromino[2][0][1].code = 2; 
	tetromino[2][0][2].x = 1;
	tetromino[2][0][2].y = 0;
	tetromino[2][0][2].code = 2; 
	tetromino[2][0][3].x = 1;
	tetromino[2][0][3].y = -1;
	tetromino[2][0][3].code = 2; 
	
	tetromino[2][1][0].x = 0;
	tetromino[2][1][0].y = -1;
	tetromino[2][1][0].code = 2;
	tetromino[2][1][1].x = 0;
	tetromino[2][1][1].y = 0;
	tetromino[2][1][1].code = 2;
	tetromino[2][1][2].x = 0;
	tetromino[2][1][2].y = 1;
	tetromino[2][1][2].code = 2;
	tetromino[2][1][3].x = 1;
	tetromino[2][1][3].y = 1;
	tetromino[2][1][3].code = 2; 
	
	tetromino[2][2][0].x = 1;
	tetromino[2][2][0].y = 0;
	tetromino[2][2][0].code = 2;
	tetromino[2][2][1].x = 0;
	tetromino[2][2][1].y = 0;
	tetromino[2][2][1].code = 2;
	tetromino[2][2][2].x = -1;
	tetromino[2][2][2].y = 0;
	tetromino[2][2][2].code = 2;
	tetromino[2][2][3].x = -1;
	tetromino[2][2][3].y = 1;
	tetromino[2][2][3].code = 2;

	tetromino[2][3][0].x = 0;
	tetromino[2][3][0].y = 1;
	tetromino[2][3][0].code = 2;
	tetromino[2][3][1].x = 0;
	tetromino[2][3][1].y = 0;
	tetromino[2][3][1].code = 2;
	tetromino[2][3][2].x = 0;
	tetromino[2][3][2].y = -1;
	tetromino[2][3][2].code = 2;
	tetromino[2][3][3].x = -1;
	tetromino[2][3][3].y = -1;
	tetromino[2][3][3].code = 2;

	//O미노  
	
	tetromino[3][0][0].x = 0;
	tetromino[3][0][0].y = -1;
	tetromino[3][0][0].code = 3;
	tetromino[3][0][1].x = 0;
	tetromino[3][0][1].y = 0;
	tetromino[3][0][1].code = 3;
	tetromino[3][0][2].x = 1;
	tetromino[3][0][2].y = 0;
	tetromino[3][0][2].code = 3;
	tetromino[3][0][3].x = 1;
	tetromino[3][0][3].y = -1;
	tetromino[3][0][3].code = 3;
	
	tetromino[3][1][0].x = 0;
	tetromino[3][1][0].y = -1;
	tetromino[3][1][0].code = 3;
	tetromino[3][1][1].x = 0;
	tetromino[3][1][1].y = 0;
	tetromino[3][1][1].code = 3;
	tetromino[3][1][2].x = 1;
	tetromino[3][1][2].y = 0;
	tetromino[3][1][2].code = 3;
	tetromino[3][1][3].x = 1;
	tetromino[3][1][3].y = -1;
	tetromino[3][1][3].code = 3;
	
	tetromino[3][2][0].x = 0;
	tetromino[3][2][0].y = -1;
	tetromino[3][2][0].code = 3;
	tetromino[3][2][1].x = 0;
	tetromino[3][2][1].y = 0;
	tetromino[3][2][1].code = 3;
	tetromino[3][2][2].x = 1;
	tetromino[3][2][2].y = 0;
	tetromino[3][2][2].code = 3;
	tetromino[3][2][3].x = 1;
	tetromino[3][2][3].y = -1;
	tetromino[3][2][3].code = 3;
	
	tetromino[3][3][0].x = 0;
	tetromino[3][3][0].y = -1;
	tetromino[3][3][0].code = 3;
	tetromino[3][3][1].x = 0;
	tetromino[3][3][1].y = 0;
	tetromino[3][3][1].code = 3;
	tetromino[3][3][2].x = 1;
	tetromino[3][3][2].y = 0;
	tetromino[3][3][2].code = 3;
	tetromino[3][3][3].x = 1;
	tetromino[3][3][3].y = -1;
	tetromino[3][3][3].code = 3;

	//S미노
	
	tetromino[4][0][0].x = 1;
	tetromino[4][0][0].y = -1;
	tetromino[4][0][0].code = 4;
	tetromino[4][0][1].x = 0;
	tetromino[4][0][1].y = -1;
	tetromino[4][0][1].code = 4;
	tetromino[4][0][2].x = 0;
	tetromino[4][0][2].y = 0;
	tetromino[4][0][2].code = 4;
	tetromino[4][0][3].x = -1;
	tetromino[4][0][3].y = 0;
	tetromino[4][0][3].code = 4;
	
	tetromino[4][1][0].x = 1;
	tetromino[4][1][0].y = 1;
	tetromino[4][1][0].code = 4;
	tetromino[4][1][1].x = 1;
	tetromino[4][1][1].y = 0;
	tetromino[4][1][1].code = 4;
	tetromino[4][1][2].x = 0;
	tetromino[4][1][2].y = 0;
	tetromino[4][1][2].code = 4;
	tetromino[4][1][3].x = 0;
	tetromino[4][1][3].y = -1;
	tetromino[4][1][3].code = 4;
	
	tetromino[4][2][0].x = -1;
	tetromino[4][2][0].y = 1;
	tetromino[4][2][0].code = 4;
	tetromino[4][2][1].x = 0;
	tetromino[4][2][1].y = 1;
	tetromino[4][2][1].code = 4;
	tetromino[4][2][2].x = 0;
	tetromino[4][2][2].y = 0;
	tetromino[4][2][2].code = 4;
	tetromino[4][2][3].x = 1;
	tetromino[4][2][3].y = 0;
	tetromino[4][2][3].code = 4;
	
	tetromino[4][3][0].x = -1;
	tetromino[4][3][0].y = -1;
	tetromino[4][3][0].code = 4;
	tetromino[4][3][1].x = -1;
	tetromino[4][3][1].y = 0;
	tetromino[4][3][1].code = 4;
	tetromino[4][3][2].x = 0;
	tetromino[4][3][2].y = 0;
	tetromino[4][3][2].code = 4;
	tetromino[4][3][3].x = 0;
	tetromino[4][3][3].y = 1;
	tetromino[4][3][3].code = 4;

	//J미노
	
	tetromino[5][0][0].x = 1;
	tetromino[5][0][0].y = 0;
	tetromino[5][0][0].code = 5;
	tetromino[5][0][1].x = 0;
	tetromino[5][0][1].y = 0;
	tetromino[5][0][1].code = 5;
	tetromino[5][0][2].x = -1;
	tetromino[5][0][2].y = 0;
	tetromino[5][0][2].code = 5;
	tetromino[5][0][3].x = -1;
	tetromino[5][0][3].y = -1;
	tetromino[5][0][3].code = 5; 

	tetromino[5][1][0].x = 0;
	tetromino[5][1][0].y = 1;
	tetromino[5][1][0].code = 5;
	tetromino[5][1][1].x = 0;
	tetromino[5][1][1].y = 0;
	tetromino[5][1][1].code = 5;
	tetromino[5][1][2].x = 0;
	tetromino[5][1][2].y = -1;
	tetromino[5][1][2].code = 5;
	tetromino[5][1][3].x = 1;
	tetromino[5][1][3].y = -1;
	tetromino[5][1][3].code = 5;

	tetromino[5][2][0].x = -1;
	tetromino[5][2][0].y = 0;
	tetromino[5][2][0].code = 5;
	tetromino[5][2][1].x = 0;
	tetromino[5][2][1].y = 0;
	tetromino[5][2][1].code = 5;
	tetromino[5][2][2].x = 1;
	tetromino[5][2][2].y = 0;
	tetromino[5][2][2].code = 5;
	tetromino[5][2][3].x = 1;
	tetromino[5][2][3].y = 1;
	tetromino[5][2][3].code = 5;
	
	tetromino[5][3][0].x = 0;
	tetromino[5][3][0].y = -1;
	tetromino[5][3][0].code = 5;
	tetromino[5][3][1].x = 0;
	tetromino[5][3][1].y = 0;
	tetromino[5][3][1].code = 5;
	tetromino[5][3][2].x = 0;
	tetromino[5][3][2].y = 1;
	tetromino[5][3][2].code = 5;
	tetromino[5][3][3].x = -1;
	tetromino[5][3][3].y = 1;
	tetromino[5][3][3].code = 5;
	
	//T미노 
	
	tetromino[6][0][0].x = 1;
	tetromino[6][0][0].y = 0;
	tetromino[6][0][0].code = 6;
	tetromino[6][0][1].x = 0;
	tetromino[6][0][1].y = 0;
	tetromino[6][0][1].code = 6;
	tetromino[6][0][2].x = -1;
	tetromino[6][0][2].y = 0;
	tetromino[6][0][2].code = 6;
	tetromino[6][0][3].x = 0;
	tetromino[6][0][3].y = -1;
	tetromino[6][0][3].code = 6;
	
	tetromino[6][1][0].x = 0;
	tetromino[6][1][0].y = 1;
	tetromino[6][1][0].code = 6;
	tetromino[6][1][1].x = 0;
	tetromino[6][1][1].y = 0;
	tetromino[6][1][1].code = 6;
	tetromino[6][1][2].x = 0;
	tetromino[6][1][2].y = -1;
	tetromino[6][1][2].code = 6;
	tetromino[6][1][3].x = 1;
	tetromino[6][1][3].y = 0;
	tetromino[6][1][3].code = 6;
	
	tetromino[6][2][0].x = 1;
	tetromino[6][2][0].y = 0;
	tetromino[6][2][0].code = 6;
	tetromino[6][2][1].x = 0;
	tetromino[6][2][1].y = 0;
	tetromino[6][2][1].code = 6;
	tetromino[6][2][2].x = -1;
	tetromino[6][2][2].y = 0;
	tetromino[6][2][2].code = 6;
	tetromino[6][2][3].x = 0;
	tetromino[6][2][3].y = 1;
	tetromino[6][2][3].code = 6;
	
	tetromino[6][3][0].x = 0;
	tetromino[6][3][0].y = 1;
	tetromino[6][3][0].code = 6;
	tetromino[6][3][1].x = 0;
	tetromino[6][3][1].y = 0;
	tetromino[6][3][1].code = 6;
	tetromino[6][3][2].x = 0;
	tetromino[6][3][2].y = -1;
	tetromino[6][3][2].code = 6;
	tetromino[6][3][3].x = -1;
	tetromino[6][3][3].y = 0;
	tetromino[6][3][3].code = 6;
	
	//I미노
	
	tetromino[7][0][0].x = -1;
	tetromino[7][0][0].y = 0;
	tetromino[7][0][0].code = 7;
	tetromino[7][0][1].x = 0;
	tetromino[7][0][1].y = 0;
	tetromino[7][0][1].code = 7;
	tetromino[7][0][2].x = 1;
	tetromino[7][0][2].y = 0;
	tetromino[7][0][2].code = 7;
	tetromino[7][0][3].x = 2;
	tetromino[7][0][3].y = 0;
	tetromino[7][0][3].code = 7; 
	
	tetromino[7][1][0].x = 1;
	tetromino[7][1][0].y = -1;
	tetromino[7][1][0].code = 7;
	tetromino[7][1][1].x = 1;
	tetromino[7][1][1].y = 0;
	tetromino[7][1][1].code = 7;
	tetromino[7][1][2].x = 1;
	tetromino[7][1][2].y = 1;
	tetromino[7][1][2].code = 7;
	tetromino[7][1][3].x = 1;
	tetromino[7][1][3].y = 2;
	tetromino[7][1][3].code = 7;
	
	tetromino[7][2][0].x = -1;
	tetromino[7][2][0].y = 1;
	tetromino[7][2][0].code = 7;
	tetromino[7][2][1].x = 0;
	tetromino[7][2][1].y = 1;
	tetromino[7][2][1].code = 7;
	tetromino[7][2][2].x = 1;
	tetromino[7][2][2].y = 1;
	tetromino[7][2][2].code = 7;
	tetromino[7][2][3].x = 2;
	tetromino[7][2][3].y = 1;
	tetromino[7][2][3].code = 7;
	
	tetromino[7][3][0].x = 0;
	tetromino[7][3][0].y = -1;
	tetromino[7][3][0].code = 7;
	tetromino[7][3][1].x = 0;
	tetromino[7][3][1].y = 0;
	tetromino[7][3][1].code = 7;
	tetromino[7][3][2].x = 0;
	tetromino[7][3][2].y = 1;
	tetromino[7][3][2].code = 7;
	tetromino[7][3][3].x = 0;
	tetromino[7][3][3].y = 2;
	tetromino[7][3][3].code = 7;
	
	/*
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
	*/
}

