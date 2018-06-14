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

#include <stdio.h>
LRESULT
WindowProc(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param) {
    PAINTSTRUCT ps;
    HDC hdc;
    char* greeting = "Hello, World!";
    switch (message) {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            // Here your application is laid out.
            // For this introduction, we just print out "Hello, World!"
            // in the top left corner.
            TextOutA(hdc, 5, 5, greeting, strlen(greeting));
            // End application-specific layout section.

            EndPaint(hwnd, &ps);
            break;
        //case WM_DESTROY:
        // TODO: We might want to do something if our window is destroyed:
        default:
            return DefWindowProc(hwnd, message, w_param, l_param);
    }

    return 0;
}

i32
WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, i32 cmd_show) {
    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszClassName = "TestWindowClass";

    if(!RegisterClass(&window_class)) {
        assert(false);
        return -1;
    }

    HWND hwnd = CreateWindow(window_class.lpszClassName, // Class name
                             "Test", // Window name
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 512, 512,
                             NULL, NULL, instance, NULL);

    if(!hwnd) {
        assert(false);
        return -1;
    }

    ShowWindow(hwnd, cmd_show);
    UpdateWindow(hwnd);

    MSG msg;
    for(;;) {
        BOOL result = GetMessage(&msg, hwnd, 0, 0);
        if(result > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            break;
        }
    }

    return 0;
}
