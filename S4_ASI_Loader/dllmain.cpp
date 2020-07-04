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
#include <vector>

#define PLUGINFILTER L"*.asi"
#define PLUGINDIR L"plugins\\"

#define ERROR_BOX(err) {\
	const TCHAR fmt[] = err"\n\nError Code %d.";\
	TCHAR buf[sizeof(fmt) + 32];\
	swprintf_s(buf, fmt, (DWORD)GetLastError());\
	MessageBox(NULL, buf, L"ASI LOADER ERROR", MB_ICONERROR);\
	ExitProcess(0);\
}

#define STDCALL_DECORATION __declspec(dllexport) __declspec(naked) void* __stdcall
#define STDCALL_PROXY_0(name) STDCALL_DECORATION name()
#define STDCALL_PROXY_4(name) STDCALL_DECORATION name(void *a)
#define STDCALL_PROXY_8(name) STDCALL_DECORATION name(void *a, void *b)
#define STDCALL_PROXY_12(name) STDCALL_DECORATION name(void *a, void *b, void *c)
#define STDCALL_PROXY_16(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d)
#define STDCALL_PROXY_20(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e)
#define STDCALL_PROXY_24(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f)
#define STDCALL_PROXY_28(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g)
#define STDCALL_PROXY_32(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h)
#define STDCALL_PROXY_36(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i)
#define STDCALL_PROXY_48(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i, void *j, void *k, void *l)
#define STDCALL_PROXY_52(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i, void *j, void *k, void *l, void *m)

static std::vector<HMODULE> plugins; // handles to all the plugins we loaded

