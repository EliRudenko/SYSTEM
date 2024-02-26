#pragma once
#include "header.h"
#include <fstream>
#include <sstream>

class CodingThreadDlg
{
private:
    void Cls_OnButton1();
public:
    CodingThreadDlg(void);
public:
    ~CodingThreadDlg(void);
    static BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
    static CodingThreadDlg* ptr;
    void Cls_OnClose(HWND hwnd);
    BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void UpdateEditControl(const std::wstring& text);
    HWND hDialog;
};