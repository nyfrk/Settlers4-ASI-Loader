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
#include <cstdio>
#include <stdlib.h>
#include <shlwapi.h>
#include <set>
#include <string>

#define PLUGINFILTER L"*.asi"
#define PLUGINDIR L"plugins\\"

#define ERROR_BOX(err) {\
	const TCHAR fmt[] = err"\n\nError Code %d.";\
	TCHAR buf[sizeof(fmt) + 32];\
	swprintf_s(buf, fmt, (DWORD)GetLastError());\
	MessageBox(NULL, buf, L"ASI LOADER ERROR", MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);\
	ExitProcess(0);\
}

// Load the plugins
DWORD __stdcall AsiLoad(HMODULE hModule) {
	// Get the directory of the exe
	HMODULE main = GetModuleHandle(NULL);
	if (main == NULL)
		ERROR_BOX(L"Cannot get module handle of your exe.");
	wchar_t filename[MAX_PATH + 1 + _countof(PLUGINDIR PLUGINFILTER)];
	auto len = GetModuleFileName(main, filename, MAX_PATH);
	if (len <= 0)
		ERROR_BOX(L"Cannot get file name of your exe.");
	// find where the directory ends (i.e. where the exe name begins)
	filename[len] = L'\0';
	for (--len; len >= 0; len--) {
		if (filename[len] == L'\\') {
			filename[++len] = L'\0';
			break;
		}
	}
	// change directory to PLUGINDIR and add PLUGINFILTER
	memcpy(&(filename[len]), PLUGINDIR PLUGINFILTER, sizeof(PLUGINDIR PLUGINFILTER));
	len += _countof(PLUGINDIR) - 1; // len of the fully qualified plugin dir
	auto remainingLen = _countof(filename) - len;
	auto pluginname = &(filename[len]);
	std::set<std::wstring> plugins;
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	hFind = FindFirstFile(filename, &ffd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			// Due to some DOS compatibility FindFirstFile does match *.asi*.
			// So we use this to match to *.asi only.
			if (!PathMatchSpec(ffd.cFileName, PLUGINFILTER))
				continue;
			plugins.emplace(ffd.cFileName);
		} while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
		
		// iterates the plugins alphabetically to load them in a predictable order
		for (auto& plugin : plugins) {
			wcscpy_s(pluginname, remainingLen, plugin.c_str()); // make it a fully qualified name
			HMODULE hMod = LoadLibrary(filename);
			if (!hMod) {
				const TCHAR fmt[] = L"Cannot load plugin\n%s" L"\n\nError Code %d.";
				TCHAR buf[sizeof(fmt) + MAX_PATH + 32];
				swprintf_s(buf, fmt, filename, (DWORD)GetLastError());
				MessageBox(NULL, buf, L"ASI LOADER ERROR", MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
			}
		}
	} 

	if (hModule) {
		FreeLibraryAndExitThread(hModule, 0);
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		// We try to avoid loading the plugins from DllMain. In the DllMain we may
		// only expect kernel32 imports to be resolved. Furthermore we are in a loader
		// lock. We therefore try to create a new thread to load all the plugins. 
		// We increase the ref count of our module to prevent unloading of our dll
		// while the thread is still running. The system starts the thread only when 
		// all imports are mapped. So we can use User32.dll etc
		// Note that we must use FreeLibraryAndExitThread to prevent the unload/exit
		// race condition
		HMODULE mod;
		if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)AsiLoad, &mod)) {
			auto h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AsiLoad, hModule, 0, NULL);
			if (h) CloseHandle(h);
			else { AsiLoad(NULL); FreeLibrary(mod); }
		}
		else {
			AsiLoad(NULL);
		}
	}
	return TRUE;
}