struct binkw32_t { 
	HMODULE hDll;
	FARPROC Orig_BinkBufferBlit_12;
	FARPROC Orig_BinkBufferCheckWinPos_12;
	FARPROC Orig_BinkBufferClear_8;
	FARPROC Orig_BinkBufferClose_4;
	FARPROC Orig_BinkBufferGetDescription_4;
	FARPROC Orig_BinkBufferGetError_0;
	FARPROC Orig_BinkBufferLock_4;
	FARPROC Orig_BinkBufferOpen_16;
	FARPROC Orig_BinkBufferSetDirectDraw_8;
	FARPROC Orig_BinkBufferSetHWND_8;
	FARPROC Orig_BinkBufferSetOffset_12;
	FARPROC Orig_BinkBufferSetResolution_12;
	FARPROC Orig_BinkBufferSetScale_12;
	FARPROC Orig_BinkBufferUnlock_4;
	FARPROC Orig_BinkCheckCursor_20;
	FARPROC Orig_BinkClose_4;
	FARPROC Orig_BinkCloseTrack_4;
	FARPROC Orig_BinkCopyToBuffer_28;
	FARPROC Orig_BinkDDSurfaceType_4;
	FARPROC Orig_BinkDX8SurfaceType_4;
	FARPROC Orig_BinkDoFrame_4;
	FARPROC Orig_BinkGetError_0;
	FARPROC Orig_BinkGetKeyFrame_12;
	FARPROC Orig_BinkGetRealtime_12;
	FARPROC Orig_BinkGetRects_8;
	FARPROC Orig_BinkGetSummary_8;
	FARPROC Orig_BinkGetTrackData_8;
	FARPROC Orig_BinkGetTrackID_8;
	FARPROC Orig_BinkGetTrackMaxSize_8;
	FARPROC Orig_BinkGetTrackType_8;
	FARPROC Orig_BinkGoto_12;
	FARPROC Orig_BinkIsSoftwareCursor_8;
	FARPROC Orig_BinkLogoAddress_0;
	FARPROC Orig_BinkNextFrame_4;
	FARPROC Orig_BinkOpen_8;
	FARPROC Orig_BinkOpenDirectSound_4;
	FARPROC Orig_BinkOpenMiles_4;
	FARPROC Orig_BinkOpenTrack_8;
	FARPROC Orig_BinkOpenWaveOut_4;
	FARPROC Orig_BinkPause_8;
	FARPROC Orig_BinkRestoreCursor_4;
	FARPROC Orig_BinkService_4;
	FARPROC Orig_BinkSetError_4;
	FARPROC Orig_BinkSetFrameRate_8;
	FARPROC Orig_BinkSetIO_4;
	FARPROC Orig_BinkSetIOSize_4;
	FARPROC Orig_BinkSetMixBins_8;
	FARPROC Orig_BinkSetPan_8;
	FARPROC Orig_BinkSetSimulate_4;
	FARPROC Orig_BinkSetSoundOnOff_8;
	FARPROC Orig_BinkSetSoundSystem_8;
	FARPROC Orig_BinkSetSoundTrack_4;
	FARPROC Orig_BinkSetVideoOnOff_8;
	FARPROC Orig_BinkSetVolume_8;
	FARPROC Orig_BinkWait_4;
	FARPROC Orig_RADSetMemory_8;
	FARPROC Orig_RADTimerRead_0;
	FARPROC Orig_YUV_blit_16a1bpp_52;
	FARPROC Orig_YUV_blit_16a1bpp_mask_52;
	FARPROC Orig_YUV_blit_16a4bpp_52;
	FARPROC Orig_YUV_blit_16a4bpp_mask_52;
	FARPROC Orig_YUV_blit_16bpp_48;
	FARPROC Orig_YUV_blit_16bpp_mask_48;
	FARPROC Orig_YUV_blit_24bpp_48;
	FARPROC Orig_YUV_blit_24bpp_mask_48;
	FARPROC Orig_YUV_blit_24rbpp_48;
	FARPROC Orig_YUV_blit_24rbpp_mask_48;
	FARPROC Orig_YUV_blit_32abpp_52;
	FARPROC Orig_YUV_blit_32abpp_mask_52;
	FARPROC Orig_YUV_blit_32bpp_48;
	FARPROC Orig_YUV_blit_32bpp_mask_48;
	FARPROC Orig_YUV_blit_32rabpp_52;
	FARPROC Orig_YUV_blit_32rabpp_mask_52;
	FARPROC Orig_YUV_blit_32rbpp_48;
	FARPROC Orig_YUV_blit_32rbpp_mask_48;
	FARPROC Orig_YUV_blit_UYVY_48;
	FARPROC Orig_YUV_blit_UYVY_mask_48;
	FARPROC Orig_YUV_blit_YUY2_48;
	FARPROC Orig_YUV_blit_YUY2_mask_48;
	FARPROC Orig_YUV_blit_YV12_52;
	FARPROC Orig_YUV_init_4;
	FARPROC Orig_radfree_4;
	FARPROC Orig_radmalloc_4;
} binkw32;

