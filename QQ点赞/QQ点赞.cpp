// QQ����.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "QQ����.h"
#include"�ɷ�ϵͳ.h"

// ȫ�ֱ���: 
HINSTANCE hInst;// ��ǰʵ��
UINT nPraiseCount = 0;
TCHAR szGroup[MAX_PATH];//����Ⱥ��
DWORD dwPeriod = 0;//���ʱ��
DWORD dwPrepare = 0;//׼��ʱ��
HCURSOR hCursor = NULL;
HCURSOR hOldCursor = NULL;
HWND hProgress = NULL; //������
UINT nCount = 0;//������
HANDLE hThread = INVALID_HANDLE_VALUE;//�����߳̾��
BOOL bSpeed = FALSE;//����ģʽ

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  �ڴ˷��ô��롣
	skinppLoadSkin("SlickOS2.ssk");
	ExitIfHaveNotPayMoney()

	hInst = hInstance;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, MainDlg);
	skinppExitSkin();
	return GetLastError();
}

//BOOL CompareColor(HDC hdc,POINT pt)
//{
//	COLORREF col = GetPixel(hdc, pt.x, pt.y);
//	COLORREF colWhite = RGB(255, 255, 255);
//	if (memcmp(&col, &colWhite, sizeof(COLORREF) == 0)) {
//		return TRUE;
//	}
//	return FALSE;
//}

