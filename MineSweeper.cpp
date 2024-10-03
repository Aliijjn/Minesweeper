#include "MineSweeper.hpp"

//terminal
HANDLE						hConsole;
CONSOLE_SCREEN_BUFFER_INFO	consoleInfo;
WORD						saved_attributes;
DWORD						written;

void	convertInt(int* i, std::string prompt, int lowerBound, int upperBound)
{
	std::string	temp;

	while (1)
	{
		std::cout << prompt;
		getline(std::cin, temp);
		try
		{
			*i = std::stoi(temp);
		}
		catch (...)
		{
			std::cout << "ERROR: invalid argument, should be a number\n\n";
			continue;
		}
		if (*i < lowerBound)
		{
			std::cout << "ERROR: value should not be less than " << lowerBound << "\n\n";
		}
		else if (*i > upperBound)
		{
			std::cout << "ERROR: value should not be greater than " << upperBound << "\n\n";
		}
		else
		{
			break;
		}
	}
}

std::string toUpper(std::string str)
{
	std::string cpy = str;
	for (int i = 0; cpy[i]; i++)
		cpy[i] = toupper(cpy[i]);
	return (cpy);
}

gameInfo	getUserInput()
{
	gameInfo	info = { 0, 0, 0 };
	std::string	str;
	float		bombFactor = 0;

	while (1)
	{
		std::cout << "S: Small (10x10)\nM: Medium (15x15)\nL: Large (20x20)\nH: Huge (26x26)\nC: Custom\nPlease enter a field size: ";
		getline(std::cin, str);
		str = toUpper(str);
		if (str == "S" || str == "SMALL")
		{
			info.x = 10;
			info.y = 10;
		}
		else if (str == "M" || str == "MEDIUM")
		{
			info.x = 15;
			info.y = 15;
		}
		else if (str == "L" || str == "LARGE")
		{
			info.x = 20;
			info.y = 20;
		}
		else if (str == "H" || str == "HUGE")
		{
			info.x = 26;
			info.y = 26;
		}
		else if (str == "C" || str == "CUSTOM")
		{
			convertInt(&(info.x), "Enter the field width (1-" + std::to_string(MAX_SIZE) + "): ", 1, MAX_SIZE);
			convertInt(&(info.y), "Enter the field height (1-" + std::to_string(MAX_SIZE) + "): ", 1, MAX_SIZE);
			if (info.x * info.y < 10)
			{
				std::cout << "ERROR: Map should be at least 10 blocks total\n";
				continue;
			}
		}
		else
		{
			std::cout << "ERROR: Invalid input, please try again\n";
			continue;
		}
		break;
	}
	while (1)
	{
		std::cout << "\nE: Easy (10%)\nM: Medium(15%)\nH: Hard (20%)\nX: Extreme (30%)\nC: Custom\nPlease select a difficulty (bomb %): ";
		getline(std::cin, str);
		str = toUpper(str);
		if (str == "E" || str == "EASY")
		{
			bombFactor = .1;
		}
		else if (str == "M" || str == "MEDIUM")
		{
			bombFactor = .15;
		}
		else if (str == "H" || str == "HARD")
		{
			bombFactor = .20;
		}
		else if (str == "X" || str == "EXTREME")
		{
			bombFactor = .30;
		}
		else if (str == "C" || str == "CUSTOM")
		{
			convertInt(&(info.bombCount), "Enter the bomb count (1-" + std::to_string(info.x * info.y - 9) + "): ", 1, info.x * info.y - 9);
		}
		else
		{
			std::cout << "ERROR: Invalid input, please try again\n";
			continue;
		}
		if (bombFactor != 0)
		{
			info.bombCount = info.x * info.y * bombFactor;
		}
		break;
	}
	return info;
}

UINT tRand(int modulo)
{
	UINT x = __rdtsc();
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x % modulo;
}

//Checks for mines, is protected for bad coord input
bool	checkMine(int x, int y, gameInfo* info)
{
	if (x < 0 || y < 0 || x >= info->x || y >= info->y)
		return false;
	return info->field[y][x] == MINE;
}

int		checkMineCount(int x, int y, gameInfo* info)
{
	return	checkMine(x - 1, y - 1, info) + checkMine(x, y - 1, info) + checkMine(x + 1, y - 1, info) +
			checkMine(x - 1, y, info)     +                             checkMine(x + 1, y, info) +
			checkMine(x - 1, y + 1, info) + checkMine(x, y + 1, info) + checkMine(x + 1, y + 1, info);
}

void	makeField(gameInfo* info, int safeX, int safeY, bool setZero)
{
	std::vector<std::vector<int8_t>>	field(info->y, std::vector<int8_t>(info->x, 0));
	std::vector<std::vector<int8_t>>	visible(info->y, std::vector<int8_t>(info->x, HIDDEN));
	info->field = field;
	info->visible = visible;
	if (setZero)
		return;

	UINT	x, y;

	for (int i = 0; i < info->bombCount; i++)
	{
		do
		{
			x = tRand(info->x);
			y = tRand(info->y);
		} 
		while (info->field[y][x] == MINE || ((x == safeX - 1 || x == safeX || x == safeX + 1) && (y == safeY - 1 || y == safeY || y == safeY + 1)));
		info->field[y][x] = MINE;
	}
	for (y = 0; y < info->y; y++)
	{
		for (x = 0; x < info->x; x++)
		{
			if (info->field[y][x] != MINE)
			{
				info->field[y][x] = checkMineCount(x, y, info);
			}
		}
	}
}

