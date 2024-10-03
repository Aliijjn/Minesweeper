#include "MineSweeper.hpp"

LRESULT CALLBACK windowMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
void    drawField(gameInfo* info, pixel** textures, bool override);

//graphic
BITMAPINFO  bmi;
HWND        window;
HDC         WindowDC;
pixel*      vram;
WNDCLASSA   wc;

gameInfo*   info;

void    getCursorPosition(int* x, int* y)
{
    POINT   cur;
    RECT    offset;
    RECT    client;

    GetCursorPos(&cur);
    GetWindowRect(window, &offset);
    GetClientRect(window, &client);
    offset.left += client.left;
    offset.top -= client.bottom - (offset.bottom - offset.top);
    *x = cur.x - offset.left;
    *y = info->y * SCR_SIZE - (cur.y - offset.top);
}

void getRelativeCursorPosition(int* x, int* y)
{
    getCursorPosition(x, y);
    *x -= SCR_SIZE / 4;
    *y -= SCR_SIZE / 4;
    *x /= SCR_SIZE;
    *y /= SCR_SIZE;
}

void    handleLeftMouse()
{
    int         x, y, result;
    static bool hasInitialised;

    getRelativeCursorPosition(&x, &y);
    if (info->visible[y][x] == HIDDEN)
    {
        if (hasInitialised == false)
        {
            makeField(info, x, y, false);
            hasInitialised = true;
        }
        result = reveal(info, x, y);
        if (result != IN_PROGRESS)
        {
            drawField(info, info->textures, true);
            StretchDIBits(WindowDC, 0, 0, info->x * SCR_SIZE, info->y * SCR_SIZE, 0, 0, info->x * BLOCK_SIZE, info->y * BLOCK_SIZE, vram, &bmi, 0, SRCCOPY);
            MessageBoxA(0, "Better luck next time :(", "BOOM", MB_OK);
            exit(0);
        }
    }
        
}

void    handleRightMouse()
{
    int     x, y;

    getRelativeCursorPosition(&x, &y);
    if (info->visible[y][x] != EMPTY)
        info->visible[y][x] = info->visible[y][x] == HIDDEN ? FLAGGED : HIDDEN;
}

LRESULT CALLBACK windowMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        handleLeftMouse();
        break;
    case WM_RBUTTONDOWN:
        handleRightMouse();
        break;
    case WM_QUIT:
    case WM_CLOSE:
        ExitProcess(0);
    }
    return DefWindowProcA(window, msg, wParam, lParam);
}

void drawTexture(int x, int y, int size, pixel* texture, gameInfo* info)
{
    for (int y2 = 0; y2 < size; y2++)
    {
        memcpy(&vram[x + (y + y2) * info->x * BLOCK_SIZE], &texture[y2 * size], sizeof(pixel) * size);
    }
}

void    drawField(gameInfo* info, pixel** textures, bool override)
{
    
    for (int y = 0; y < info->y; y++)
    {
        for (int x = 0; x < info->x; x++)
        {
            if (override == false && info->visible[y][x] == HIDDEN)
                drawTexture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, textures[HIDDEN], info);
            else if (override == false && info->visible[y][x] == FLAGGED)
                drawTexture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, textures[FLAGGED], info);
            else
                drawTexture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, textures[info->field[y][x]], info);
        }
    }
}

