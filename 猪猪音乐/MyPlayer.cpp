#include "MyPlayer.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HICON hIcon;
	TCHAR MusicPos[20];
	HDC HDCMem;
	RECT ListBox;
	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, Cls_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, Cls_OnCommand);
		HANDLE_MSG(hDlg, WM_CLOSE, Cls_OnClose);
		case WM_TIMER:
		{
			int pos = GetMusicPos(CurrentMusic) / 1000 ;
			wsprintf(MusicPos, L"%02d:%02d", pos / 60, pos % 60);
			SetWindowText(GetDlgItem(hDlg, IDC_TIME), MusicPos);
			SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPOS, TRUE, pos);
			if (pos == CurrentMusicLen)
			{
				KillTimer(hDlg, TIMEID);
				lstrcpy(CurrentMusic, L"");
			}
		}
		break;
		case WM_HSCROLL:
		{
			StopMusic(CurrentMusic);
			int pos = SendMessage(GetDlgItem(hDlg,IDC_SLIDER1),TBM_GETPOS,0,0);
			FromMusicPos(CurrentMusic, pos * 1000);
		}
		break;
		case WM_CONTEXTMENU:
		{
			if ((HWND)wParam == GetDlgItem(hDlg, IDC_LIST))
			{
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				TrackPopupMenuEx(hmenu , TPM_LEFTALIGN | TPM_TOPALIGN, xPos, yPos, hDlg, NULL);
			}
		}
		break;
		case WM_CTLCOLORLISTBOX:		{			GetClientRect((HWND)lParam,&ListBox);			HDCMem = CreateCompatibleDC((HDC)wParam);			SelectObject(HDCMem, hBmp);			BitBlt((HDC)wParam, 0, 0, ListBox.right, ListBox.bottom, HDCMem, 0, 0, SRCCOPY);			DeleteDC(HDCMem);			SetTextColor((HDC)wParam, RGB(135, 206, 250));//�����ı���ɫ			SetBkMode((HDC)wParam, TRANSPARENT);			return (BOOL)((HBRUSH)GetStockObject(NULL_BRUSH));		}		break;
	}
	return FALSE;
}

BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SkinH_Attach();	SkinH_DetachEx(GetDlgItem(hwnd, IDC_LIST));//ж��Ƥ�������List��UI	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);	HBITMAP hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(TDB_BITMAP1));
	
	//����ͼ��
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	SetClassLong(hwnd, GCL_HICON, (long)hIcon);  //SetWindowLong  (���໯���ڣ�

	//������ݲ˵�
	hmenu = CreatePopupMenu();
	AppendMenu(hmenu, MF_STRING, MENUADD, L"����Ҫ����Լ�ϲ���ĸ�");
	AppendMenu(hmenu, MF_STRING, MENUDEL, L"���׸����������Ҫɾ��");
	AppendMenu(hmenu, MF_STRING, MENUCLEAR, L"ɵ����Ҫ����Լ����б�");

	TCHAR musicNames[MAX_PATH] = L"";
	GetPrivateProfileString(AppName, NULL, NULL, musicNames, MAX_PATH, iniPath);
	TCHAR *p = musicNames;
	while (*p)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_INSERTSTRING, -1, (LPARAM)p);
		p = p + lstrlen(p) + 1;
	}

	return TRUE;
}

void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{ 
		case IDC_LIST:
		{
			if (LBN_DBLCLK == codeNotify)
			{
				Start(hwnd);
			}
		}
		break;
	    case IDSTART1:
		{
			Start(hwnd);
		}
		break;
		case IDPAUSE:
		{
			Pause(hwnd);
		}
		break;
		case MENUADD:
		{
			Add(hwnd);
		}
		break;
		case MENUDEL:
		{
			Del(hwnd);
		}
		break;
		case MENUCLEAR:
		{
			Clear(hwnd);
		}
		break;
	}
}

void Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

