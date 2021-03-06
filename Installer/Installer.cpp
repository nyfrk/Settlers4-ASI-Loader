///////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2020 nyfrk <nyfrk@gmx.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>

#include "resource.h"
#include "md5.h"
#include "Version.h"

// the md5 of the original binkw32.dll
#define BINKW_MD5 "\xEA\xE5\x89\x0D\x7B\xB2\x53\x2C\x55\x33\xD9\x4C\x30\x65\x1F\xEB"

#pragma comment(lib,"Version.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"Shell32.lib")

#define MAX_LOADSTRING 100

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


/// <summary>
/// Replaces all forward slashes with backward slashes
/// </summary>
/// <param name="str">The zero terminated string</param>
static void ReplaceForwardSlashes(LPTSTR str) {
    for (TCHAR* c = str; (c = _tcschr(c, _T('/'))) != NULL; ) *c = _T('\\');
}

/// <summary>
/// Get a directory from a registry entry. It will make sure that only 
/// backslashes will be used.
/// </summary>
/// <param name="dir">The path in the registry</param>
/// <param name="key">The key</param>
/// <param name="out">The directory that has been read from the specified 
/// registry key.</param>
/// <returns></returns>
static HRESULT RegGetDir(LPCTSTR dir, LPCTSTR key, LPTSTR out) {
    HRESULT err;
    DWORD bufSize = MAX_PATH;
    DWORD type = RRF_RT_REG_SZ;
    TCHAR buf[MAX_PATH+1];
    _tcscpy_s(buf, dir);
    _tcscat_s(buf, _T("\\"));
    _tcscat_s(buf, key);
    HKEY hKey;
    err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, dir, 0L, KEY_ALL_ACCESS, &hKey);
    if (err == ERROR_SUCCESS) {
        err = RegQueryValueEx(hKey, key, NULL, &type, (LPBYTE)out, &bufSize);
        RegCloseKey(hKey);
    }
    if (err == ERROR_SUCCESS) {
        out[bufSize] = '\0';
        // replace all forward slashes with backslashes
        ReplaceForwardSlashes(out);
    }
    return err;
}

/// <summary>
/// Get a pointer to the file name portion of a fully qualified filename.
/// </summary>
/// <param name="fqfn">The fully qualified filename</param>
/// <param name="len">The length of fqfn</param>
/// <returns>A pointer to the beginning of the file name portion</returns>
static PTCHAR GetStartOfFilename(LPTSTR fqfn, SIZE_T len) {
    for (unsigned i = len; i >= 0 && i <= len; --i) {
        if (fqfn[i] == '\\') return &(fqfn[++i]);
    }
    return fqfn;
}

/// <summary>
/// Get a pointer to the file name portion of a fully qualified filename.
/// </summary>
/// <param name="fqfn">The fully qualified filename</param>
/// <returns>A pointer to the beginning of the file name portion</returns>
static PTCHAR GetStartOfFilename(LPTSTR fqfn) {
    PTCHAR ret = fqfn;
    while (*fqfn != '\0') {
        bool set = *fqfn == '\\';
        ++fqfn;
        if (set) ret = fqfn;
    }
    return ret;
}

/// <summary>
/// Load a string from string table. Note that the returned string is only valid until the
/// next invocation of this function.
/// </summary>
/// <param name="id">The ID</param>
/// <returns>a pointer to the string.</returns>
LPCTSTR STR(UINT id) {
#ifdef UNICODE
    // Resource strings are always Unicode/UTF-16LE encoded. So this is only
    // possible when building with UNICODE.
    LPCTSTR buf = 0;
    auto len = LoadString(GetModuleHandle(NULL), id, (LPTSTR)&buf, 0);
    if (!len) {
        return _T("ERROR_STRING");
    } else {
        return buf;
    }
#else
    thread_local TCHAR buf[128];
    auto len = LoadString(GetModuleHandle(NULL), id, buf, _countof(buf) - 1);
    if (!len) {
        return _T("ERROR_STRING");
    }
    else {
        buf[len] = '\0';
        return buf;
    }
#endif
}