void	renderVisual(void*  param)
{
    info = (gameInfo*)param;
    MSG         msg;
    while (1)
    {
        while (PeekMessageA(&msg, window, 0, 0, 0))
        {
            GetMessageA(&msg, window, 0, 0);
            DispatchMessageA(&msg);
        }
        memset(vram, 255, info->x * BLOCK_SIZE * info->y * BLOCK_SIZE * sizeof(pixel));
        drawField(info, info->textures, false);
        StretchDIBits(WindowDC, 0, 0, info->x * SCR_SIZE, info->y * SCR_SIZE , 0, 0, info->x * BLOCK_SIZE, info->y * BLOCK_SIZE, vram, &bmi, 0, SRCCOPY);
        if (finishedGame(info) == WON)
        {
            drawField(info, info->textures, true);
            StretchDIBits(WindowDC, 0, 0, info->x * SCR_SIZE, info->y * SCR_SIZE, 0, 0, info->x * BLOCK_SIZE, info->y * BLOCK_SIZE, vram, &bmi, 0, SRCCOPY);
            MessageBoxA(0, "You've won :)", "Congrats", MB_OK);
            exit(0);
        }
        Sleep(5);
    }
}

pixel* openFile(std::string fileName)
{
    //std::cout << fileName << "\n";
    HANDLE file = CreateFileA(fileName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //printf("handle: %i\n", file);
    char buffer[14];
    ReadFile(file, buffer, 14, 0, 0);
    UINT32  file_size = (UINT8)buffer[2] + (UINT8)buffer[3] * 256;
    //std::cout << "File size: " << file_size << "\n";
    UINT32  start_dest = buffer[10] + buffer[11] * 256;
    //printf("starting pos: %i\n", start_dest);
    SetFilePointer(file, start_dest, 0, 0);
    char* temp = new char[file_size];
    pixel* image = new pixel[(file_size - start_dest) / 3];
    ReadFile(file, temp, file_size, 0, 0);
    for (int i = 0; i < (file_size - start_dest) / 3; i++)
    {
        image[i].b = temp[i * 3 + 0];
        image[i].g = temp[i * 3 + 1];
        image[i].r = temp[i * 3 + 2];
    }
    CloseHandle(file);
    delete[] temp;
    return (image);
}

pixel** initTextures()
{
    pixel**  textures = new pixel*[TEXTURE_COUNT];

    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        textures[i] = openFile("HighRes/" + std::to_string(i) + ".bmp");
    }
    return textures;
}

void	initRender(gameInfo* info)
{
    WNDCLASSA wc{};

    wc.lpfnWndProc = windowMessageHandler;                                                                  // Pointer to the window procedure
    wc.hInstance = GetModuleHandle(NULL);                                                                   // Handle to the application instance
    wc.lpszClassName = "class";                                                                             // Name of the window class
    wc.hCursor = LoadCursor(NULL, IDC_HAND);                                                                // Default cursor
    wc.hIcon = (HICON)LoadImageA(NULL, "Flag.ico", IMAGE_ICON, 256, 256, LR_DEFAULTSIZE | LR_LOADFROMFILE); // Create Icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                                                          // Default background color
    wc.style = CS_HREDRAW | CS_VREDRAW;                                                                     // Window style (optional)

    // Register the window class
    RegisterClassA(&wc);
    vram = new pixel[info->x * BLOCK_SIZE * info->y * BLOCK_SIZE]{};
    window = CreateWindowExA(0, "class", "Minesweeper", WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_EX_CLIENTEDGE, 0, 0, info->x * SCR_SIZE, info->y * SCR_SIZE, 0, 0, wc.hInstance, 0);
    WindowDC = GetDC(window);

    bmi.bmiHeader.biWidth = info->x * BLOCK_SIZE;
    bmi.bmiHeader.biHeight = info->y * BLOCK_SIZE;
    bmi.bmiHeader.biBitCount = sizeof(pixel) * 8;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);

    RECT client;
    GetClientRect(window, &client);
    SetWindowPos(window, 0, 150, 50, info->x * SCR_SIZE + (info->x * SCR_SIZE - client.right), info->y * SCR_SIZE + (info->y * SCR_SIZE - client.bottom), 0);
}

void    renderMain()
{
    gameInfo    info = getUserInput();

    makeField(&info, -1, -1, true);
    //printField(&info, true);
    info.textures = initTextures();
    initRender(&info);
    renderVisual(&info);
}
