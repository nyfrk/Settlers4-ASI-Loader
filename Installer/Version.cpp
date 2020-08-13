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
#include "windows.h"
#include <tchar.h>
#include <stdio.h>
#include "resource.h"


#define RtlOffsetToPointer(B,O)  ((PCHAR)( ((PCHAR)(B)) + ((ULONG_PTR)(O))  ))

/// <summary>
/// Manually maps an image from memory for use with FindResource(image)...
/// See https://stackoverflow.com/a/39401163/1642059 for source
/// </summary>
/// <param name="pvRawData">The unmapped module in memory</param>
/// <param name="cbRawData">Size of the data pointed by pvRawData</param>
/// <returns>Mapped image. If not used anymore pass it to VirtualFree with MEM_RELEASE</returns>
PVOID MapImage(PIMAGE_DOS_HEADER pvRawData, ULONG cbRawData)
{
    if (cbRawData < sizeof(IMAGE_DOS_HEADER)) {
        return 0;
    }

    if (pvRawData->e_magic != IMAGE_DOS_SIGNATURE) {
        return 0;
    }

    ULONG e_lfanew = pvRawData->e_lfanew, s = e_lfanew + sizeof(IMAGE_NT_HEADERS);

    if (e_lfanew >= s || s > cbRawData) {
        return 0;
    }

    PIMAGE_NT_HEADERS pinth = (PIMAGE_NT_HEADERS)RtlOffsetToPointer(pvRawData, e_lfanew);

    if (pinth->Signature != IMAGE_NT_SIGNATURE) {
        return 0;
    }

    ULONG SizeOfImage = pinth->OptionalHeader.SizeOfImage, SizeOfHeaders = pinth->OptionalHeader.SizeOfHeaders;

    s = e_lfanew + SizeOfHeaders;

    if (SizeOfHeaders > SizeOfImage || SizeOfHeaders >= s || s > cbRawData) {
        return 0;
    }

    s = FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + pinth->FileHeader.SizeOfOptionalHeader;

    if (s > SizeOfHeaders) {
        return 0;
    }

    ULONG NumberOfSections = pinth->FileHeader.NumberOfSections;

    PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER)RtlOffsetToPointer(pinth, s);

    ULONG Size;

    if (NumberOfSections) {
        if (e_lfanew + s + NumberOfSections * sizeof(IMAGE_SECTION_HEADER) > SizeOfHeaders) {
            return 0;
        }

        do {
            if (Size = min(pish->Misc.VirtualSize, pish->SizeOfRawData)) {
                union {
                    ULONG VirtualAddress, PointerToRawData;
                };

                VirtualAddress = pish->VirtualAddress, s = VirtualAddress + Size;

                if (VirtualAddress > s || s > SizeOfImage) {
                    return 0;
                }

                PointerToRawData = pish->PointerToRawData, s = PointerToRawData + Size;

                if (PointerToRawData > s || s > cbRawData) {
                    return 0;
                }
            }
        } while (pish++, --NumberOfSections);
    }

    PVOID ImageBase = VirtualAlloc(0, SizeOfImage, MEM_COMMIT, PAGE_READWRITE);

    if (!ImageBase) {
        return 0;
    }

    memcpy(ImageBase, pvRawData, SizeOfHeaders);

    if (NumberOfSections = pinth->FileHeader.NumberOfSections) {
        do {
            --pish;
            if (Size = min(pish->Misc.VirtualSize, pish->SizeOfRawData)) {
                memcpy(RtlOffsetToPointer(ImageBase, pish->VirtualAddress),
                    RtlOffsetToPointer(pvRawData, pish->PointerToRawData), Size);
            }
        } while (--NumberOfSections);
    }
    return ImageBase;
}

