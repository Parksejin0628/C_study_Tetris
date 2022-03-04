#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include<conio.h>
#include<stdlib.h>
#include"declarationData.h"

#define KEY_DOWN 0x8000
#define KEY_UP 0x0001
#define KEY_KEEP 0x8001
#define BASEUI 1
#define NEXTUI 2
#define NEWBLOCK 0
#define HOLDBLOCK 1

typedef short key;

void settingGame();														//게임 실행시 초기 세팅 
void printBoard(int blockCode, double x, int y);						//blockCode값에 따라 지정된 문자를 출력하는 함수 
int updateGame();														//지속적으로 호출되는 함수 
void goto_xy(int x, int y);												//커서위치를 옮기는 함수  
void createBlock(int code);												//블록을 생성하는 함수 
void fallingBlock();													//일정 시간마다 블록이 낙하하는 것을 제어하는 함수  
void softDrop();														//소프트 드롭을 실행하는 함수 
int blockMoveSimulation(block blockQueue[4], int moveX, int moveY); 	//블록이 움직이는 것을 시뮬레이션해서 불가능한 경우 0, 가능한 경우 1을 리턴하고 preloadBlockQueue를 업데이트 해주는 함수  
void inputPreloadBlockQueue(int index, int code, int x, int y);			//preloadBlockQueue에 정보를 입력하는 함수
void reloadBlock();														//현재 조종중인 블록의 좌표를 최신화해주는 함수 
int blockRotationSimulation(block blockQueue[4], int rotDir);			//블록을 회전시키는 것을 시뮬레이션해주는 함수 
void inputKey();														//키 입력을 받는 함수 
void landBlock(block blockQueue[4], int decreseY);						//블록을 설치하는 함수
void checkLine(int line);												//y번째 줄이 가득 찼는지 확인하는 함수 
void hardDrop();														//하드 드롭을 실행하는 함수
void printUI(int numUI, int *data1, int *data2, int *data3);			//UI를 출력하는 함수 
void createBag(int *bag7);												//7bag를 생성하는 함수
void hold();															//hold를 구현한 함수 
int importNext();														//next에서 다음 블록을 리턴하고, next를 최신화 하는 함수 
void setColor(unsigned short color);									//콘솔 글자의 색을 지정하는 함수 
void searchHardDrop(block movingBlock);							//고스트의 위치를 파악 및 출력하기 위한 함수 
void reloadBoard();														//보드 전체의 수정사항을 적용하는 함수 


int board[24][12] = {0};												//게임 보드판 변수 
block blockQueue[4] = {0};												//현재 플레이어가 조작하고 있는 블록의 정보를 담은 변수 
block preloadBlockQueue[4] = {0};										//블록의 이동예정인 좌표의 정보를 담고 있는 변수 
time_t gameStartTime = 0;												//게이을 시작한 시각을 저장하는 변수 
time_t fallingCriteriaTime = 0;											//낙하 딜레의 기준이 되어주는 변수 
time_t moveCriteriaTime = 0;											//블록제어 딜레이의 기준이 되어주는 변수  
time_t rotCriteriaTime = 0;												//블록회전 딜레이의 기준이 되어주는 변수 
time_t fallingDelayTime = 1000;											//블록낙하 딜레이 시간  
time_t moveDelayTime = 167;												//블록이동 딜레이 시간 
time_t rotDelayTime = 100;												//블록회전 딜레이 시간  
time_t fallingDelayDecreaseTime = 5;									//블록낙하 딜레이 감소량
time_t ARR = 33;
time_t DAS = 167;
time_t infinityDelayTime = 1000;
time_t infinityCriteriaTime = 0;
int blockRot = 0;														//블록의 회전 상태를 저장하는 변수			
int clearLine = 0;														//제거한 줄의 수  
int targetLine = 40;													//목표 줄의 수 
int holdValue = 0; 														//hold에 있는 블록 값  
int preloadBoard[24][12] = {0};
key keydown_left = 0;
key keydown_right = 0;
key keydown_up = 0;
key keydown_down = 0;
key keydown_z = 0;
key keydown_x = 0;
key keydown_c = 0;
key keydown_space = 0;

