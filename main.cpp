#include <iostream>
#include <windows.h>

typedef NTSTATUS(WINAPI* RtlSetProcessIsCritical) (BOOLEAN, PBOOLEAN, BOOLEAN);

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    LUID luid;
    BOOL bRet = FALSE;

    if (LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
        TOKEN_PRIVILEGES tp;

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : 0;
        //
        //  Enable the privilege or disable all privileges.
        //
        if (AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        {
            //
            //  Check to see if you have proper access.
            //  You may get "ERROR_NOT_ALL_ASSIGNED".
            //
            bRet = (GetLastError() == ERROR_SUCCESS);
        }
    }
    return bRet;
}

int main()
{
    /*BlueScreen*/
    HMODULE ntdll = LoadLibraryA("ntdll.dll");//Load ntdll.dll.

    RtlSetProcessIsCritical SetProcessIsCritical = (RtlSetProcessIsCritical)GetProcAddress(ntdll, "RtlSetProcessIsCritical");

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hToken;

    /*Enable Debug Privilege*/
    if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
        CloseHandle(hToken);
    }

    SetProcessIsCritical(TRUE, NULL, FALSE);//Set Process is Critical.

    return 0;//Exit.
}
