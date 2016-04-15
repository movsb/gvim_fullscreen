#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0502
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
    char ClassName[128];
    ClassName[GetClassNameA(hwnd, &ClassName[0], sizeof(ClassName)/sizeof(*ClassName))] = '\0';
    if(strcmp(ClassName, "Vim") == 0) {
        HWND* phVim = reinterpret_cast<HWND*>(lParam);
        *phVim = hwnd;
        return FALSE;
    }
    else {
        return TRUE;
    }
}

extern "C" __declspec(dllexport) int __cdecl ToggleFullscreen(int alpha) {
    HWND hWnd = NULL;
    if(EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, LPARAM(&hWnd)))
        hWnd = NULL;
    if(hWnd != NULL) {
        RECT* r;
        // 0: non full-screen, 1: full-screen
        switch(reinterpret_cast<int>(GetPropA(hWnd, "__full_state__")))
        {
        case 0:
            if(!(r = reinterpret_cast<RECT*>(GetPropA(hWnd, "__window_rect__")))) {
                r = (RECT*)HeapAlloc(GetProcessHeap(), 0, sizeof(RECT));
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            }

            GetWindowRect(hWnd, r);
            SetPropA(hWnd, "__window_rect__", HANDLE(r));

            SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));

            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
            SetLayeredWindowAttributes(hWnd, 0, (BYTE)(alpha <= 0 ? 30 : alpha), LWA_ALPHA);

            SetPropA(hWnd, "__full_state__", HANDLE(1));
            break;
        case 1:
            r = reinterpret_cast<RECT*>(GetPropA(hWnd, "__window_rect__"));

            SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)COLOR_BTNFACE);

            SetWindowPos(hWnd, HWND_NOTOPMOST, r->left, r->top, r->right-r->left, r->bottom-r->top, SWP_FRAMECHANGED);
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

            SetPropA(hWnd, "__full_state__", HANDLE(0));
            break;
        }
    }

    return 0;
}