int holdAble = 1;

//반드시 필요한 전역 변수들  

int playing = 1; 														//플레이 여부
time_t nowTime = 0;														//현재시간 즉, 게임 시작 후 얼마나 지났는지를 저장하는 변수  
int blockExistence = 0;													//현재 플레이어가 조종하는 블록이 존재하는지 판단하는 변수 
int blockCreatePosX = 5;
int blockCreatePosY = 2;
int boardPosX = 5;

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
	createBlock(0); //현재 진행중  
	fallingBlock();
	inputKey();
	printUI(BASEUI, NULL, NULL, NULL);
	reloadBoard();

	
	return 1;
}

void settingGame()
{
//초기 변수 설정 
	//board변수 초기 설정 
	for(int row=0; row<=23; row++)
	{
		board[row][0] = -1;
		board[row][11] = -1;
	}
	for(int column=0; column<=11; column++)
	{
		//board[0][column] = -1;
		board[23][column] = -1;
	}
	//랜덤 시드값 생성 
	srand((unsigned int)time(NULL));
	
	//테트로미노 변수 선언 
	settingTetromino();
	//월킥 데이터 선언 
	settingWallkickData();
	
	//초기 시간변수 설정 
	gameStartTime = clock();
	fallingCriteriaTime = clock();
	rotCriteriaTime = clock();
	
	//초기 next값 설정 
	for(int i=0; i<6; i++)
	{
		importNext();
	}
	
	
	
//초기 화면출력 세팅 
	//보드판 생성 
	for(int row=3; row<=23; row++)
	{
		for(int column=0; column<=11; column++)
		{
			printBoard(board[row][column], column + boardPosX, row);
		}
		printf("\n");
	}
	
	//hold칸 생성 
	goto_xy(0, 3);
	setColor(8); //회색 
	printf("□□hold□\n");
	printf("□\n");
	printf("□\n");
	printf("□□□□□\n");
	
	//next칸 생성 
	for(int i=3; i<=18; i++)
	{
		setColor(8); //회색 
		goto_xy(34, i);
		if(i == 3)
		{
			printf("□next□□");
		}
		else if(i==18)
		{
			printf("□□□□□");
		}
		else if((i-3)%3 == 0)
		{
			printf("--------□");
		}
		else
		{
			printBoard(-1, 21, i);
		}
	}
	
	//preloadBoard를 board값과 같게하기
	for(int row = 0; row < 23; row++)
	{
		for(int column = 0; column < 12; column++)
		{
			preloadBoard[row][column] = board[row][column];
		}
	 } 
	
	
	return;
}

void printBoard(int blockCode, double x, int y)
{
	x*=2;
	goto_xy(x, y); 
	
	//블록 코드에 따른 출력 
	if(blockCode == -1)
	{
		setColor(8);
		printf("□");
	}
	else if(blockCode == 0)
	{
		printf("  ");
	}
	else
	{
		if(blockCode == 1 || blockCode == 8)
		{
			setColor(4); //빨강 
		}
		else if(blockCode == 2 || blockCode == 9)
		{
			setColor(7); //하양 
		}
		else if(blockCode == 3 || blockCode == 10)
		{
			setColor(6); //노랑  
		}
		else if(blockCode == 4 || blockCode == 11)
		{
			setColor(2); //초록 
		}
		else if(blockCode == 5 || blockCode == 12)
		{
			setColor(1); //파랑 
		}
		else if(blockCode == 6 || blockCode == 13)
		{
			setColor(5); //보라 
		}
		else if(blockCode == 7 || blockCode == 14)
		{
			setColor(3); //청록색 
		}
		else if(blockCode == 15)
		{
			setColor(12); //옅은 빨강 
		}
		else if(blockCode == 16)
		{
			setColor(8); //옅은 하양 
		}
		else if(blockCode == 17)
		{
			setColor(14); //옅은 노랑 
		}
		else if(blockCode == 18)
		{
			setColor(10); //옅은 초록 
		}
		else if(blockCode == 19)
		{
			setColor(9); //옅은 파랑 
		}
		else if(blockCode == 20)
		{
			setColor(13); //옅은 보라  
		}
		else if(blockCode == 21)
		{
			setColor(11); //옅은 청록 
		}
		printf("■");
	}
	setColor(7); 
	
	return;
}