VOID SetOrReadDataFromReg(HWND hDlg, BOOL bSet)
{
	TCHAR g_sz[] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\QQ����");
	HKEY hKey;
	DWORD dwTemp;
	if (!bSet) {
		RegCreateKeyEx(HKEY_CURRENT_USER, g_sz, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, 0, &hKey, NULL);
		dwTemp = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);//���޺�����
		RegSetValueEx(hKey, _T("���޺�����"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
		dwTemp = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
		RegSetValueEx(hKey, _T("������"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
		dwTemp = Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK1));
		RegSetValueEx(hKey, _T("�Զ��ػ�"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
		dwTemp = Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK2));
		RegSetValueEx(hKey, _T("����ģʽ"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
		dwTemp = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
		RegSetValueEx(hKey, _T("���ʱ��"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
		dwTemp = GetDlgItemInt(hDlg, IDC_EDIT5, NULL, FALSE);
		RegSetValueEx(hKey, _T("׼��ʱ��"), NULL, REG_DWORD, (LPBYTE)&dwTemp, sizeof(dwTemp));
	}
	else {
		RegOpenKeyEx(HKEY_CURRENT_USER, g_sz, NULL, KEY_QUERY_VALUE, &hKey);
		DWORD dwType, size = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("���޺�����"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		SetDlgItemInt(hDlg, IDC_EDIT1, dwTemp, FALSE);
		RegQueryValueEx(hKey, _T("������"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		SetDlgItemInt(hDlg, IDC_EDIT2, dwTemp, FALSE);
		RegQueryValueEx(hKey, _T("�Զ��ػ�"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK1), dwTemp);
		RegQueryValueEx(hKey, _T("����ģʽ"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK2), dwTemp);
		RegQueryValueEx(hKey, _T("���ʱ��"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		SetDlgItemInt(hDlg, IDC_EDIT3, dwTemp, FALSE);
		RegQueryValueEx(hKey, _T("׼��ʱ��"), NULL, &dwType, (LPBYTE)&dwTemp, &size);
		SetDlgItemInt(hDlg, IDC_EDIT5, dwTemp, FALSE);
	}
	RegCloseKey(hKey);
}

VOID OnInit(HWND hDlg)
{
	SendMessage(hDlg, WM_SETICON, 0, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_QQ)));
	hProgress = GetDlgItem(hDlg, IDC_PROGRESS1);//������������
	SendMessage(hProgress, PBM_SETSTEP, 1, 0);
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR1));
	SetOrReadDataFromReg(hDlg, TRUE);//���ò���
}



BOOL CALLBACK OpenDataCard(HWND hWnd, LPARAM)
{
	if (!IsWindowVisible(hWnd)) {//���ڲ��ɼ�
		return TRUE;
	}
	TCHAR szClass[MAX_PATH], szName[MAX_PATH];
	GetClassName(hWnd, szClass, _countof(szClass));
	GetWindowText(hWnd, szName, _countof(szName));
	if (lstrcmp(_T("TXGuiFoundation"), szClass) != 0 ||
		szName[0] == '\0'/*�ǳƲ���Ϊ��*/ ||
		lstrcmp(szName, _T("QQ")) == 0 ||
		lstrcmp(szName, szGroup) == 0
		) {
		return TRUE;
	}
	else {
		RECT rcQQWnd;
		GetWindowRect(hWnd, &rcQQWnd);
		POINT pt = { (rcQQWnd.left + rcQQWnd.right) / 2-25, rcQQWnd.top + 25 };
		ScreenToClient(hWnd, &pt);
		LPARAM point = MAKELPARAM(pt.x,pt.y);
		SendMessage(hWnd, WM_MOUSEMOVE, 0, point);
		SendMessage(hWnd, WM_LBUTTONDOWN, 0, point);
		SendMessage(hWnd, WM_LBUTTONUP, 0, point);
	}
	PostMessage(hWnd, WM_CLOSE, 0, 0);

	SendMessage(hProgress, PBM_DELTAPOS, 1, 0);
	return TRUE;
}

VOID ChangeSpeed()
{
#define MAXFRIENDS 10
	static int count = MAXFRIENDS;
	if (count == MAXFRIENDS || count==0) {
		count = MAXFRIENDS;
		dwPeriod = 150;
	}
	else {
		dwPeriod -= 10;
	}
	count--;
#undef MAXFREENDS
}

BOOL CALLBACK StartToPraise(HWND hWnd, LPARAM)
{
	if (!IsWindowVisible(hWnd)) {//���ڲ��ɼ�
		return TRUE;
	}
	TCHAR szClass[MAX_PATH], szName[MAX_PATH];
	GetClassName(hWnd, szClass, _countof(szClass));
	GetWindowText(hWnd, szName, _countof(szName));
	if (lstrcmp(_T("TXGuiFoundation"), szClass) != 0 || szName[0] == '\0'/*�ǳƲ���Ϊ��*/) { //������
		return TRUE;
	}
	LPTSTR p = _tcschr(szName, '\0');
	p -= 3;
	if (lstrcmp(_T("������"), p) != 0) {
		return TRUE;
	}
	if (bSpeed) {
		ChangeSpeed();
	}
	LPARAM point = MAKELPARAM(332, 339);
	SendMessage(hWnd, WM_MOUSEMOVE, 0, point);
	for (UINT n = 0; n < nPraiseCount/*����n��*/; n++) {
		SendMessage(hWnd, WM_LBUTTONDOWN, 0, point);
		SendMessage(hWnd, WM_LBUTTONUP, 0, point);
		Sleep(dwPeriod);
	}
	PostMessage(hWnd, WM_CLOSE, 0, 0);

	SendMessage(hProgress, PBM_DELTAPOS, 1, 0);//���½�����
	return TRUE;
}

UINT CALLBACK Prepare(HWND hDlg) //׼������
{
#define MAXFRIENDS 10

	UINT nCount = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
	nPraiseCount = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);//��ȡ������
	if (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK2))) {
		Button_Enable(GetDlgItem(hDlg, IDC_EDIT3), FALSE);
		bSpeed = TRUE;
	}
	else {
		Button_Enable(GetDlgItem(hDlg, IDC_EDIT3), TRUE);
		dwPeriod = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
		bSpeed = FALSE;
	}
	dwPrepare = GetDlgItemInt(hDlg, IDC_EDIT5, NULL, FALSE);

	if (nCount == 0 || nPraiseCount == 0) {
		MessageBox(hDlg, _T("��Ч����"), _T("QQ����"), MB_ICONERROR);
		return FALSE;
	}
	Button_Enable(GetDlgItem(hDlg, IDOK), FALSE);
	Edit_Enable(GetDlgItem(hDlg, IDC_EDIT4), FALSE);

	//������
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, nCount * 3));
	SendMessage(hProgress, PBM_SETPOS, 0, 0);
	//�ָ�����
	int temp = nCount%MAXFRIENDS;
	BOOL bContinued = TRUE;
	for (; bContinued;) {
		if (nCount >= MAXFRIENDS) {
			::nCount = MAXFRIENDS;
			nCount -= MAXFRIENDS;
		}
		else {
			::nCount = temp;
			bContinued = FALSE;
		}
		Praise(hDlg);
	}
	SendMessage(hProgress, PBM_SETPOS, SendMessage(hProgress, PBM_GETRANGE, FALSE, 0), 0);

	if (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK1))) {
		SetOrReadDataFromReg(hDlg, FALSE);//�������
		ShutDown();//�ػ�
	}
	MessageBox(hDlg, _T("���"), _T("QQ����"), MB_ICONINFORMATION);
	Edit_Enable(GetDlgItem(hDlg, IDC_EDIT4), TRUE);
	Button_Enable(GetDlgItem(hDlg, IDOK), TRUE);
	Button_Enable(GetDlgItem(hDlg, IDC_BUTTON1), FALSE);
	return TRUE;

