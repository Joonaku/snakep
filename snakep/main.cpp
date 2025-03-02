#include <windows.h>
#include <wchar.h>
#include <iostream>
#include <stdlib.h>
#include "resource.h"
#include <string>

#define DIRECTION_LEFT	0
#define DIRECTION_RIGHT 1
#define DIRECTION_UP	2
#define DIRECTION_DOWN  3

const wchar_t g_szClassName[] = L"wc";
int snakeSize = 2;
int snakeDirection = DIRECTION_RIGHT;

typedef struct _SNAKEINFO
{
	int x = 0;
	int y = 0;
	int width = x + 20;
	int height = y + 20;

	int direction = DIRECTION_RIGHT;
}SNAKEINFO;
SNAKEINFO g_snakeInfo[50];

typedef struct _APPLEINFO
{
	int x;
	int y;
	int width;
	int height;
}APPLEINFO;
APPLEINFO g_appleInfo;

void GenerateApple()
{
	int min = 0, max = 24;
	g_appleInfo.x = ((min + rand() % (max - min + 1)) * 20);
	g_appleInfo.y = ((min + rand() % (max - min + 1)) * 20);
	g_appleInfo.width = g_appleInfo.x + 20;
	g_appleInfo.height = g_appleInfo.y + 20;
}

// Update snake position according to direction and test for collision
void UpdateSnake(HWND hwnd)
{
	for (int i = snakeSize; i > 0; i--)
	{
		g_snakeInfo[i].x = g_snakeInfo[i - 1].x;
		g_snakeInfo[i].y = g_snakeInfo[i - 1].y;


		// TODO: Fix snake collision checking
		/*
		if ((g_snakeInfo[i].x == g_snakeInfo[0].x) && (g_snakeInfo[i].y == g_snakeInfo[0].y))
		{
			MessageBox(hwnd, L"Game Over", L"s", MB_OK);

			return;
		}*/
	}

	switch (snakeDirection)
	{
	case DIRECTION_RIGHT:
		g_snakeInfo[0].x += 20;
		break;
	case DIRECTION_LEFT:
		g_snakeInfo[0].x -= 20;
		break;
	case DIRECTION_UP:
		g_snakeInfo[0].y -= 20;
		break;
	case DIRECTION_DOWN:
		g_snakeInfo[0].y += 20;
		break;
	}
	InvalidateRect(hwnd, NULL, TRUE);

	if (g_snakeInfo[0].x == g_appleInfo.x && g_snakeInfo[0].y == g_appleInfo.y)
	{
		GenerateApple();
		snakeSize++;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		GenerateApple();
		SetTimer(hwnd, 1, 200, NULL);
		break;
	case WM_PAINT:
	{
		int prevX, prevY;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT snakeHead;
		RECT apple = { g_appleInfo.x, g_appleInfo.y, g_appleInfo.width, g_appleInfo.height };
		HBRUSH brush = CreateSolidBrush(RGB(0, 100, 0));

		for (int i = 0; i < snakeSize; i++)
		{
			snakeHead = { g_snakeInfo[i].x, g_snakeInfo[i].y, g_snakeInfo[i].x + 20, g_snakeInfo[i].y + 20 };
			FillRect(hdc, &snakeHead, brush);
		}

		brush = CreateSolidBrush(RGB(255, 0, 0));
		FillRect(hdc, &apple, brush);
		EndPaint(hwnd, &ps);

		break;
	}
	break;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT && snakeDirection != DIRECTION_LEFT)
		{
			snakeDirection = DIRECTION_RIGHT;
		}
		else if (wParam == VK_LEFT && snakeDirection != DIRECTION_RIGHT)
		{
			snakeDirection = DIRECTION_LEFT;
		}
		else if (wParam == VK_DOWN && snakeDirection != DIRECTION_UP)
		{
			snakeDirection = DIRECTION_DOWN;
		}
		else if (wParam == VK_UP && snakeDirection != DIRECTION_DOWN)
		{
			snakeDirection = DIRECTION_UP;
		}
		break;
	case WM_TIMER:
	{
		UpdateSnake(hwnd);
		std::wstring snakeSizeStr = std::to_wstring(snakeSize);
		CreateWindow(L"Edit", snakeSizeStr.c_str(), WS_VISIBLE | WS_CHILD, 200, 200, 20, 20, hwnd, NULL, NULL, NULL);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned)time(NULL));
	WNDCLASSEX wc;
	HWND hwnd;
	HWND childWindow;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		L"Snek",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 550, 550,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}