void createBlock(int code)
{
	//제어중인 블록이 있을 경우 블록을 생성하지 않는다. 
	if(blockExistence == 1)
	{
		return;
	}
	
	int blockCode = 0;
	int X = 0;
	int Y = 0;
	
	//새로운 블록을 생성하는 경우 
	if(code == NEWBLOCK) 
	{
		blockCode = importNext(); 
		holdAble = 1;
	}
	//정해진 블록 (주로 hold에 있던 블록)을 생성하는 경우 
	else if(code == HOLDBLOCK)
	{
		blockCode = code;
	}
	// 새로운 블록의 좌표 설정 및 블록 출력 
	for(int i=0; i<=3; i++)
	{
		X = blockCreatePosX + tetromino[blockCode][0][i].x;
		Y = blockCreatePosY + tetromino[blockCode][0][i].y;
		board[Y][X] = blockCode;
		blockQueue[i].x = X;
		blockQueue[i].y = Y;
		blockQueue[i].code = blockCode;
		
		//inputPreloadBlockQueue(i, blockCode, X, Y); 
	}
	
	/*for(int i=0; i<=3; i++)
	{
		shadowPos[i].x = 0;
		shadowPos[i].y = 0;
	}*/
	searchHardDrop(); //현재 진행중 
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
	if(nowTime - fallingCriteriaTime >= fallingDelayTime)
	{
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
		if(nowTime - infinityCriteriaTime >= infinityDelayTime)
		{
			fallingCriteriaTime = clock();
			landBlock(blockQueue, 0);
		}
		
		return;
	}
	else
	{
		fallingCriteriaTime = clock();
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
	
	infinityCriteriaTime = clock();
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		
		board[Y][X] = 0;
	}
	
	CODE = preloadBlockQueue[0].code;
	for(int i=0; i<=3; i++)
	{
		X = preloadBlockQueue[i].x;
		Y = preloadBlockQueue[i].y;
		
		board[Y][X] = CODE;
		blockQueue[i] = preloadBlockQueue[i];
	}
	if(blockExistence == 1)	searchHardDrop();
	
	return; 
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
	int keycode_c = 0x43;
	if(GetAsyncKeyState(VK_LEFT))
	{
		if(keydown_left == 1 && (nowTime - moveCriteriaTime) > moveDelayTime)
		{
			keydown_left = 0;
			moveDelayTime = ARR;
		} 
		if(keydown_left == 0)
		{
			moveCriteriaTime = clock();
			keydown_left = 1;
			temp = blockMoveSimulation(blockQueue, -1, 0);
			if(temp != 0)
			{
				reloadBlock();
			}
		}
		
	}
	else if(!GetAsyncKeyState(VK_LEFT) && keydown_left == 1)	
	{
		keydown_left = 0;
		moveDelayTime = DAS;
	}
	
	if(GetAsyncKeyState(VK_RIGHT))
	{
		if(keydown_right == 1 && (nowTime - moveCriteriaTime) > moveDelayTime)
		{
			keydown_right = 0;
			moveDelayTime = ARR;
		} 
		if(keydown_right == 0)
		{
			moveCriteriaTime = clock();
			keydown_right = 1;
			temp = blockMoveSimulation(blockQueue, 1, 0);
			if(temp != 0)
			{
				reloadBlock();
			}
		}
		
	}
	else if(!GetAsyncKeyState(VK_RIGHT) && keydown_right == 1)	
	{
		keydown_right = 0;
		moveDelayTime = DAS;
	}
	
	if(GetAsyncKeyState(VK_DOWN))
	{
		if(keydown_down == 1 && (nowTime - moveCriteriaTime) > moveDelayTime)
		{
			keydown_down = 0;
		} 
		if(keydown_down == 0)
		{
			moveCriteriaTime = clock();
			moveDelayTime = ARR*2;
			keydown_down = 1;
			softDrop();
		}
	}
		
	else if(!GetAsyncKeyState(VK_DOWN) && keydown_down == 1)	
	{
		keydown_down = 0;
		moveDelayTime = DAS;
	}
	
	/*if(GetAsyncKeyState(VK_DOWN) && keydown_down == 0)
	{
		/*if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} 
		keydown_down = 1;
		moveCriteriaTime = clock();
		softDrop();
	}
	else if(!GetAsyncKeyState(VK_DOWN))	keydown_down = 0;*/
	
	if(GetAsyncKeyState(keycode_z) && keydown_z == 0)
	{
		/*if(nowTime - rotCriteriaTime < rotDelayTime)
		{
			return;
		} */
		keydown_z = 1;
		rotCriteriaTime = clock();
		temp = blockRotationSimulation(blockQueue, -1);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
	else if(!GetAsyncKeyState(keycode_z))	keydown_z = 0;
	
	if(GetAsyncKeyState(keycode_x) && keydown_x == 0)
	{
		/*if(nowTime - rotCriteriaTime < rotDelayTime)
		{
			return;
		} */
		keydown_x = 1;
		rotCriteriaTime = clock();
		temp = blockRotationSimulation(blockQueue, 1);
		if(temp != 0)
		{
			reloadBlock();
		}
	}
	else if(!GetAsyncKeyState(keycode_x))	keydown_x = 0;
	
	if(GetAsyncKeyState(VK_SPACE) && keydown_space == 0)
	{
		/*if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} */
		keydown_space = 1;
		moveCriteriaTime = clock();
		hardDrop();
	}
	else if(!GetAsyncKeyState(VK_SPACE))	keydown_space = 0;
	
	if(GetAsyncKeyState(keycode_c) && keydown_c == 0)
	{
		/*if(nowTime - moveCriteriaTime < moveDelayTime)
		{
			return;
		} */
		keydown_c = 1;
		moveCriteriaTime = clock();
		hold();
	}
	else if(!GetAsyncKeyState(keycode_c))	keydown_c = 0;
}

int blockRotationSimulation(block blockQueue[4], int rotDir)
{
	int X = 0;
	int Y = 0;
	int CODE = blockQueue[0].code;
	block kick; 
	int poss = 0;
	int IData = 0;
	
	for(int test=0; test<5; test++)
	{
		if(CODE != 7)
		{
			for(int i=0; i<=3; i++)
			{
				X = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].x - tetromino[CODE][blockRot][i].x;
				Y = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].y - tetromino[CODE][blockRot][i].y;
				kick.x = wallkickData_normal[blockRot][test].x - wallkickData_normal[(blockRot + 4 + rotDir) % 4][test].x;
				kick.y = wallkickData_normal[blockRot][test].y - wallkickData_normal[(blockRot + 4 + rotDir) % 4][test].y;
				kick.y *= -1; //(y좌표가 커질수록 밑으로 내려가기 떄문에 -1를 곱해준다.)
				//goto_xy(0, 27);
				//printf("kick.x : %d , kick.y : %d            ", kick.x, kick.y);
			
				if(board[blockQueue[i].y + Y + kick.y][blockQueue[i].x + X + kick.x] != 0 && board[blockQueue[i].y + Y + kick.y][blockQueue[i].x + X + kick.x] != CODE)
				{
					break;
				}
				inputPreloadBlockQueue(i, CODE, blockQueue[i].x + X + kick.x, blockQueue[i].y + Y + kick.y);
				if(i==3)	poss = 1;
			}
		}
		else if(CODE == 7)
		{
			for(int i=0; i<=3; i++)
			{
				X = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].x - tetromino[CODE][blockRot][i].x;
				Y = tetromino[CODE][(blockRot + 4 + rotDir) % 4][i].y - tetromino[CODE][blockRot][i].y;
				if(blockRot == 0)
				{
					if(rotDir == 1)
					{
						IData = 0;
					}
					else
					{
						IData = 5;
					}
				}
				else if(blockRot == 1)
				{
					if(rotDir == 1)
					{
						IData = 1;
					}
					else
					{
						IData = 8;
					}
				}
				else if(blockRot == 2)
				{
					if(rotDir == 1)
					{
						IData = 2;
					}
					else
					{
						IData = 7;
					}
				}
				else
				{
					if(rotDir == 1)
					{
						IData = 3;
					}
					else
					{
						IData = 6;
					}
				}
				kick.x = wallkickData_I[IData][test].x;
				kick.y = wallkickData_I[IData][test].y;
				kick.y *= -1; //(y좌표가 커질수록 밑으로 내려가기 떄문에 -1를 곱해준다.)
			
				if(board[blockQueue[i].y + Y + kick.y][blockQueue[i].x + X + kick.x] != 0 && board[blockQueue[i].y + Y + kick.y] [blockQueue[i].x + X + kick.x] != CODE)
				{
					break;
				}
				inputPreloadBlockQueue(i, CODE, blockQueue[i].x + X + kick.x, blockQueue[i].y + Y + kick.y);
				if(i==3)	poss = 1;
			}
		}
		if(poss == 1)
		{
			blockRot = (blockRot + 4 + rotDir) % 4;
			return 1;
		}
	}
	
	
	return 0;
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
		inputPreloadBlockQueue(i,CODE + 7, X, Y + decreaseY);
		line[Y + decreaseY] = 1;
	}
	blockExistence = 0;
	reloadBlock();
	for(int i=0; i<24; i++)
	{
		if(line[i] != 0)
		{
			checkLine(i);
		}
	}
	if(fallingDelayTime >= 50)
	{
		fallingDelayTime -= fallingDelayDecreaseTime;
	}
	searchHardDrop();
		
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
	}
	for(int y = line - 1; y>=0; y--)
	{
		for(int x = 1; x<=10; x++)
		{
			board[y+1][x] = board[y][x];
			board[y][x] = 0;
		}
	}
	clearLine++; 
	
	return;
}

