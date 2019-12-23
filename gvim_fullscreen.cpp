#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0502
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
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

extern "C" __declspec(dllexport) int __cdecl ToggleTransparency(const char* args) {
    HWND hWnd = NULL;
    if(EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, LPARAM(&hWnd)))
        hWnd = NULL;
    if(hWnd != NULL) {
        int newval = 0;
        int t1, t2;
        if(sscanf(args, "%d,%d", &t1, &t2) == 2) {
            int current = reinterpret_cast<int>(GetPropA(hWnd, "__transparency__"));
            if (current == 0)
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

            if (current == 0 || current == t2)
                newval = t1;
            else
                newval = t2;

            SetLayeredWindowAttributes(hWnd, 0, (BYTE)newval, LWA_ALPHA);
            SetPropA(hWnd, "__transparency__", HANDLE(newval));
        }
    }

    return 0;
}

static BOOL CALLBACK EnumMonitor(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param)
{
    MONITORINFO *pmi = reinterpret_cast<MONITORINFO*>(param);
    GetMonitorInfo(handle, pmi);
    return true;
}

extern "C" __declspec(dllexport) int __cdecl ToggleFullscreen(int) {
    HWND hWnd = NULL;
    if(EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, LPARAM(&hWnd)))
        hWnd = NULL;
    if(hWnd != NULL) {
        RECT* r;
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        MONITORINFO *pmi = &mi;
        // 0: non full-screen, 1: full-screen
        switch(reinterpret_cast<int>(GetPropA(hWnd, "__full_state__")))
        {
        case 0:
            if(!(r = reinterpret_cast<RECT*>(GetPropA(hWnd, "__window_rect__")))) {
                r = (RECT*)HeapAlloc(GetProcessHeap(), 0, sizeof(RECT));

                // remove clientedge for vim textarea
                HWND hTextArea = FindWindowEx(hWnd, NULL, "VimTextArea", "Vim text area");
                DWORD dwExStyle = GetWindowLongPtr(hTextArea, GWL_EXSTYLE);
                SetWindowLongPtr(hTextArea, GWL_EXSTYLE, dwExStyle & ~WS_EX_CLIENTEDGE);
            }

            GetWindowRect(hWnd, r);
            SetPropA(hWnd, "__window_rect__", HANDLE(r));

            SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));

            EnumDisplayMonitors(NULL, r, EnumMonitor, reinterpret_cast<LPARAM>(pmi));
            SetWindowPos(hWnd, HWND_TOP, pmi->rcMonitor.left, pmi->rcMonitor.top, pmi->rcMonitor.right - pmi->rcMonitor.left, pmi->rcMonitor.bottom - pmi->rcMonitor.top, SWP_FRAMECHANGED);

            SetPropA(hWnd, "__full_state__", HANDLE(1));
            break;
        case 1:
            SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)COLOR_BTNFACE);

            r = reinterpret_cast<RECT*>(GetPropA(hWnd, "__window_rect__"));
            SetWindowPos(hWnd, HWND_TOP, r->left, r->top, r->right-r->left, r->bottom-r->top, SWP_FRAMECHANGED);

            SetPropA(hWnd, "__full_state__", HANDLE(0));
            break;
        }
    }

    return 0;
}

