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
	TCHAR className[] = "��� �����";
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
		MessageBox(NULL, "�� ���������� ���������������� �����!", "������", MB_OK);
		return NULL;
	}

	hWindow = CreateWindow(className,
		"��������� ���������� � ������������� ������",
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

	AppendMenu(hmenu1, MF_STRING | MF_POPUP, (UINT)hPopupmenu1, "&����");
	{
		AppendMenu(hPopupmenu1, MF_STRING, 1111, TEXT("��������� ������������� �����"));
		AppendMenu(hPopupmenu1, MF_STRING, 1112, "������� �������������� �����");
		AppendMenu(hPopupmenu1, MF_STRING, 1113, "�����");
	}
	AppendMenu(hmenu1, MF_STRING, 1114, "&������");


	SetMenu(hWindow, hmenu1);
	if (!hWindow) {
		MessageBox(NULL, "�� ���������� ������� ����!", "������", MB_OK);
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
		
		// ����������� ����� ������ �� shift ���
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
			ofn.hwndOwner = hWindow; // ������������ ���� 
			ofn.lpstrFilter = "����������� ����� (*.shf)\0*.shf\0��� �����\0*.*\0"; // ����� (������) ��� ������������ ������
			ofn.lpstrFile = NameFile;  // ���� ����� �������� ������ ��� �����
			ofn.nMaxFile = 255;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = NULL;
			ofn.lpstrDefExt = "shf"; // ������ ���������� �� ���������
			if (GetSaveFileName(&ofn)) // ���������� ���� "���������", ������� ���������� �������, ���� ������ ������ Ok
			{
				
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, MyDialog) == IDOK)
				{
					ofstream fout("C:\\cpp\\baumanclasses\\hw\\hw2\\hw2.shf", ios_base::app);
					if (!fout.is_open())
					{
						MessageBox(hWindow, "������ �������� �����.", "��������� �� ������", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					else
					{ 
					
						fout << scrambler(size, key, text);
					}
				fout.close();
				fflush(stdin);
				MessageBox(hWindow, "������ ������� ���������.", "���������", MB_OK | MB_ICONINFORMATION);
				InvalidateRect(hWindow, 0, TRUE);
				}
				else MessageBox(hWindow, "������ �� ���������. ���� �� ������!!", "���������", MB_OK | MB_ICONHAND);
			}
			break;
		
		case 1112:
			
			ZeroMemory(&ofn, sizeof(ofn));
			hDeviceContext = BeginPaint(hWindow, &paintStruct);
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWindow;
			ofn.lpstrFile = NameFile; // ���� ����� �������� ��� �����
									  // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
									  // use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = "����������� ����� (*.shf)\0*.shf\0��� �����\0*.*\0"; // ������ ��� ����������� ������
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = "shf"; // ������ ���������� �� ���������

			if (GetOpenFileName(&ofn)) // ���������� ���� "�������", ������� ���������� �������, ���� ������ ������ �������
			{
				// ���������� ���� ��� ����� �����
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, MyDialog) == IDOK)
				{
					setlocale(LC_CTYPE, "rus");
					ifstream fin("C:\\cpp\\baumanclasses\\hw\\hw2\\hw2.shf");
					if (!fin.is_open())
					{
						MessageBox(hWindow, "������ �������� �����.", "��������� �� ������", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					else
					{
							TextOut(hDeviceContext, 10, 10, transcript(size, key, text), strlen(text));
					}
					fin.close();
					fflush(stdin);
					InvalidateRect(hWindow, 0, TRUE); // ����������� ����
				}
				else MessageBox(hWindow, "������ �� ���������. ���� �� ������!!", "���������", MB_OK | MB_ICONHAND);
			}

			break;
		
		case 1113:
			DestroyWindow(hWindow);
			break;
		case 1114:
			MessageBox(hWindow, "������ ��������� ������������� ��� ���������� � ������������� ������. \n ���������� ������ ���������, ��������� ��� ������������� ����� � ������ ������. \n ��� ����������� ��������� ���������� ������� ������������� ���� .shf � ������ ������. \n ��������� �����������!", "���������", MB_OK | MB_ICONINFORMATION);
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
			if (LOWORD(wParameter) == IDOK) // ������ ��������� ���� �� ���������� ����
			{
				char Str[32];
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, 31, (LPARAM)Str); // ��������� ����� �� ���������� ���� � �������� � ������ Str
																  // ������ ������ �� ������
				if (sscanf_s(Str, "%d", &key) < 1) // ����� �������� �� ��������� �� ������
				{
					MessageBox(hDlg, "�������� ������ �����. ���� ������ ���� ����� ������", "��������� � �������� ������� �����", MB_OK | MB_ICONHAND);
					return (INT_PTR)TRUE; // ����� ��� �������� ����������� ����

				}

			}
			EndDialog(hDlg, LOWORD(wParameter)); // �������� ����������� ����
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
