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
//
// The purpose of this project is to generate an import lib for the victim dll. 
// The proxy dll will use it to load the victim dll.
// We use the same module.def for both dlls. This way we dont have to 
// GetProcAddress each function and we may more easily switch the proxy victim
// to another lib by providing a different import lib. Note that the dll generated
// by this project is a byproduct and not needed. Therefore we just create stub
// definitions. The following code does just that.

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

// return line to prevent optimization (prevent pointing different symbols to the same definition)
#define PROXY_BODY { return (void*)__LINE__; } 
#define STDCALL_DECORATION extern "C" void* __stdcall 

#define STDCALL_PROXY_0(name) STDCALL_DECORATION name() PROXY_BODY
#define STDCALL_PROXY_4(name) STDCALL_DECORATION name(void *a) PROXY_BODY
#define STDCALL_PROXY_8(name) STDCALL_DECORATION name(void *a, void *b) PROXY_BODY
#define STDCALL_PROXY_12(name) STDCALL_DECORATION name(void *a, void *b, void *c) PROXY_BODY
#define STDCALL_PROXY_16(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d) PROXY_BODY
#define STDCALL_PROXY_20(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e) PROXY_BODY
#define STDCALL_PROXY_24(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f) PROXY_BODY
#define STDCALL_PROXY_28(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g) PROXY_BODY
#define STDCALL_PROXY_32(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h) PROXY_BODY
#define STDCALL_PROXY_36(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i) PROXY_BODY
#define STDCALL_PROXY_48(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i, void *j, void *k, void *l) PROXY_BODY
#define STDCALL_PROXY_52(name) STDCALL_DECORATION name(void *a, void *b, void *c, void *d, void *e, void* f, void *g, void *h, void *i, void *j, void *k, void *l, void *m) PROXY_BODY

STDCALL_PROXY_12(BinkBufferBlit)
STDCALL_PROXY_12(BinkBufferCheckWinPos)
STDCALL_PROXY_8(BinkBufferClear)
STDCALL_PROXY_4(BinkBufferClose)
STDCALL_PROXY_4(BinkBufferGetDescription)
STDCALL_PROXY_0(BinkBufferGetError)
STDCALL_PROXY_4(BinkBufferLock)
STDCALL_PROXY_16(BinkBufferOpen)
STDCALL_PROXY_8(BinkBufferSetDirectDraw)
STDCALL_PROXY_8(BinkBufferSetHWND)
STDCALL_PROXY_12(BinkBufferSetOffset)
STDCALL_PROXY_12(BinkBufferSetResolution)
STDCALL_PROXY_12(BinkBufferSetScale)
STDCALL_PROXY_4(BinkBufferUnlock)
STDCALL_PROXY_20(BinkCheckCursor)
STDCALL_PROXY_4(BinkClose)
STDCALL_PROXY_4(BinkCloseTrack)
STDCALL_PROXY_28(BinkCopyToBuffer)
STDCALL_PROXY_4(BinkDDSurfaceType)
STDCALL_PROXY_4(BinkDX8SurfaceType)
STDCALL_PROXY_4(BinkDoFrame)
STDCALL_PROXY_0(BinkGetError)
STDCALL_PROXY_12(BinkGetKeyFrame)
STDCALL_PROXY_12(BinkGetRealtime)
STDCALL_PROXY_8(BinkGetRects)
STDCALL_PROXY_8(BinkGetSummary)
STDCALL_PROXY_8(BinkGetTrackData)
STDCALL_PROXY_8(BinkGetTrackID)
STDCALL_PROXY_8(BinkGetTrackMaxSize)
STDCALL_PROXY_8(BinkGetTrackType)
STDCALL_PROXY_12(BinkGoto)
STDCALL_PROXY_8(BinkIsSoftwareCursor)
STDCALL_PROXY_0(BinkLogoAddress)
STDCALL_PROXY_4(BinkNextFrame)
STDCALL_PROXY_8(BinkOpen)
STDCALL_PROXY_4(BinkOpenDirectSound)
STDCALL_PROXY_4(BinkOpenMiles)
STDCALL_PROXY_8(BinkOpenTrack)
STDCALL_PROXY_4(BinkOpenWaveOut)
STDCALL_PROXY_8(BinkPause)
STDCALL_PROXY_4(BinkRestoreCursor)
STDCALL_PROXY_4(BinkService)
STDCALL_PROXY_4(BinkSetError)
STDCALL_PROXY_8(BinkSetFrameRate)
STDCALL_PROXY_4(BinkSetIO)
STDCALL_PROXY_4(BinkSetIOSize)
STDCALL_PROXY_8(BinkSetMixBins)
STDCALL_PROXY_8(BinkSetPan)
STDCALL_PROXY_4(BinkSetSimulate)
STDCALL_PROXY_8(BinkSetSoundOnOff)
STDCALL_PROXY_8(BinkSetSoundSystem)
STDCALL_PROXY_4(BinkSetSoundTrack)
STDCALL_PROXY_8(BinkSetVideoOnOff)
STDCALL_PROXY_8(BinkSetVolume)
STDCALL_PROXY_4(BinkWait)
STDCALL_PROXY_8(RADSetMemory)
STDCALL_PROXY_0(RADTimerRead)
STDCALL_PROXY_52(YUV_blit_16a1bpp)
STDCALL_PROXY_52(YUV_blit_16a1bpp_mask)
STDCALL_PROXY_52(YUV_blit_16a4bpp)
STDCALL_PROXY_52(YUV_blit_16a4bpp_mask)
STDCALL_PROXY_48(YUV_blit_16bpp)
STDCALL_PROXY_48(YUV_blit_16bpp_mask)
STDCALL_PROXY_48(YUV_blit_24bpp)
STDCALL_PROXY_48(YUV_blit_24bpp_mask)
STDCALL_PROXY_48(YUV_blit_24rbpp)
STDCALL_PROXY_48(YUV_blit_24rbpp_mask)
STDCALL_PROXY_52(YUV_blit_32abpp)
STDCALL_PROXY_52(YUV_blit_32abpp_mask)
STDCALL_PROXY_48(YUV_blit_32bpp)
STDCALL_PROXY_48(YUV_blit_32bpp_mask)
STDCALL_PROXY_52(YUV_blit_32rabpp)
STDCALL_PROXY_52(YUV_blit_32rabpp_mask)
STDCALL_PROXY_48(YUV_blit_32rbpp)
STDCALL_PROXY_48(YUV_blit_32rbpp_mask)
STDCALL_PROXY_48(YUV_blit_UYVY)
STDCALL_PROXY_48(YUV_blit_UYVY_mask)
STDCALL_PROXY_48(YUV_blit_YUY2)
STDCALL_PROXY_48(YUV_blit_YUY2_mask)
STDCALL_PROXY_52(YUV_blit_YV12)
STDCALL_PROXY_4(YUV_init)
STDCALL_PROXY_4(radfree)
STDCALL_PROXY_4(radmalloc)

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {
    return TRUE;
}