void hardDrop()
{
	int temp = 0;
	
	for(int i=1; i<=30; i++)
	{
		temp = blockMoveSimulation(blockQueue, 0, i);
		if(temp == 0)
		{
			landBlock(blockQueue, i-1);
			
			return;
		}
	}
	
	return;
}

void printUI(int numUI, int *data1, int *data2, int *data3)
{
	int X = 0;
	int Y = 0;
	
	if(numUI == BASEUI)
	{
		goto_xy(0, 28);
		printf("현재 시간 | %d:%d.%d         ", nowTime / 60000, (nowTime / 1000)%60, nowTime % 1000);
		goto_xy(0, 29);
		printf("line : %d / %d        ", clearLine, targetLine);
		/*goto_xy(0, 30);
		printf("blockCode : %d             ", blockQueue[0].code);*/
		if(clearLine >= targetLine)
		{
			goto_xy (0, 30);
			printf("clear!                 \n");
			playing = 0;
		}
	}
	else if(numUI == NEXTUI)
	{
		int *next = data1;
		int *nextFrontIndex = data2;
		int *nextRearIndex = data3;
		for(int y=4; y<=17; y++)
		{
			if((y-3) % 3 != 0)
			{
				printBoard(0, 17, y);
				printBoard(0, 18, y);
				printBoard(0, 19, y);
				printBoard(0, 20, y);
			}
		}
		for(int i=*nextFrontIndex; i<=*nextRearIndex ; i++)
		{
			for(int j=0; j<=3; j++)
			{
				X = tetromino[next[i%5]][0][j].x;
				Y = tetromino[next[i%5]][0][j].y;
				if(next[i%5] == 3)
				{
					printBoard(next[i%5], 18 + X, 5 + (3*(i-*nextFrontIndex)) + Y);
				}
				else if(next[i%5] == 7)
				{
					printBoard(next[i%5], 18 + X, 5 + (3*(i-*nextFrontIndex)) + Y);
				}
				else
				{
					printBoard(next[i%5], 18.5 + X, 5 + (3*(i-*nextFrontIndex)) + Y);
				}
			}
		}
	}
	
	return;
}

