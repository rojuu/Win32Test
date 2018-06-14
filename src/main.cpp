#include <windows.h>
#include <assert.h>

#include <stdint.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

b32 RUNNING = true;

LRESULT
WindowProc(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param) {
    PAINTSTRUCT ps;
    HDC hdc;
    char greeting[] = _T("Hello, World!");
    switch (message) {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            // Here your application is laid out.
            // For this introduction, we just print out "Hello, World!"
            // in the top left corner.
            TextOutA(hdc,
                    5, 5,
                    greeting, strlen(greeting));
            // End application-specific layout section.

            EndPaint(hwnd, &ps);
            break;
        case WM_DESTROY:
            RUNNING = false;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, w_param, l_param);
    }

    return 0;
}

i32
WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, i32 cmd_show) {
    WNDCLASS window_class;
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = instance;
    window_class.hIcon = 0;
    window_class.hCursor = 0;
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszMenuName = 0;
    window_class.lpszClassName = "TestWindowClass";

    if(!RegisterClass(&window_class)) {
        assert(false);
        return -1;
    }

    HWND hwnd = CreateWindow("TestWindowClass", // Class name
                             "Test", // Window name
                             WS_OVERLAPPED|WS_CAPTION|WS_BORDER|
                                WS_SYSMENU|WS_MINIMIZEBOX,//|WS_MAXIMIZEBOX|, // Style params
                             CW_USEDEFAULT, CW_USEDEFAULT, //0, 0, // Position
                             512, 512, // Size
                             NULL, // Parent
                             NULL, // Menu
                             instance, // Instance
                             NULL); // Param
    if(!hwnd) {
        assert(false);
        return -1;
    }

    ShowWindow(hwnd, cmd_show);
    UpdateWindow(hwnd);

    while(RUNNING) {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}