#undef MAXFRIENDS
}

UINT Praise(HWND hDlg) //���޺���
{
#define SupplementCount 10

	HWND hQQ = NULL;
	GetDlgItemText(hDlg, IDC_EDIT4, szGroup, _countof(szGroup));
	if (szGroup[0] == '\0') {
		hQQ = FindWindow(_T("TXGuiFoundation"), _T("QQ"));
	}
	else {
		hQQ = FindWindow(_T("TXGuiFoundation"), szGroup);
	}

	static UINT temp = 0;

	for (UINT n = 0; n < nCount; n++) {
		SendMessage(hQQ, WM_KEYDOWN, VK_RETURN, 0);
		SendMessage(hQQ, WM_KEYDOWN, VK_DOWN, 0);
		SendMessage(hProgress, PBM_DELTAPOS, 1, 0);//���½�����
	}

	EnumWindows(OpenDataCard, NULL);//�����Ͽ�
	Sleep(dwPrepare);//�ȴ�
	EnumWindows(StartToPraise, NULL);//����

	
	return TRUE;
#undef SupplementCount
}

VOID OnMouseMove(HWND hDlg)
{
	if (GetCapture() == NULL) {
		return;
	}
	if (!IsWindowEnabled(GetDlgItem(hDlg, IDC_EDIT4))) {
		return;
	}
	POINT pt;
	GetCursorPos(&pt);
	HWND hWnd = WindowFromPoint(pt);
	static TCHAR sz[MAX_PATH];
	GetWindowText(hWnd, sz, _countof(sz));
	SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), sz);
}

VOID ShutDown()//�ػ�
{
	HANDLE hToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	LUID uID = { 0 };
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &uID);
	TOKEN_PRIVILEGES tp = { 0 };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = uID;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hToken);

	ExitWindowsEx(EWX_SHUTDOWN, ERROR_SUCCESS);
}

VOID OnOk(BOOL& bStaus,HWND hDlg)//OK������Ӧ
{
	hThread = (HANDLE)_beginthreadex(0, 0, (UINT(CALLBACK*)(PVOID))Prepare, hDlg, 0, 0);
	bStaus = TRUE;
	Button_Enable(GetDlgItem(hDlg, IDC_BUTTON1), TRUE);
}

INT_PTR CALLBACK MainDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bStaus = TRUE;//TrueΪ��ʼ��FalseΪ��ͣ
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		OnInit(hDlg);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(hDlg);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hDlg);
		hOldCursor = SetCursor(hCursor);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		SetCursor(hOldCursor);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			SetOrReadDataFromReg(hDlg, FALSE);//�������
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDOK:
			OnOk(bStaus, hDlg);
			break;
		case IDC_BUTTON1:
			if (bStaus) {
				SuspendThread(hThread);
				SetDlgItemText(hDlg, IDC_BUTTON1, _T("�ָ�"));
				bStaus = FALSE;
			}
			else {
				ResumeThread(hThread);
				SetDlgItemText(hDlg, IDC_BUTTON1, _T("��ͣ"));
				bStaus = TRUE;
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
