#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>

#define MAX_SIZE 1000
//graphic
#define RENDER_MODE 1
#define TEXTURE_COUNT 12
#define BLOCK_SIZE 16
#define SCR_SIZE (BLOCK_SIZE * 2)

enum
{
	LOST = -1,
	IN_PROGRESS,
	WON
};

enum
{
	EMPTY = 0,
	MINE = 9,
	HIDDEN = 10,
	FLAGGED = 11
};

typedef struct pixel{
	UINT8	b, g, r, a;
}	pixel;

typedef struct gameInfo
{
	int								x, y, bombCount;
	pixel**							textures;
	std::vector<std::vector<int8_t>>	field;
	std::vector<std::vector<int8_t>>	visible;
}	gameInfo;

//MineSweeper.cpp
gameInfo	getUserInput();
void		makeField(gameInfo* info, int safeX, int safeY, bool setZero);
void		printField(gameInfo* info, bool override);
int			reveal(gameInfo* info, int x, int y);
void		endGame(int result, gameInfo* info);
int			finishedGame(gameInfo* info);

//render.cpp
void    renderMain();
