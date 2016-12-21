#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <commdlg.h>
#include <stdlib.h>
#include <fstream>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Basetsd.h>
#include <cstdlib>
#include "resource.h"
#define shift 4
#define IDD_DIALOG1 101
#define IDC_EDIT1   1000

using namespace std;

LRESULT CALLBACK WindowProcess(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInstance;
INT_PTR CALLBACK MyDialog(HWND hDlg, UINT uMessage, WPARAM wParameter, LPARAM lParameter);

int WINAPI WinMain(HINSTANCE hInst,
	HINSTANCE hPrevInst,
	LPSTR pCommandLine,
	int nCommandShow)
{
	TCHAR className[] = "Мой класс";
	HWND hWindow;
	MSG message;
	WNDCLASSEX windowClass;

	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = className;
	windowClass.cbWndExtra = NULL;
	windowClass.cbClsExtra = NULL;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hInstance = hInst;

	if (!RegisterClassEx(&windowClass))
	{
		MessageBox(NULL, "Не получилось зарегистрировать класс!", "Ошибка", MB_OK);
		return NULL;
	}

	hWindow = CreateWindow(className,
		"Программа шифрования и расшифрования данных",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		NULL,
		CW_USEDEFAULT,
		NULL,
		(HWND)NULL,
		NULL,
		HINSTANCE(hInst),
		NULL
	);
	HMENU hmenu1 = CreateMenu();
	HMENU hPopupmenu1 = CreatePopupMenu();

	AppendMenu(hmenu1, MF_STRING | MF_POPUP, (UINT)hPopupmenu1, "&Файл");
	{
		AppendMenu(hPopupmenu1, MF_STRING, 1111, TEXT("Сохранить зашифрованный текст"));
		AppendMenu(hPopupmenu1, MF_STRING, 1112, "Открыть расшифрованный текст");
		AppendMenu(hPopupmenu1, MF_STRING, 1113, "Выход");
	}
	AppendMenu(hmenu1, MF_STRING, 1114, "&Помощь");


	SetMenu(hWindow, hmenu1);
	if (!hWindow) {
		MessageBox(NULL, "Не получилось создать окно!", "Ошибка", MB_OK);
		return NULL;
	}
	ShowWindow(hWindow, nCommandShow);
	UpdateWindow(hWindow);
	while (GetMessage(&message, NULL, NULL, NULL)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return message.wParam;
}

PTCHAR scrambler(int size, int key, PTCHAR text)
{
	double* gamma = 0;
	int* x = 0;
	int* y = 0;

	gamma = (double *)malloc(size * sizeof(double));
	srand(key); 
	for (int i = 0; i < size; i++) gamma[i] = rand() % 127; 

	
	x = (int *)malloc(size * sizeof(int));
	y = (int *)malloc(size * sizeof(int));
	

		//XOR
		for (int i = 0; i < size; i++)
		{
			x[i] = (int)((int)text[i] + gamma[i]) % 127;
			if (x[i] == 0) x[i] = 127;
		}
		
		// Циклический сдвиг вправо на shift бит
		for (int j = 0; j < shift; j++)
		{
			for (int i = 0; i < size; i++) y[i] = x[i];
			x[0] = y[size - 1];
			for (int i = 0; i < size - 1; i++) x[i + 1] = y[i];
		}
			for (int i = 0; i < size; i++) text[i] = (char)x[i];
			
			return text;
}
PTCHAR transcript(int size, int key, PTCHAR text)
{
	double * gamma = 0;
	int* x = 0;
	int* y = 0;
	gamma = (double *)malloc(size * sizeof(double));
	
	srand(key);
	for (int i = 0; i < size; i++) gamma[i] = rand() % 127;
	//for (int i = 0; i < size; i++) gamma[i] = a[i];

	x = (int *)malloc(size * sizeof(int));
	y = (int *)malloc(size * sizeof(int));

	for (int i = 0; i < size; i++) x[i] = (int)text[i];
	
	for (int j = 0; j < shift; j++)
	{
		for (int i = 0; i < size; i++) y[i] = x[i];
		x[size - 1] = y[0];
		for (int i = 0; i < size - 1; i++) x[i] = y[i + 1];
	}

	for (int i = 0; i < size; i++)
	{
		y[i] = (int)(x[i] - gamma[i] + 127) % 127;
		if (y[i] == 0) y[i] = 127;
	}
	
	for (int i = 0; i < size; i++) text[i] = (char)y[i];


	return text;
}


OPENFILENAME ofn;
TCHAR NameFile[256] = "hw2.shf";
int key;

LRESULT CALLBACK WindowProcess(HWND hWindow, UINT uMessage,
	WPARAM wParameter, LPARAM lParameter)
{
	HDC hDeviceContext;
	PAINTSTRUCT paintStruct;
	RECT rectPlace;
	HFONT hFont;

	static PTCHAR text;
	static int size = 0;
	double* a = (double*)malloc(sizeof(double));

	switch (uMessage)
	{
	case WM_CREATE:
		text = (PTCHAR)GlobalAlloc(GPTR, 50000 * sizeof(TCHAR));
		break;
	case WM_PAINT:
		text[size] = wParameter;
		hDeviceContext = BeginPaint(hWindow, &paintStruct);
		GetClientRect(hWindow, &rectPlace);
		SetTextColor(hDeviceContext, NULL);
		hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, VARIABLE_PITCH,
			"Arial Bold");
		SelectObject(hDeviceContext, hFont);
		if (wParameter != VK_RETURN)
			DrawText(hDeviceContext,
			(LPCSTR)text,
				size, &rectPlace,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hWindow, &paintStruct);
		break;
	case WM_CHAR:
		switch (wParameter)
		{
		case VK_RETURN:
			size = 0;
			break;
		default:
			text[size] = (char)wParameter;
			size++;
			break;
		}
		InvalidateRect(hWindow, NULL, TRUE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParameter))
		{
		case 1111:
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWindow; // родительское окно 
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0"; // Маска (фильтр) для отображаемых файлов
			ofn.lpstrFile = NameFile;  // Сюда будет записано полное имя файла
			ofn.nMaxFile = 255;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = NULL;
			ofn.lpstrDefExt = "shf"; // Строка расширение по умолчанию
			if (GetSaveFileName(&ofn)) // Появляется окно "Сохранить", функция возвращает истинно, если нажата кнопка Ok
			{
				
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, MyDialog) == IDOK)
				{
					ofstream fout("C:\\cpp\\baumanclasses\\hw\\hw2\\hw2.shf", ios_base::app);
					if (!fout.is_open())
					{
						MessageBox(hWindow, "Ошибка открытия файла.", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					else
					{ 
					
						fout << scrambler(size, key, text);
					}
				fout.close();
				fflush(stdin);
				MessageBox(hWindow, "Данные успешно сохранены.", "Сообщение", MB_OK | MB_ICONINFORMATION);
				InvalidateRect(hWindow, 0, TRUE);
				}
				else MessageBox(hWindow, "Данные не сохранены. Ключ не введен!!", "Сообщение", MB_OK | MB_ICONHAND);
			}
			break;
		
		case 1112:
			
			ZeroMemory(&ofn, sizeof(ofn));
			hDeviceContext = BeginPaint(hWindow, &paintStruct);
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWindow;
			ofn.lpstrFile = NameFile; // Сюда будет записано имя файла
									  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
									  // use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0"; // Фильтр для отображения файлов
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = "shf"; // Строка расширение по умолчанию

			if (GetOpenFileName(&ofn)) // Появляется окно "Открыть", функция возвращает истинно, если нажата кнопка Открыть
			{
				// Отображаем окно для ввода ключа
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, MyDialog) == IDOK)
				{
					setlocale(LC_CTYPE, "rus");
					ifstream fin("C:\\cpp\\baumanclasses\\hw\\hw2\\hw2.shf");
					if (!fin.is_open())
					{
						MessageBox(hWindow, "Ошибка открытия файла.", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					else
					{
							TextOut(hDeviceContext, 10, 10, transcript(size, key, text), strlen(text));
					}
					fin.close();
					fflush(stdin);
					InvalidateRect(hWindow, 0, TRUE); // Перерисовка окна
				}
				else MessageBox(hWindow, "Данные не прочитаны. Ключ не введен!!", "Сообщение", MB_OK | MB_ICONHAND);
			}

			break;
		
		case 1113:
			DestroyWindow(hWindow);
			break;
		case 1114:
			MessageBox(hWindow, "Данная программа предназначена для шифрования и расшифрования данных. \n Необходимо ввести сообщение, сохранить его зашифрованную копию с вводом пароля. \n Для расшифровки сообщения необходимо открыть зашифрованный файл .shf и ввести пароль. \n Приятного пользования!", "Сообщение", MB_OK | MB_ICONINFORMATION);
			break;
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(NULL);
		GlobalFree((HGLOBAL)text);
		break;
	default:
		return DefWindowProc(hWindow, uMessage, wParameter, lParameter);
	}
	
	return NULL;
}
INT_PTR CALLBACK MyDialog(HWND hDlg, UINT uMessage, WPARAM wParameter, LPARAM lParameter)
{
	UNREFERENCED_PARAMETER(lParameter);
	switch (uMessage)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParameter) == IDOK || LOWORD(wParameter) == IDCANCEL)
		{
			if (LOWORD(wParameter) == IDOK) // Читаем секретный ключ из текстового поля
			{
				char Str[32];
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, 31, (LPARAM)Str); // Прочитали текст из текстового поля и записали в струку Str
																  // Читаем данные из строки
				if (sscanf_s(Str, "%d", &key) < 1) // Целое значение не прочитано из строки
				{
					MessageBox(hDlg, "Неверный формат ключа. Ключ должен быть целым числом", "Сообщение о неверном формате ключа", MB_OK | MB_ICONHAND);
					return (INT_PTR)TRUE; // Выход без закрытия диалогового окна

				}

			}
			EndDialog(hDlg, LOWORD(wParameter)); // Закрытие диалогового окна
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