//변수 최적화 완료 
void createBag(int *bag7)
{
	int randomValue = 0;
	int temp = 0;
	
	for(int i=0; i<7; i++)
	{
		bag7[i] = i+1;
	}
	for(int i=0; i<7; i++)
	{
		randomValue = rand() % 7;
		temp = bag7[i];
		bag7[i] = bag7[randomValue];
		bag7[randomValue] = temp;
	}
	
	return;
} 

void hold()
{
	int originValue = holdValue;
	int X = 0;
	int Y = 0;
	
	if(holdAble == 0)
	{
		return;
	}
	holdAble = 0;
	holdValue = blockQueue[0].code;
	for(int i=0; i<=3; i++)
	{
		printBoard(0, 1+i, 4);
		printBoard(0, 1+i, 5);
	}
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		
		inputPreloadBlockQueue(i, 0, X, Y);
		
		X = tetromino[holdValue][0][i].x;
		Y = tetromino[holdValue][0][i].y;
		if(holdValue == 3)
		{
			printBoard(holdValue, 2 + X, 5 + Y);
		}
		else if(holdValue == 7)
		{
			printBoard(holdValue, 2 + X, 5 + Y);
		}
		else
		{
			printBoard(holdValue, 2.5 + X, 5 + Y);
		}
		
	}
	blockExistence = 0;
	reloadBlock();
	for(int i=0; i<=3; i++)		//기존 그림자 지우기 
	{
		printBoard(0, shadowPos[i].x, shadowPos[i].y);
	}
	createBlock(originValue);
	
	
	return;
}