void Start(HWND hwnd)
{
	TCHAR MusicLongPath[MAX_PATH] = L"";
	TCHAR MusicShortPath[MAX_PATH] = L"";
	TCHAR ButtonText[20] = L"";
	TCHAR MusicLen[20] = L"";
	TCHAR MusicName[MAX_PATH] = L"";
	int ret = SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETCURSEL, 0, 0);//�õ���ǰ��������
	Button_GetText(GetDlgItem(hwnd, IDPAUSE), ButtonText, 20);
	if (LB_ERR != ret)//��������ֵ�ǳ���LB_ERR������û��ָ���κ�λ��
	{
		if (!lstrcmp(ButtonText, L"�����������"))
		{
			Button_SetText(GetDlgItem(hwnd, IDPAUSE), L"�����������ͣ����");
		}
		if (lstrlen(CurrentMusic) != 0)
		{
			StopMusic(CurrentMusic);
		}
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETTEXT, ret, (LPARAM)MusicName);
		GetPrivateProfileString(AppName, MusicName, NULL, MusicLongPath, MAX_PATH, iniPath);
		GetShortPathName(MusicLongPath, MusicShortPath, MAX_PATH);
		PlayMusic(MusicShortPath);
		lstrcpy(CurrentMusic, MusicShortPath);
		//��ȡ����
		CurrentMusicLen = GetMusicLength(CurrentMusic) / 1000;//������ת��Ϊ��
		wsprintf(MusicLen, L"%02d:%02d", CurrentMusicLen / 60, CurrentMusicLen % 60);
		SetWindowText(GetDlgItem(hwnd, IDC_LENGTH), MusicLen);
		//���ý�����
		SendMessage(GetDlgItem(hwnd, IDC_SLIDER1), TBM_SETRANGE, TRUE, MAKELONG(0, CurrentMusicLen));
		//���붨ʱ��
		SetTimer(hwnd, TIMEID, 500, NULL);
	}
	else
	{
		MessageBox(hwnd, L"��ѡ�����,�������ţ�", L"ɵ��", MB_OK);
	}
}

void Pause(HWND hwnd)
{
	TCHAR ButtonText[20] = L"";
	Button_GetText(GetDlgItem(hwnd, IDPAUSE), ButtonText, 20);
	if (!lstrcmp(ButtonText, L"�����������ͣ����"))
	{
		PauseMusic(CurrentMusic);
		Button_SetText(GetDlgItem(hwnd, IDPAUSE), L"�����������");
	}
	else if (!lstrcmp(ButtonText, L"�����������"))
	{
		ResumeMusic(CurrentMusic);
		Button_SetText(GetDlgItem(hwnd, IDPAUSE), L"�����������ͣ����");
	}
}

void PlayMusic(TCHAR* MusicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"open %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);
	wsprintf(order, L"play %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);
}

void StopMusic(TCHAR* MusicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"stop %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);
	wsprintf(order, L"close %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);

}

void PauseMusic(TCHAR* MusicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"pause %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);
}

void ResumeMusic(TCHAR* MusicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"resume %s", MusicPath);
	mciSendString(order, NULL, 0, NULL);
}

int GetMusicLength(TCHAR* MusicPath)//���ص�ʱ�䵥λ�Ǻ���
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR Len[10] = L"";
	wsprintf(order, L"status %s length", MusicPath);
	mciSendString(order, Len, sizeof(Len), NULL);
	return _wtoi(Len);
}

int GetMusicPos(TCHAR* MusicPath)
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR Len[10] = L"";
	wsprintf(order, L"status %s position", MusicPath);
	mciSendString(order, Len, sizeof(Len), NULL);
	return _wtoi(Len);
}

void FromMusicPos(TCHAR* MusicPath ,int pos)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"play %s from %d", MusicPath, pos);
	mciSendString(order, NULL, 0, NULL);
}

void Add(HWND hwnd)
{
	OPENFILENAME ofn;
	TCHAR szFileName[MAX_PATH * 30];
	TCHAR MusicDir[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(szFileName, sizeof(szFileName));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;//�����ھ��  
	ofn.lpstrFilter = TEXT("MP3�ļ� (*.mp3)\0*.mp3\0");//������Դ򿪸�ʽ
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
	ofn.lpstrInitialDir = TEXT(".\\");//Ĭ�ϵĴ򿪵��ļ�·���������Ե�ǰĿ¼Ϊ��  

	TCHAR *p = NULL;
	if (GetOpenFileName(&ofn))
	{
		
		lstrcpy(MusicDir, szFileName);
		lstrcat(MusicDir, L"\\");
		
		p = szFileName + ofn.nFileOffset;
		while (*p)
		{
		    TCHAR MusicPath[MAX_PATH] = L"";
			lstrcpy(MusicPath, MusicDir);
			lstrcat(MusicPath, p);

			TCHAR MusicName[MAX_PATH] = L"";
			lstrcpy(MusicName, p);
			SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_INSERTSTRING, -1, (LPARAM)MusicName);
			p = p + lstrlen(p) + 1;

			WritePrivateProfileString(AppName, MusicName, MusicPath, iniPath);
		}
	}
}

void Del(HWND hwnd)
{
	TCHAR musicName[40] = L"";
	int sel = SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETCURSEL, 0, 0);
	if (sel > -1)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETTEXT, (WPARAM)sel, (LPARAM)musicName);
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_DELETESTRING, sel, 0);
		WritePrivateProfileString(AppName, musicName, NULL, iniPath);
	}
	else
	{
		MessageBox(hwnd, L"��ѡ��Ҫɾ���ĸ���", L"", MB_OK);
	}
}

void Clear(HWND hwnd)
{

	SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_RESETCONTENT, 0, 0);
	WritePrivateProfileString(AppName, NULL, NULL, iniPath);

}