/// <summary>
/// Get the version of the payload. 
/// </summary>
/// <param name="mayor"></param>
/// <param name="minor"></param>
/// <returns>ERROR_SUCCESS or an error code</returns>
DWORD GetPayloadVersion(WORD& mayor, WORD& minor) {
    auto hMod = GetModuleHandle(NULL);
    HRSRC hResInfo = FindResource(hMod, MAKEINTRESOURCE(IDR_BINKW32), RT_RCDATA);
    if (!hResInfo) return GetLastError();
    HGLOBAL hResData = LoadResource(hMod, hResInfo);
    if (!hResData) return GetLastError();
    DWORD dwSize = SizeofResource(hMod, hResInfo);
    if (!dwSize) return GetLastError();
    PVOID lpData = LockResource(hResData);
    if (!lpData) return GetLastError();
    HMODULE hInst = (HMODULE)MapImage((PIMAGE_DOS_HEADER)lpData, dwSize);
    if (!hInst) return GetLastError();
    hResInfo = FindResource(hInst, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION); // note, this is actually a cheat, as this is the id of OUR version info. But it should be the same for the payload.
    if (!hResInfo) { VirtualFree(hInst, 0, MEM_RELEASE);  return GetLastError(); }
    dwSize = SizeofResource(hInst, hResInfo);
    if (!dwSize) { VirtualFree(hInst, 0, MEM_RELEASE);  return GetLastError(); }
    hResData = LoadResource(hInst, hResInfo);
    if (!hResData) { VirtualFree(hInst, 0, MEM_RELEASE);  return GetLastError(); }
    LPVOID pRes = LockResource(hResData);
    if (!pRes) { VirtualFree(hInst, 0, MEM_RELEASE);  return GetLastError(); }
    LPVOID pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
    if (!pResCopy) { VirtualFree(hInst, 0, MEM_RELEASE);  return GetLastError(); }
    CopyMemory(pResCopy, pRes, dwSize);
    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    auto queryVal = VerQueryValue(pResCopy, TEXT("\\"), (LPVOID*)&lpFfi, &uLen);
    if (!queryVal) {
        LocalFree(pResCopy); // release the copy
        VirtualFree(hInst, 0, MEM_RELEASE);
        return GetLastError();
    }
    mayor = HIWORD(lpFfi->dwFileVersionMS);
    minor = LOWORD(lpFfi->dwFileVersionMS);
    LocalFree(pResCopy); // release the copy
    VirtualFree(hInst, 0, MEM_RELEASE);
    return ERROR_SUCCESS;
}

DWORD GetVersion(LPCTSTR szVersionFile, WORD& mayor, WORD& minor, WORD& lmayor, WORD& lminor, LPTSTR company) {
    bool succ = false;
    DWORD  verHandle = 0;
    UINT   size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD  verSize = GetFileVersionInfoSize(szVersionFile, &verHandle);

    if (verSize != NULL) {
        void* verData = new BYTE[verSize];
        if (GetFileVersionInfo(szVersionFile, verHandle, verSize, verData)) {
            if (VerQueryValue(verData, _T("\\"), (VOID FAR * FAR*) & lpBuffer, &size)) {
                if (size) {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd) {
                        mayor = HIWORD(verInfo->dwFileVersionMS);
                        minor = LOWORD(verInfo->dwFileVersionMS);
                        lmayor = HIWORD(verInfo->dwFileVersionLS);
                        lminor = LOWORD(verInfo->dwFileVersionLS);
                        succ = true;
                    }
                }
            }
            UINT cbLang;
            WORD* langInfo;
            if (VerQueryValue(verData, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langInfo, &cbLang)) {
                TCHAR tszVerStrName[128];
                _stprintf_s(tszVerStrName, _T("\\StringFileInfo\\%04x%04x\\%s"), langInfo[0], langInfo[1], _T("CompanyName"));
                LPVOID lpt;
                if (VerQueryValue(verData, tszVerStrName, &lpt, &size)) {
                    succ = succ && 0 == _tcscmp((LPTSTR)lpt, company);
                }
                else { succ = false; }
            } else { succ = false; }
        }
        delete[] verData;
    }
    if (!succ) {
        auto err = GetLastError();
        if (err == ERROR_SUCCESS)
            return -1;
        else
            return err;
    }
    return ERROR_SUCCESS;
}