//변수 최적화 완료 
int importNext()
{
	static int bag7[7] = {0};														//7bag값을 저장하는 함수 
	static int next[5] = {0};														//next값을 저장하는 함수 
	static int bagIndex = 7;														//7bag의 인덱스 (처음 블록과 next블록 5개를 뽑아야 하기 때문에 시작을 5로 했다.) 
	static int nextFrontIndex = 0;													//nextQueue의 frontIndex 
	static int nextRearIndex = 4;													//nextQueue의 reatIndex
	
	int nextBlock = next[(nextFrontIndex) % 5];
	
	
	nextFrontIndex ++;
	nextRearIndex ++;
	next[(nextRearIndex)%5] = bag7[bagIndex];
	bagIndex++;
	if(bagIndex >= 7)
	{
		createBag(bag7);
		bagIndex = 0;
	}
	printUI(NEXTUI, next, &nextFrontIndex, &nextRearIndex); 
		
	return nextBlock;
} 

//0 : 검은색 / 1 : 파란색 / 2 : 녹색 / 3 : 청록색 / 4 : 빨간색 / 5 : 자주색 / 6 : 노란색 / 7 : 흰색 / 8~ : 0~7의 옅은색 
void setColor(unsigned short color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	
	return;
}

void searchHardDrop(block movingBlock)
{
	int temp = 0;
	int decreaseY = 0;
	int search = 0;
	static block shadowPos[4] = {0};												//기존 그림자가 있던 좌표 
	
	if(code > 7)	code -= 7;
	for(int i=0; i<=3; i++)
	{
		printBoard(0, shadowPos[i].x, shadowPos[i].y);
		shadowPos[i].x = 0;
		shadowPos[i].y = 0;
	}
	
	for(int y=1; y<=30; y++)
	{
		for(int i=0; i<=3; i++)
		{
			if(board[Y + y][X] != 0 && board[Y + y][X] != CODE)
			{
				decreaseY = y;
				search = 1;
			}
		}
		if(search == 1)
		{
			break;
		}
	}
	for(int i=0; i<=3; i++)
	{
		X = blockQueue[i].x;
		Y = blockQueue[i].y;
		if(board[Y + decreaseY -1][X] == 0)
		{
			printBoard(CODE + 14, X + boardPosX ,Y + decreaseY -1);
			shadowPos[i].x = X + boardPosX;
			shadowPos[i].y = Y + decreaseY -1;
		}
	}

	
	return;
}

void reloadBoard()
{
	for(int row = 0; row < 24; row++)
	{
		for(int column = 0; column < 12; column++)
		{
			if(board[row][column] != preloadBoard[row][column])
			{
				preloadBoard[row][column] = board[row][column];
				printBoard(board[row][column], column + boardPosX, row);
			}
		}
	}
	return;
}