extern "C" {
	STDCALL_PROXY_12(BinkBufferBlit) { _asm { jmp[binkw32.Orig_BinkBufferBlit_12] } }
	STDCALL_PROXY_12(BinkBufferCheckWinPos) { _asm { jmp[binkw32.Orig_BinkBufferCheckWinPos_12] } }
	STDCALL_PROXY_8(BinkBufferClear) { _asm { jmp[binkw32.Orig_BinkBufferClear_8] } }
	STDCALL_PROXY_4(BinkBufferClose) { _asm { jmp[binkw32.Orig_BinkBufferClose_4] } }
	STDCALL_PROXY_4(BinkBufferGetDescription) { _asm { jmp[binkw32.Orig_BinkBufferGetDescription_4] } }
	STDCALL_PROXY_0(BinkBufferGetError) { _asm { jmp[binkw32.Orig_BinkBufferGetError_0] } }
	STDCALL_PROXY_4(BinkBufferLock) { _asm { jmp[binkw32.Orig_BinkBufferLock_4] } }
	STDCALL_PROXY_16(BinkBufferOpen) { _asm { jmp[binkw32.Orig_BinkBufferOpen_16] } }
	STDCALL_PROXY_8(BinkBufferSetDirectDraw) { _asm { jmp[binkw32.Orig_BinkBufferSetDirectDraw_8] } }
	STDCALL_PROXY_8(BinkBufferSetHWND) { _asm { jmp[binkw32.Orig_BinkBufferSetHWND_8] } }
	STDCALL_PROXY_12(BinkBufferSetOffset) { _asm { jmp[binkw32.Orig_BinkBufferSetOffset_12] } }
	STDCALL_PROXY_12(BinkBufferSetResolution) { _asm { jmp[binkw32.Orig_BinkBufferSetResolution_12] } }
	STDCALL_PROXY_12(BinkBufferSetScale) { _asm { jmp[binkw32.Orig_BinkBufferSetScale_12] } }
	STDCALL_PROXY_4(BinkBufferUnlock) { _asm { jmp[binkw32.Orig_BinkBufferUnlock_4] } }
	STDCALL_PROXY_20(BinkCheckCursor) { _asm { jmp[binkw32.Orig_BinkCheckCursor_20] } }
	STDCALL_PROXY_4(BinkClose) { _asm { jmp[binkw32.Orig_BinkClose_4] } }
	STDCALL_PROXY_4(BinkCloseTrack) { _asm { jmp[binkw32.Orig_BinkCloseTrack_4] } }
	STDCALL_PROXY_28(BinkCopyToBuffer) { _asm { jmp[binkw32.Orig_BinkCopyToBuffer_28] } }
	STDCALL_PROXY_4(BinkDDSurfaceType) { _asm { jmp[binkw32.Orig_BinkDDSurfaceType_4] } }
	STDCALL_PROXY_4(BinkDX8SurfaceType) { _asm { jmp[binkw32.Orig_BinkDX8SurfaceType_4] } }
	STDCALL_PROXY_4(BinkDoFrame) { _asm { jmp[binkw32.Orig_BinkDoFrame_4] } }
	STDCALL_PROXY_0(BinkGetError) { _asm { jmp[binkw32.Orig_BinkGetError_0] } }
	STDCALL_PROXY_12(BinkGetKeyFrame) { _asm { jmp[binkw32.Orig_BinkGetKeyFrame_12] } }
	STDCALL_PROXY_12(BinkGetRealtime) { _asm { jmp[binkw32.Orig_BinkGetRealtime_12] } }
	STDCALL_PROXY_8(BinkGetRects) { _asm { jmp[binkw32.Orig_BinkGetRects_8] } }
	STDCALL_PROXY_8(BinkGetSummary) { _asm { jmp[binkw32.Orig_BinkGetSummary_8] } }
	STDCALL_PROXY_8(BinkGetTrackData) { _asm { jmp[binkw32.Orig_BinkGetTrackData_8] } }
	STDCALL_PROXY_8(BinkGetTrackID) { _asm { jmp[binkw32.Orig_BinkGetTrackID_8] } }
	STDCALL_PROXY_8(BinkGetTrackMaxSize) { _asm { jmp[binkw32.Orig_BinkGetTrackMaxSize_8] } }
	STDCALL_PROXY_8(BinkGetTrackType) { _asm { jmp[binkw32.Orig_BinkGetTrackType_8] } }
	STDCALL_PROXY_12(BinkGoto) { _asm { jmp[binkw32.Orig_BinkGoto_12] } }
	STDCALL_PROXY_8(BinkIsSoftwareCursor) { _asm { jmp[binkw32.Orig_BinkIsSoftwareCursor_8] } }
	STDCALL_PROXY_0(BinkLogoAddress) { _asm { jmp[binkw32.Orig_BinkLogoAddress_0] } }
	STDCALL_PROXY_4(BinkNextFrame) { _asm { jmp[binkw32.Orig_BinkNextFrame_4] } }
	STDCALL_PROXY_8(BinkOpen) { _asm { jmp[binkw32.Orig_BinkOpen_8] } }
	STDCALL_PROXY_4(BinkOpenDirectSound) { _asm { jmp[binkw32.Orig_BinkOpenDirectSound_4] } }
	STDCALL_PROXY_4(BinkOpenMiles) { _asm { jmp[binkw32.Orig_BinkOpenMiles_4] } }
	STDCALL_PROXY_8(BinkOpenTrack) { _asm { jmp[binkw32.Orig_BinkOpenTrack_8] } }
	STDCALL_PROXY_4(BinkOpenWaveOut) { _asm { jmp[binkw32.Orig_BinkOpenWaveOut_4] } }
	STDCALL_PROXY_8(BinkPause) { _asm { jmp[binkw32.Orig_BinkPause_8] } }
	STDCALL_PROXY_4(BinkRestoreCursor) { _asm { jmp[binkw32.Orig_BinkRestoreCursor_4] } }
	STDCALL_PROXY_4(BinkService) { _asm { jmp[binkw32.Orig_BinkService_4] } }
	STDCALL_PROXY_4(BinkSetError) { _asm { jmp[binkw32.Orig_BinkSetError_4] } }
	STDCALL_PROXY_8(BinkSetFrameRate) { _asm { jmp[binkw32.Orig_BinkSetFrameRate_8] } }
	STDCALL_PROXY_4(BinkSetIO) { _asm { jmp[binkw32.Orig_BinkSetIO_4] } }
	STDCALL_PROXY_4(BinkSetIOSize) { _asm { jmp[binkw32.Orig_BinkSetIOSize_4] } }
	STDCALL_PROXY_8(BinkSetMixBins) { _asm { jmp[binkw32.Orig_BinkSetMixBins_8] } }
	STDCALL_PROXY_8(BinkSetPan) { _asm { jmp[binkw32.Orig_BinkSetPan_8] } }
	STDCALL_PROXY_4(BinkSetSimulate) { _asm { jmp[binkw32.Orig_BinkSetSimulate_4] } }
	STDCALL_PROXY_8(BinkSetSoundOnOff) { _asm { jmp[binkw32.Orig_BinkSetSoundOnOff_8] } }
	STDCALL_PROXY_8(BinkSetSoundSystem) { _asm { jmp[binkw32.Orig_BinkSetSoundSystem_8] } }
	STDCALL_PROXY_4(BinkSetSoundTrack) { _asm { jmp[binkw32.Orig_BinkSetSoundTrack_4] } }
	STDCALL_PROXY_8(BinkSetVideoOnOff) { _asm { jmp[binkw32.Orig_BinkSetVideoOnOff_8] } }
	STDCALL_PROXY_8(BinkSetVolume) { _asm { jmp[binkw32.Orig_BinkSetVolume_8] } }
	STDCALL_PROXY_4(BinkWait) { _asm { jmp[binkw32.Orig_BinkWait_4] } }
	STDCALL_PROXY_8(RADSetMemory) { _asm { jmp[binkw32.Orig_RADSetMemory_8] } }
	STDCALL_PROXY_0(RADTimerRead) { _asm { jmp[binkw32.Orig_RADTimerRead_0] } }
	STDCALL_PROXY_52(YUV_blit_16a1bpp) { _asm { jmp[binkw32.Orig_YUV_blit_16a1bpp_52] } }
	STDCALL_PROXY_52(YUV_blit_16a1bpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_16a1bpp_mask_52] } }
	STDCALL_PROXY_52(YUV_blit_16a4bpp) { _asm { jmp[binkw32.Orig_YUV_blit_16a4bpp_52] } }
	STDCALL_PROXY_52(YUV_blit_16a4bpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_16a4bpp_mask_52] } }
	STDCALL_PROXY_48(YUV_blit_16bpp) { _asm { jmp[binkw32.Orig_YUV_blit_16bpp_48] } }
	STDCALL_PROXY_48(YUV_blit_16bpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_16bpp_mask_48] } }
	STDCALL_PROXY_48(YUV_blit_24bpp) { _asm { jmp[binkw32.Orig_YUV_blit_24bpp_48] } }
	STDCALL_PROXY_48(YUV_blit_24bpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_24bpp_mask_48] } }
	STDCALL_PROXY_48(YUV_blit_24rbpp) { _asm { jmp[binkw32.Orig_YUV_blit_24rbpp_48] } }
	STDCALL_PROXY_48(YUV_blit_24rbpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_24rbpp_mask_48] } }
	STDCALL_PROXY_52(YUV_blit_32abpp) { _asm { jmp[binkw32.Orig_YUV_blit_32abpp_52] } }
	STDCALL_PROXY_52(YUV_blit_32abpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_32abpp_mask_52] } }
	STDCALL_PROXY_48(YUV_blit_32bpp) { _asm { jmp[binkw32.Orig_YUV_blit_32bpp_48] } }
	STDCALL_PROXY_48(YUV_blit_32bpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_32bpp_mask_48] } }
	STDCALL_PROXY_52(YUV_blit_32rabpp) { _asm { jmp[binkw32.Orig_YUV_blit_32rabpp_52] } }
	STDCALL_PROXY_52(YUV_blit_32rabpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_32rabpp_mask_52] } }
	STDCALL_PROXY_48(YUV_blit_32rbpp) { _asm { jmp[binkw32.Orig_YUV_blit_32rbpp_48] } }
	STDCALL_PROXY_48(YUV_blit_32rbpp_mask) { _asm { jmp[binkw32.Orig_YUV_blit_32rbpp_mask_48] } }
	STDCALL_PROXY_48(YUV_blit_UYVY) { _asm { jmp[binkw32.Orig_YUV_blit_UYVY_48] } }
	STDCALL_PROXY_48(YUV_blit_UYVY_mask) { _asm { jmp[binkw32.Orig_YUV_blit_UYVY_mask_48] } }
	STDCALL_PROXY_48(YUV_blit_YUY2) { _asm { jmp[binkw32.Orig_YUV_blit_YUY2_48] } }
	STDCALL_PROXY_48(YUV_blit_YUY2_mask) { _asm { jmp[binkw32.Orig_YUV_blit_YUY2_mask_48] } }
	STDCALL_PROXY_52(YUV_blit_YV12) { _asm { jmp[binkw32.Orig_YUV_blit_YV12_52] } }
	STDCALL_PROXY_4(YUV_init) { _asm { jmp[binkw32.Orig_YUV_init_4] } }
	STDCALL_PROXY_4(radfree) { _asm { jmp[binkw32.Orig_radfree_4] } }
	STDCALL_PROXY_4(radmalloc) { _asm { jmp[binkw32.Orig_radmalloc_4] } }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		binkw32.hDll = LoadLibraryA("binkw32Hooked.dll");
		if (binkw32.hDll == NULL)
			ERROR_BOX(L"Cannot load original binkw32Hooked.dll library!\n\nTo use this ASI loader, you must rename the original binkw32.dll to binkw32hooked.dll. Repair the game and make sure to exactly follow the instructions of the README.");
		binkw32.Orig_BinkBufferBlit_12 = GetProcAddress(binkw32.hDll, "_BinkBufferBlit@12");
		binkw32.Orig_BinkBufferCheckWinPos_12 = GetProcAddress(binkw32.hDll, "_BinkBufferCheckWinPos@12");
		binkw32.Orig_BinkBufferClear_8 = GetProcAddress(binkw32.hDll, "_BinkBufferClear@8");
		binkw32.Orig_BinkBufferClose_4 = GetProcAddress(binkw32.hDll, "_BinkBufferClose@4");
		binkw32.Orig_BinkBufferGetDescription_4 = GetProcAddress(binkw32.hDll, "_BinkBufferGetDescription@4");
		binkw32.Orig_BinkBufferGetError_0 = GetProcAddress(binkw32.hDll, "_BinkBufferGetError@0");
		binkw32.Orig_BinkBufferLock_4 = GetProcAddress(binkw32.hDll, "_BinkBufferLock@4");
		binkw32.Orig_BinkBufferOpen_16 = GetProcAddress(binkw32.hDll, "_BinkBufferOpen@16");
		binkw32.Orig_BinkBufferSetDirectDraw_8 = GetProcAddress(binkw32.hDll, "_BinkBufferSetDirectDraw@8");
		binkw32.Orig_BinkBufferSetHWND_8 = GetProcAddress(binkw32.hDll, "_BinkBufferSetHWND@8");
		binkw32.Orig_BinkBufferSetOffset_12 = GetProcAddress(binkw32.hDll, "_BinkBufferSetOffset@12");
		binkw32.Orig_BinkBufferSetResolution_12 = GetProcAddress(binkw32.hDll, "_BinkBufferSetResolution@12");
		binkw32.Orig_BinkBufferSetScale_12 = GetProcAddress(binkw32.hDll, "_BinkBufferSetScale@12");
		binkw32.Orig_BinkBufferUnlock_4 = GetProcAddress(binkw32.hDll, "_BinkBufferUnlock@4");
		binkw32.Orig_BinkCheckCursor_20 = GetProcAddress(binkw32.hDll, "_BinkCheckCursor@20");
		binkw32.Orig_BinkClose_4 = GetProcAddress(binkw32.hDll, "_BinkClose@4");
		binkw32.Orig_BinkCloseTrack_4 = GetProcAddress(binkw32.hDll, "_BinkCloseTrack@4");
		binkw32.Orig_BinkCopyToBuffer_28 = GetProcAddress(binkw32.hDll, "_BinkCopyToBuffer@28");
		binkw32.Orig_BinkDDSurfaceType_4 = GetProcAddress(binkw32.hDll, "_BinkDDSurfaceType@4");
		binkw32.Orig_BinkDX8SurfaceType_4 = GetProcAddress(binkw32.hDll, "_BinkDX8SurfaceType@4");
		binkw32.Orig_BinkDoFrame_4 = GetProcAddress(binkw32.hDll, "_BinkDoFrame@4");
		binkw32.Orig_BinkGetError_0 = GetProcAddress(binkw32.hDll, "_BinkGetError@0");
		binkw32.Orig_BinkGetKeyFrame_12 = GetProcAddress(binkw32.hDll, "_BinkGetKeyFrame@12");
		binkw32.Orig_BinkGetRealtime_12 = GetProcAddress(binkw32.hDll, "_BinkGetRealtime@12");
		binkw32.Orig_BinkGetRects_8 = GetProcAddress(binkw32.hDll, "_BinkGetRects@8");
		binkw32.Orig_BinkGetSummary_8 = GetProcAddress(binkw32.hDll, "_BinkGetSummary@8");
		binkw32.Orig_BinkGetTrackData_8 = GetProcAddress(binkw32.hDll, "_BinkGetTrackData@8");
		binkw32.Orig_BinkGetTrackID_8 = GetProcAddress(binkw32.hDll, "_BinkGetTrackID@8");
		binkw32.Orig_BinkGetTrackMaxSize_8 = GetProcAddress(binkw32.hDll, "_BinkGetTrackMaxSize@8");
		binkw32.Orig_BinkGetTrackType_8 = GetProcAddress(binkw32.hDll, "_BinkGetTrackType@8");
		binkw32.Orig_BinkGoto_12 = GetProcAddress(binkw32.hDll, "_BinkGoto@12");
		binkw32.Orig_BinkIsSoftwareCursor_8 = GetProcAddress(binkw32.hDll, "_BinkIsSoftwareCursor@8");
		binkw32.Orig_BinkLogoAddress_0 = GetProcAddress(binkw32.hDll, "_BinkLogoAddress@0");
		binkw32.Orig_BinkNextFrame_4 = GetProcAddress(binkw32.hDll, "_BinkNextFrame@4");
		binkw32.Orig_BinkOpen_8 = GetProcAddress(binkw32.hDll, "_BinkOpen@8");
		binkw32.Orig_BinkOpenDirectSound_4 = GetProcAddress(binkw32.hDll, "_BinkOpenDirectSound@4");
		binkw32.Orig_BinkOpenMiles_4 = GetProcAddress(binkw32.hDll, "_BinkOpenMiles@4");
		binkw32.Orig_BinkOpenTrack_8 = GetProcAddress(binkw32.hDll, "_BinkOpenTrack@8");
		binkw32.Orig_BinkOpenWaveOut_4 = GetProcAddress(binkw32.hDll, "_BinkOpenWaveOut@4");
		binkw32.Orig_BinkPause_8 = GetProcAddress(binkw32.hDll, "_BinkPause@8");
		binkw32.Orig_BinkRestoreCursor_4 = GetProcAddress(binkw32.hDll, "_BinkRestoreCursor@4");
		binkw32.Orig_BinkService_4 = GetProcAddress(binkw32.hDll, "_BinkService@4");
		binkw32.Orig_BinkSetError_4 = GetProcAddress(binkw32.hDll, "_BinkSetError@4");
		binkw32.Orig_BinkSetFrameRate_8 = GetProcAddress(binkw32.hDll, "_BinkSetFrameRate@8");
		binkw32.Orig_BinkSetIO_4 = GetProcAddress(binkw32.hDll, "_BinkSetIO@4");
		binkw32.Orig_BinkSetIOSize_4 = GetProcAddress(binkw32.hDll, "_BinkSetIOSize@4");
		binkw32.Orig_BinkSetMixBins_8 = GetProcAddress(binkw32.hDll, "_BinkSetMixBins@8");
		binkw32.Orig_BinkSetPan_8 = GetProcAddress(binkw32.hDll, "_BinkSetPan@8");
		binkw32.Orig_BinkSetSimulate_4 = GetProcAddress(binkw32.hDll, "_BinkSetSimulate@4");
		binkw32.Orig_BinkSetSoundOnOff_8 = GetProcAddress(binkw32.hDll, "_BinkSetSoundOnOff@8");
		binkw32.Orig_BinkSetSoundSystem_8 = GetProcAddress(binkw32.hDll, "_BinkSetSoundSystem@8");
		binkw32.Orig_BinkSetSoundTrack_4 = GetProcAddress(binkw32.hDll, "_BinkSetSoundTrack@4");
		binkw32.Orig_BinkSetVideoOnOff_8 = GetProcAddress(binkw32.hDll, "_BinkSetVideoOnOff@8");
		binkw32.Orig_BinkSetVolume_8 = GetProcAddress(binkw32.hDll, "_BinkSetVolume@8");
		binkw32.Orig_BinkWait_4 = GetProcAddress(binkw32.hDll, "_BinkWait@4");
		binkw32.Orig_RADSetMemory_8 = GetProcAddress(binkw32.hDll, "_RADSetMemory@8");
		binkw32.Orig_RADTimerRead_0 = GetProcAddress(binkw32.hDll, "_RADTimerRead@0");
		binkw32.Orig_YUV_blit_16a1bpp_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_16a1bpp_52");
		binkw32.Orig_YUV_blit_16a1bpp_mask_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_16a1bpp_mask_52");
		binkw32.Orig_YUV_blit_16a4bpp_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_16a4bpp_52");
		binkw32.Orig_YUV_blit_16a4bpp_mask_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_16a4bpp_mask_52");
		binkw32.Orig_YUV_blit_16bpp_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_16bpp_48");
		binkw32.Orig_YUV_blit_16bpp_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_16bpp_mask_48");
		binkw32.Orig_YUV_blit_24bpp_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_24bpp_48");
		binkw32.Orig_YUV_blit_24bpp_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_24bpp_mask_48");
		binkw32.Orig_YUV_blit_24rbpp_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_24rbpp_48");
		binkw32.Orig_YUV_blit_24rbpp_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_24rbpp_mask_48");
		binkw32.Orig_YUV_blit_32abpp_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_32abpp_52");
		binkw32.Orig_YUV_blit_32abpp_mask_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_32abpp_mask_52");
		binkw32.Orig_YUV_blit_32bpp_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_32bpp_48");
		binkw32.Orig_YUV_blit_32bpp_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_32bpp_mask_48");
		binkw32.Orig_YUV_blit_32rabpp_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_32rabpp_52");
		binkw32.Orig_YUV_blit_32rabpp_mask_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_32rabpp_mask_52");
		binkw32.Orig_YUV_blit_32rbpp_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_32rbpp_48");
		binkw32.Orig_YUV_blit_32rbpp_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_32rbpp_mask_48");
		binkw32.Orig_YUV_blit_UYVY_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_UYVY_48");
		binkw32.Orig_YUV_blit_UYVY_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_UYVY_mask_48");
		binkw32.Orig_YUV_blit_YUY2_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_YUY2_48");
		binkw32.Orig_YUV_blit_YUY2_mask_48 = GetProcAddress(binkw32.hDll, "_YUV_blit_YUY2_mask_48");
		binkw32.Orig_YUV_blit_YV12_52 = GetProcAddress(binkw32.hDll, "_YUV_blit_YV12_52");
		binkw32.Orig_YUV_init_4 = GetProcAddress(binkw32.hDll, "_YUV_init_4");
		binkw32.Orig_radfree_4 = GetProcAddress(binkw32.hDll, "_radfree@4");
		binkw32.Orig_radmalloc_4 = GetProcAddress(binkw32.hDll, "_radmalloc@4");
		{
			// Load the plugins
			HMODULE main = GetModuleHandle(NULL);
			if (main == NULL)
				ERROR_BOX(L"Cannot get module handle of your exe.");
			wchar_t filename[MAX_PATH + 1 + _countof(PLUGINDIR PLUGINFILTER)];
			auto len = GetModuleFileName(main, filename, MAX_PATH);
			if (len <= 0)
				ERROR_BOX(L"Cannot get file name of your exe.");
			// get the path
			filename[len] = L'\0';
			for (--len; len >= 0; len--) {
				if (filename[len] == L'\\') {
					filename[++len] = L'\0';
					break;
				}
			}
			// change directory to PLUGINDIR and add PLUGINFILTER
			memcpy(&(filename[len]), PLUGINDIR PLUGINFILTER, sizeof(PLUGINDIR PLUGINFILTER));
			len += _countof(PLUGINDIR)-1; // len of the fully qualified plugin dir
			WIN32_FIND_DATA ffd;
			HANDLE hFind;
			hFind = FindFirstFile(filename, &ffd);
			if (hFind == INVALID_HANDLE_VALUE)
				ERROR_BOX(L"Cannot enumerate plugins in " PLUGINDIR);
			do {
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				// Due to some DOS compatibility FindFirstFile does match *.asi*.
				// So we use this to match to *.asi only.
				if (!PathMatchSpec(ffd.cFileName, PLUGINFILTER)) 
					continue;
				wcscpy_s(&(filename[len]), _countof(filename) - len, ffd.cFileName);
				HMODULE hMod = LoadLibrary(filename);
				if (hMod) {
					plugins.push_back(hMod);
				} else {
					const TCHAR fmt[] = L"Cannot load plugin\n%s" L"\n\nError Code %d."; 
					TCHAR buf[sizeof(fmt) + MAX_PATH + 32]; \
					swprintf_s(buf, fmt, filename, (DWORD)GetLastError()); \
					MessageBox(NULL, buf, L"ASI LOADER ERROR", MB_ICONEXCLAMATION | MB_OK);
				}
			} while (FindNextFile(hFind, &ffd) != 0);

			FindClose(hFind);
		}
		break;
	case DLL_PROCESS_DETACH:
		for (auto& mod : plugins)
			FreeLibrary(mod);
		if (binkw32.hDll)
			FreeLibrary(binkw32.hDll);
		break;
	}
	return TRUE;
}
