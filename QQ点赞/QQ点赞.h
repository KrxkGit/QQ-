#pragma once

#include "resource.h"
#include"SkinPPWTL.h"
#pragma comment(lib,"SkinPPWTL.lib")

INT_PTR CALLBACK MainDlg(HWND, UINT, WPARAM, LPARAM);
VOID OnInit(HWND hDlg);
UINT Praise(HWND); //点赞主函数
UINT CALLBACK Prepare(HWND hDlg); //准备点赞
VOID SetOrReadDataFromReg(HWND hDlg, BOOL bSet);//保存参数
VOID ShutDown();//关机