#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <Commctrl.h>
#include "resource.h"
#include "SkinH.h"
#pragma comment(lib, "SkinHu.lib")
#pragma comment(lib, "WINMM.LIB")

#define TIMEID 99
#define MENUADD 101
#define MENUDEL 102
#define MENUCLEAR 103

TCHAR AppName[10] = L"MusicList";
TCHAR iniPath[20] = L"D://musiclist.ini";
TCHAR CurrentMusic[MAX_PATH];
HMENU hmenu;//�����ݲ˵�
HBITMAP hBmp;//Listλͼ
int CurrentMusicLen;

//��Ϣ����
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Cls_OnClose(HWND hwnd);

void Start(HWND hwnd); //�򿪲��Ÿ���
void Pause(HWND hwnd); //��ͣ/��������

void PlayMusic(TCHAR *MusicPath);//��ȡ·��
void StopMusic(TCHAR* MusicPath);//��ͣ�رո���
void PauseMusic(TCHAR* MusicPath);//��ͣ
void ResumeMusic(TCHAR* MusicPath);//����
int GetMusicLength(TCHAR* MusicPath);//��ȡ��������
int GetMusicPos(TCHAR* MusicPath);//�õ���ǰ�����Ľ���
void FromMusicPos(TCHAR* MusicPath, int pos);//��ĳ��λ�ÿ�ʼ����
void Add(HWND hwnd);//���б������
void Del(HWND hwnd);//���б���ɾ��
void Clear(HWND hwnd);//����б