void	printField(gameInfo* info, bool override)
{
	char		c;
	std::string y_coord;

	std::cout << "\n   ";
	for (char c = 'a'; c <= 'z' && c < info->x + 'a'; c++)
	{
		std::cout << c << ' ';
	}
	for (int y = 0; y < info->y; y++)
	{
		y_coord = y >= 10 ? " " : "  ";
		std::cout << '\n' << y << y_coord;
		for (int x = 0; x < info->x; x++)
		{
			if (!info->visible[y][x] && override == false)
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_INTENSITY);
				std::cout << ". ";
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				continue;
			}
			switch (info->field[y][x])
			{
				case MINE:
					c = 'X';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
					break;
				case EMPTY:
					c = '.';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					break;
				case 1:
					c = '1';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
					break; 
				case 2:
					c = '2';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_GREEN);
					break;
				case 3:
					c = '3';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_RED);
					break;
				case 4:
					c = '4';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED);
					break;
				default:
					c = info->field[y][x] + '0';
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE);
					break;
			}
			std::cout << c << ' ';
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}
	}
	std::cout << "\n\n";
}

int	finishedGame(gameInfo* info)
{
	for (int y = 0; y < info->y; y++)
	{
		for (int x = 0; x < info->x; x++)
		{
			if (info->field[y][x] != MINE && info->visible[y][x] == HIDDEN)
				return IN_PROGRESS;
		}
	}
	return WON;
}

int	reveal(gameInfo* info, int x, int y)
{
	if (x < 0 || y < 0 || x >= info->x || y >= info->y || info->visible[y][x] == EMPTY)
		return IN_PROGRESS;
	info->visible[y][x] = EMPTY;
	if (info->field[y][x] == EMPTY)
	{
		reveal(info, x-1, y-1); reveal(info, x, y-1); reveal(info, x+1, y-1);
		reveal(info, x-1, y);   /* you are here :) */ reveal(info, x+1, y);
		reveal(info, x-1, y+1); reveal(info, x, y+1); reveal(info, x+1, y+1);
	}
	return (info->field[y][x] == MINE) ? LOST : IN_PROGRESS;
}

int	parseMove(gameInfo* info, int* xPtr, int* yPtr)
{
	std::string		input;
	int				x, y;

	while (1)
	{
		std::cout << "Please enter a square to reveal: ";
		getline(std::cin, input);
		if (!(input.length() == 2 && isalpha(input[0]) && isdigit(input[1])) && !(input.length() == 3 && isalpha(input[0]) && isdigit(input[1]) && isdigit(input[2])))
		{
			std::cout << "ERROR: should be formated as \n\"a3\" or \"G10\"\n";
			continue;
		}
		x = toupper(input[0]) - 'A';
		y = input[2] == '\0' ? input[1] - '0' : (input[1] - '0') * 10 + input[2] - '0';
		if (x < 0 || y < 0 || x >= info->x || y >= info->y)
		{
			std::cout << "ERROR: coords are out of bounds\n";
			continue;
		}
		break;
	}
	*xPtr = x;
	*yPtr = y;
	return reveal(info, x, y);
}

void	initConsole(HANDLE* hConsole, CONSOLE_SCREEN_BUFFER_INFO* consoleInfo, WORD* saved_attributes)
{
	*hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(*hConsole, consoleInfo);
	*saved_attributes = consoleInfo->wAttributes;
	SetConsoleTextAttribute(*hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void	endGame(int result, gameInfo* info)
{
	std::string	str;

	printField(info, true);

	if (result == WON)
		std::cout << "Congrats! You won :)\n";
	else
		std::cout << "Better luck next time :(\n";
	std::cout << "Press ENTER to exit\n";
	getline(std::cin, str);
	exit(0);
}

void	resetConsoleCursor()
{
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	FillConsoleOutputCharacter(hConsole, ' ', consoleInfo.dwSize.X * consoleInfo.dwSize.Y, { 0, 0 }, &written);
	SetConsoleCursorPosition(hConsole, { 0, 0 });
}

void	printInfo(int lastX, int lastY, int bombCount)
{
	std::cout << "Bomb count: " << bombCount << "\n";
	if (lastX != -1)
		std::cout << "Last move:  " << (char)(lastX + 'A') << lastY << "\n";
}

int		main()
{
	if (RENDER_MODE == 1)
		renderMain();
	initConsole(&hConsole, &consoleInfo, &saved_attributes);
	gameInfo	info = getUserInput();
	int			status = IN_PROGRESS;
	int			lastX = -1;
	int			lastY = -1;

	makeField(&info, -1, -1, false);
	while (1)
	{
		resetConsoleCursor();
		printInfo(lastX, lastY, info.bombCount);
		printField(&info, false);
		status = parseMove(&info, &lastX, &lastY);
		if (status != IN_PROGRESS)
			break;
		status = finishedGame(&info);
		if (status != IN_PROGRESS)
			break;
	}
	resetConsoleCursor();
	printInfo(lastX, lastY, info.bombCount);
	endGame(status, &info);
	return 0;
}