static WORD versionMayor = 0, versionMinor = 0; // Version of the payload

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {

    case WM_DESTROY:
        CoUninitialize();
        SetWindowLong(hDlg, DWL_MSGRESULT, 0);
        return (INT_PTR)TRUE;

    case WM_INITDIALOG:
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // set the icon
        SendMessage(hDlg, WM_SETICON, ICON_SMALL,
            (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON)));

        // Set the version number of the payload in the bottom left corner
        {
            auto err = GetPayloadVersion(versionMayor, versionMinor);
            TCHAR versionStr[48];
            if (!err) {
                _stprintf_s(versionStr, STR(IDS_VERSION_X_X), (DWORD)versionMayor, (DWORD)versionMinor);
                TCHAR g_HintInstallButton[128] = _T("");
                _stprintf_s(g_HintInstallButton, STR(IDS_STRING166), (DWORD)versionMayor, (DWORD)versionMinor);
                Edit_SetText(GetDlgItem(hDlg, IDC_MYVERSION), versionStr);
                Edit_SetText(GetDlgItem(hDlg, IDC_HINT), g_HintInstallButton);
            }
            else 
                _stprintf_s(versionStr, STR(IDS_STRING165), err);
            Edit_SetText(GetDlgItem(hDlg, IDC_MYVERSION), versionStr);
        }

        // Set the installation dir of the history edition if we can find it
        {
            TCHAR szFileDir[MAX_PATH + 1] = _T("");
            if (ERROR_SUCCESS == RegGetDir(
                _T("SOFTWARE\\ubisoft\\Launcher\\Installs\\11785"),
                _T("InstallDir"),
                szFileDir)) {

            }
            else { // if that fails, open the directory we are currently running in
                auto len = GetModuleFileName(NULL, szFileDir, MAX_PATH);
                *GetStartOfFilename(szFileDir, len) = '\0';
            }
            Edit_SetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir);
        }

        return (INT_PTR)TRUE;

    case WM_DROPFILES: {
        auto hDrop = (HDROP)wParam;
        TCHAR szDroppedFile[MAX_PATH];
        auto len = DragQueryFile(hDrop, 0, szDroppedFile, _countof(szDroppedFile));
        DragFinish(hDrop);
        if (len) {
            auto attr = GetFileAttributes(szDroppedFile);
            if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                *GetStartOfFilename(szDroppedFile, len) = '\0';
            }
            Edit_SetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szDroppedFile);
        }
        SetWindowLong(hDlg, DWL_MSGRESULT, 0);
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case IDCANCEL:
                EndDialog(hDlg, LOWORD(wParam));
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            case IDC_CHOOSE: {
                OPENFILENAME ofn;
                TCHAR szFile[MAX_PATH] = _T("S4_Main.exe");
                TCHAR szFileDir[MAX_PATH] = _T("");
                Edit_GetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir, _countof(szFileDir));
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hDlg;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = _countof(szFile);
                ofn.lpstrFilter = _T("S4_Main.exe\0S4_Main.EXE\0");
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.lpstrTitle = STR(IDS_STRING164);
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = szFileDir;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if (GetOpenFileName(&ofn) != FALSE) {
                    *GetStartOfFilename(szFile) = '\0' ;
                    Edit_SetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFile);
                }
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }
            case IDC_LINK: {
                ShellExecute(0, 0, STR(IDS_STRING167), 0, 0, SW_SHOW);
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }
            case IDC_INSTALLDIR: { // Detect version and update status 
                TCHAR szFileDir[MAX_PATH] = _T("");
                Edit_GetText((HWND)lParam, szFileDir, _countof(szFileDir));
                ReplaceForwardSlashes(szFileDir);
                auto attr = GetFileAttributes(szFileDir);
                if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    *GetStartOfFilename(szFileDir) = '\0';
                }
                auto len = _tcslen(szFileDir);
                if (len && len < _countof(szFileDir)-2) {
                    // append \ if it is missing from input
                    if (szFileDir[len-1] != '\\') {
                        szFileDir[len] = '\\';
                        szFileDir[++len] = '\0';
                    }
                    TCHAR szFileDst[_countof(szFileDir)];
                    LPTSTR fn = &szFileDir[len];
                    LPTSTR fnDst = &szFileDst[len];
                    auto remainingLen = _countof(szFileDir) - len;
                    _tcscpy_s(szFileDst, szFileDir);

                    _tcscpy_s(fn, remainingLen, _T("plugins"));
                    auto pluginsAttr = GetFileAttributes(szFileDir);
                    EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS), pluginsAttr != INVALID_FILE_ATTRIBUTES && pluginsAttr & FILE_ATTRIBUTE_DIRECTORY);

                    _tcscpy_s(fn   , remainingLen, _T("binkw32hooked.dll"));
                    _tcscpy_s(fnDst, remainingLen, _T("binkw32.dll"));
                    MD5 md5;
                    int binkw32State;
                    bool hasBinkw32Version = false;
                    if (ERROR_SUCCESS == ComputeMD5(szFileDir, &md5)) {
                        if (0 == memcmp(md5.bytes, BINKW_MD5, MD5LEN)) {
                            binkw32State = 0b0011; // binkw32hooked is setup correctly
                        } else {
                            binkw32State = 0b0010; // md5 does not match
                        }
                    } else {
                        binkw32State = 0b0001; // file does not exist or could not be examined
                    }

                    if (ERROR_SUCCESS == ComputeMD5(szFileDst, &md5)) {
                        if (0 == memcmp(md5.bytes, BINKW_MD5, MD5LEN)) {
                            binkw32State |= 0b1100; // binkw32 is original
                        } else {
                            binkw32State |= 0b1000; // md5 does not match
                        }
                    } else {
                        binkw32State |= 0b0100; // file does not exist or could not be examined
                    }

                    WORD binkw32mayor, binkw32minor, binkw32lmayor, binkw32lminor;
                    if (ERROR_SUCCESS == GetVersion(
                        szFileDst, 
                        binkw32mayor, 
                        binkw32minor, 
                        binkw32lmayor,
                        binkw32lminor,
                        _T("nyfrk"))) { // we compare the "company name" field so we can make sure that it is "our" DLL
                        binkw32State |= 0b10000;
                    }

                    TCHAR vers[80];
                    TCHAR status[80];
                    switch (binkw32State) {
                    case 0b11011: // binkw32 modded by us, binkw32hooked original
                    case 0b11010: // binkw32 modded by us, binkw32hooked modded
                        if (binkw32lmayor != 0 || binkw32lminor != 0)
                            _stprintf_s(vers, STR(IDS_VERSION_X_X_X_X), binkw32mayor, binkw32minor, binkw32lmayor, binkw32lminor);
                        else
                            _stprintf_s(vers, STR(IDS_VERSION_X_X), binkw32mayor, binkw32minor);
                        if (versionMayor == binkw32mayor && versionMinor == binkw32minor)
                            _tcscpy_s(status, STR(IDS_VERSION_ALREADY_INSTALLED));
                        else
                            _tcscpy_s(status, STR(IDS_UPDATEABLE));
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), TRUE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), TRUE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), TRUE);
                        break;
                    case 0b11001: // binkw32 modded by us, binkw32hooked not existant
                    case 0b01001: // binkw32 modded, binkw32hooked not existant
                        _tcscpy_s(vers, STR(IDS_STRING147));
                        _tcscpy_s(status, STR(IDS_STRING148));
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), FALSE);
                        break;
                    case 0b00111: // binkw32 doesnt exists, binkw32hooked original
                    case 0b01011: // binkw32 unknown, binkw32hooked original
                    case 0b01111: // binkw32 original, binkw32hooked original
                        _tcscpy_s(vers, STR(IDS_STRING149));
                        _tcscpy_s(status, STR(IDS_UPDATEABLE));
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), TRUE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), TRUE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), TRUE);
                        break;
                    case 0b01101: // binkw32 original, binkw32hooked doesnt exist
                        _tcscpy_s(vers, STR(IDS_STRING151));
                        _tcscpy_s(status, STR(IDS_STRING150));
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), TRUE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), TRUE);
                        break;
                    case 0b01110: // binkw32 original, binkw32hooked not original
                        if (DeleteFile(szFileDir)) { // safe to delete since we detected the original
                            _tcscpy_s(vers, STR(IDS_STRING151));
                            _tcscpy_s(status, STR(IDS_STRING150));
                            EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), TRUE);
                            EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                            EnableWindow(GetDlgItem(hDlg, IDC_HINT), TRUE);
                        } else {
                            _tcscpy_s(vers, _T(""));
                            _tcscpy_s(status, STR(IDS_STRING152));
                            EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), FALSE);
                            EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                            EnableWindow(GetDlgItem(hDlg, IDC_HINT), FALSE);
                        }
                    case 0b00101: // binkw32 doesnt exist, binkw32hooked doesnt exist
                        _tcscpy_s(vers, STR(IDS_STRING153));
                        status[0] = '\0';
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), FALSE);
                        break;
                    default:
                        _stprintf_s(vers, STR(IDS_STRING154), binkw32State);
                        _tcscpy_s(status, STR(IDS_STRING155));
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                        EnableWindow(GetDlgItem(hDlg, IDC_HINT), FALSE);
                        break;
                    }

                    Edit_SetText(GetDlgItem(hDlg, IDC_VERSION),vers);
                    Edit_SetText(GetDlgItem(hDlg, IDC_STATUS), status);
                }
                else {
                    Edit_SetText(GetDlgItem(hDlg, IDC_VERSION), _T(""));
                    Edit_SetText(GetDlgItem(hDlg, IDC_STATUS), STR(IDS_STRING156));
                    EnableWindow(GetDlgItem(hDlg, IDC_INSTALL), FALSE);
                    EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
                    EnableWindow(GetDlgItem(hDlg, IDC_HINT), FALSE);
                    EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS), FALSE);
                }
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }
            case IDC_INSTALL: {
                TCHAR szFileDir[MAX_PATH] = _T("");
                Edit_GetText(GetDlgItem(hDlg,IDC_INSTALLDIR), szFileDir, _countof(szFileDir));
                ReplaceForwardSlashes(szFileDir);
                auto attr = GetFileAttributes(szFileDir);
                if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    *GetStartOfFilename(szFileDir) = '\0';
                }
                auto len = _tcslen(szFileDir);
                if (len && len < _countof(szFileDir) - 2) {
                    // append \ if it is missing from input
                    if (szFileDir[len-1] != '\\') {
                        szFileDir[len] = '\\';
                        szFileDir[++len] = '\0';
                    }
                    TCHAR szFileDst[_countof(szFileDir)];
                    LPTSTR fn = &szFileDir[len];
                    LPTSTR fnDst = &szFileDst[len];
                    auto remainingLen = _countof(szFileDir) - len;
                    _tcscpy_s(szFileDst, szFileDir);

                    _tcscpy_s(fn, remainingLen, _T("plugins\\"));
                    if (!CreateDirectory(szFileDir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                        MessageBox(hDlg, STR(IDS_STRING157), STR(IDS_STRING158), MB_ICONERROR);
                    }

                    _tcscpy_s(fn, remainingLen, _T("binkw32hooked.dll"));
                    _tcscpy_s(fnDst, remainingLen, _T("binkw32.dll"));
                    if (CopyFile(szFileDst, szFileDir, TRUE) || GetLastError() == ERROR_FILE_EXISTS) {
                        auto hFile = CreateFile(szFileDst, // file name 
                            GENERIC_WRITE,        // open for write 
                            0,                    // no share 
                            NULL,                 // default security 
                            CREATE_ALWAYS,        // overwrite existing
                            FILE_ATTRIBUTE_NORMAL,// normal file 
                            NULL);                // no template 
                        if (hFile) {
                            auto hMod = GetModuleHandle(NULL);
                            auto res = FindResource(hMod, MAKEINTRESOURCE(IDR_BINKW32), RT_RCDATA);
                            auto hMem = LoadResource(hMod, res);
                            auto size = SizeofResource(hMod, res);
                            PVOID lpData = LockResource(hMem);
                            DWORD written = 0;
                            auto fsuccess = WriteFile(hFile, lpData, size, &written, NULL);
                            auto err = GetLastError();
                            CloseHandle(hFile);
                            if (fsuccess && written == size) {
                                szFileDir[len] = '\0';
                                Edit_SetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir);
                                //MessageBox(hDlg, _T("Loader wurde installiert."), _T("Erfolg"), MB_OK);
                            } else {
                                _stprintf_s(szFileDir, STR(IDS_STRING159), err);
                                MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                            }
                        }
                        else {
                            _stprintf_s(szFileDir, STR(IDS_STRING160), GetLastError());
                            MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                        }
                    }
                    else {
                        _stprintf_s(szFileDir, STR(IDS_STRING161), GetLastError());
                        MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                    }
                }
                else {
                    _stprintf_s(szFileDir, STR(IDS_STRING162), GetLastError());
                    MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                }

                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }
            case IDC_UNINSTALL: {
                TCHAR szFileDir[MAX_PATH] = _T("");
                Edit_GetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir, _countof(szFileDir));
                ReplaceForwardSlashes(szFileDir);
                auto attr = GetFileAttributes(szFileDir);
                if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    *GetStartOfFilename(szFileDir) = '\0';
                }
                auto len = _tcslen(szFileDir);
                if (len && len < _countof(szFileDir) - 2) {
                    // append \ if it is missing from input
                    if (szFileDir[len - 1] != '\\') {
                        szFileDir[len] = '\\';
                        szFileDir[++len] = '\0';
                    }
                    TCHAR szFileDst[_countof(szFileDir)];
                    LPTSTR fn = &szFileDir[len];
                    LPTSTR fnDst = &szFileDst[len];
                    auto remainingLen = _countof(szFileDir) - len;
                    _tcscpy_s(szFileDst, szFileDir);
                    _tcscpy_s(fn, remainingLen, _T("binkw32hooked.dll"));
                    _tcscpy_s(fnDst, remainingLen, _T("binkw32.dll"));
                    if (CopyFile(szFileDir, szFileDst, FALSE)) {
                        if (!DeleteFile(szFileDir)) {
                            _stprintf_s(szFileDir, STR(IDS_STRING163), GetLastError());
                            MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                        }
                    } else {
                        _stprintf_s(szFileDir, STR(IDS_STRING161), GetLastError());
                        MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                    }
                    *fn = '\0';
                    Edit_SetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir);
                } else {
                    _stprintf_s(szFileDir, STR(IDS_STRING162), GetLastError());
                    MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                }
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }
            case IDC_PLUGINS: {
                TCHAR szFileDir[MAX_PATH] = _T("");
                Edit_GetText(GetDlgItem(hDlg, IDC_INSTALLDIR), szFileDir, _countof(szFileDir));
                ReplaceForwardSlashes(szFileDir);
                auto attr = GetFileAttributes(szFileDir);
                if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    *GetStartOfFilename(szFileDir) = '\0';
                }
                auto len = _tcslen(szFileDir);
                if (len && len < _countof(szFileDir) - 2) {
                    // append \ if it is missing from input
                    if (szFileDir[len - 1] != '\\') {
                        szFileDir[len] = '\\';
                        szFileDir[++len] = '\0';
                    }
                    LPTSTR fn = &szFileDir[len];
                    auto remainingLen = _countof(szFileDir) - len;
                    _tcscpy_s(fn, remainingLen, _T("plugins\\"));

                    ShellExecute(NULL, _T("explore"), szFileDir, NULL, NULL, SW_SHOWDEFAULT);
                }
                else {
                    _stprintf_s(szFileDir, STR(IDS_STRING162), GetLastError());
                    MessageBox(hDlg, szFileDir, STR(IDS_STRING158), MB_OK | MB_ICONERROR);
                }
                SetWindowLong(hDlg, DWL_MSGRESULT, 0);
                return (INT_PTR)TRUE;
            }

        }
        break;
    }
    return (INT_PTR)FALSE;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // To quickly test the a specific language uncomment the following line
    // and pass one of the following arguments
    // 0x407 for de-DE
    // 0x409 for en-US
    // SetThreadUILanguage(0x409);

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

    return 0;
}
