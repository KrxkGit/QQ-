#pragma once

#include "resource.h"
#include"SkinPPWTL.h"
#pragma comment(lib,"SkinPPWTL.lib")

INT_PTR CALLBACK MainDlg(HWND, UINT, WPARAM, LPARAM);
VOID OnInit(HWND hDlg);
UINT Praise(HWND); //����������
UINT CALLBACK Prepare(HWND hDlg); //׼������
VOID SetOrReadDataFromReg(HWND hDlg, BOOL bSet);//�������
VOID ShutDown();//�ػ