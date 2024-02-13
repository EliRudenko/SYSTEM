#include <Windows.h>
#include "resource.h"
#include <string>
#include <vector>

using namespace std;

class Calculator 
{
public:
    Calculator(HINSTANCE hInstance) : hInstance(hInstance) {}

    INT_PTR ShowDialog() 
    {
        return DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, &Calculator::DlgProc, reinterpret_cast<LPARAM>(this));
    }

private:
    HINSTANCE hInstance;
    HWND hEdit1, hEdit2, hCheckBoxAdd, hCheckBoxSubtract, hCheckBoxMultiply, hCheckBoxDivide;
    HWND hButtonCalculate, hStaticResult;

    static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
    {
        Calculator* calculator = nullptr;

        if (uMsg == WM_INITDIALOG) 
        {
            calculator = reinterpret_cast<Calculator*>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(calculator));
            calculator->OnInitDialog(hwnd);
        }
        else 
        {
            calculator = reinterpret_cast<Calculator*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (calculator) 
        {
            return calculator->HandleMessage(hwnd, uMsg, wParam, lParam);
        }

        return FALSE;
    }

    void OnInitDialog(HWND hwnd) 
    {
        hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
        hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);
        hCheckBoxAdd = GetDlgItem(hwnd, IDC_CHECK_ADD);
        hCheckBoxSubtract = GetDlgItem(hwnd, IDC_CHECK_SUBTRACT);
        hCheckBoxMultiply = GetDlgItem(hwnd, IDC_CHECK_MULTIPLY);
        hCheckBoxDivide = GetDlgItem(hwnd, IDC_CHECK_DIVIDE);
        hButtonCalculate = GetDlgItem(hwnd, IDC_BUTTON_CALCULATE);
        hStaticResult = GetDlgItem(hwnd, IDC_STATIC_RESULT);

        SetCheckboxHandlers();
    }

    void SetCheckboxHandlers() 
    {
        Checkboxes = { hCheckBoxAdd, hCheckBoxSubtract, hCheckBoxMultiply, hCheckBoxDivide };
        for (HWND checkbox : Checkboxes) 
        {
            SendMessage(checkbox, BM_SETCHECK, BST_UNCHECKED, 0);
        }
    }

    INT_PTR HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
    {
        switch (uMsg) 
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BUTTON_CALCULATE && HIWORD(wParam) == BN_CLICKED) 
            {
                OnCalculate(hwnd, uMsg, wParam, lParam);
            }
            else if (IsCheckboxClicked(wParam)) 
            {
                OnCheckboxClick(wParam);
            }
            break;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        }

        return FALSE;
    }

    void OnCalculate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
    {
        int num1 = GetDlgItemInt(hwnd, IDC_EDIT1, NULL, FALSE);
        int num2 = GetDlgItemInt(hwnd, IDC_EDIT2, NULL, FALSE);

        int result = 0;
        for (HWND checkbox : Checkboxes) 
        {
            if (SendMessage(checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED) 
            {
                int operation = GetCheckboxOperation(checkbox);
                result = PerformOperation(hwnd, uMsg, wParam, lParam, num1, num2, operation);
                UncheckOtherCheckboxes(checkbox);
                break;
            }
        }

        // Display the result
        std::wstring resultText = L"Result: " + std::to_wstring(result);
        SetDlgItemText(hwnd, IDC_STATIC_RESULT, resultText.c_str());
    }

    void OnCheckboxClick(WPARAM checkboxId) 
    {
        for (HWND checkbox : Checkboxes) 
        {
            if (checkboxId == GetDlgCtrlID(checkbox)) 
            {
                SendMessage(checkbox, BM_SETCHECK, BST_CHECKED, 0);
            }
            else 
            {
                SendMessage(checkbox, BM_SETCHECK, BST_UNCHECKED, 0);
            }
        }
    }

    bool IsCheckboxClicked(WPARAM wParam) const 
    {
        for (HWND checkbox : Checkboxes) 
        {
            if (wParam == GetDlgCtrlID(checkbox)) 
            {
                return true;
            }
        }
        return false;
    }

    int GetCheckboxOperation(HWND checkbox) const
    {
        if (checkbox == hCheckBoxAdd) 
        {
            return 0; 
        }
        else if (checkbox == hCheckBoxSubtract) 
        {
            return 1; 
        }
        else if (checkbox == hCheckBoxMultiply) 
        {
            return 2;
        }
        else if (checkbox == hCheckBoxDivide) 
        {
            return 3; 
        }
        return -1; 
    }

    int PerformOperation(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int num1, int num2, int operation) const 
    {
        switch (operation) 
        {
        case 0:
            return num1 + num2;
        case 1:
            return num1 - num2;
        case 2:
            return num1 * num2;
        case 3:
            if (num2 != 0) {
                return num1 / num2;
            }
            if (num2 == 0) {
                MessageBox(hwnd, L"Cannot divide by zero!", L"Error", MB_OK | MB_ICONERROR);
                SetDlgItemText(hwnd, IDC_STATIC_RESULT, L"Error");
                return 0;
            }
        default:
            return 0;  
        }
    }

    void UncheckOtherCheckboxes(HWND checkedCheckbox) const 
    {
        for (HWND checkbox : Checkboxes) 
        {
            if (checkbox != checkedCheckbox) 
            {
                SendMessage(checkbox, BM_SETCHECK, BST_UNCHECKED, 0);
            }
        }
    }

    std::vector<HWND> Checkboxes;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) 
{
    Calculator calculator(hInstance);
    return calculator.ShowDialog();
}
