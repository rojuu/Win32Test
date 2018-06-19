#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <stdio.h>
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

static HWND global_hwnd;
static HDC global_hdc;
static HGLRC global_hrc;

static b32 global_running = true;

static f32 global_vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

static f32 global_colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

static const char* global_vertex_shader_source = R"FOO(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_col;

out vec3 frag_color;

void main()
{
   gl_Position = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
   frag_color = in_col;
}
)FOO";


static const char* global_fragment_shader_source = R"FOO(
#version 330 core
out vec4 out_color;
in vec3 frag_color;

void main()
{
   out_color = vec4(frag_color, 1.0f);
}
)FOO";

static u32 global_shader_program;
static u32 global_VAO;

static void
render_loop() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    glUseProgram(global_shader_program);
    glBindVertexArray(global_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0); // no need to unbind it every time

    SwapBuffers(global_hdc);
}

static void
compile_shaders() {
    i32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &global_vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    i32 success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        assert(!"ERROR::SHADER::VERTEX::COMPILATION_FAILED");
    }

    i32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &global_fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        assert(!"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    }

    global_shader_program = glCreateProgram();
    glAttachShader(global_shader_program, vertex_shader);
    glAttachShader(global_shader_program, fragment_shader);
    glLinkProgram(global_shader_program);
    glGetProgramiv(global_shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(global_shader_program, 512, NULL, infoLog);
        assert(!"ERROR::SHADER::PROGRAM::LINKING_FAILED");
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

static void
render_init() {
    compile_shaders();

    u32 vertex_buffer;
    glGenVertexArrays(1, &global_VAO);
    glGenBuffers(1, &vertex_buffer);
    glBindVertexArray(global_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(global_vertices), global_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), 0);
    glEnableVertexAttribArray(0);


    u32 color_buffer;
    glGenBuffers(1, &color_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(global_colors), global_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

LRESULT
WindowProc(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param) {
    PAINTSTRUCT ps;

    switch (message) {
        case WM_QUIT: {
            global_running = false;
        } break;

        case WM_PAINT: {
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        } break;

        case WM_SIZE: {
            glViewport(0, 0, LOWORD(l_param), HIWORD(l_param));
        } break;

        case WM_DESTROY: {
            global_running = false;
            return DefWindowProc(hwnd, message, w_param, l_param);
        } break;

        default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }

    return 0;
}

static i32
init_window_and_gl_context(HINSTANCE instance) {
    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszClassName = "TestWindowClass";

    if(!RegisterClass(&window_class)) {
        return -1;
    }

    HWND hwnd = CreateWindow(window_class.lpszClassName,
                             "Test",
                             WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, 512, 512,
                             NULL, NULL, instance, NULL);

    if(!hwnd) {
        return -1;
    }

    global_hwnd = hwnd;
    global_hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    i32 pfi = ChoosePixelFormat(global_hdc, &pfd);
    if(pfi == 0) {
        return -1;
    }

    if(!SetPixelFormat(global_hdc, pfi, &pfd)) {
        return -1;
    }

    HGLRC tempContext = wglCreateContext(global_hdc);
    wglMakeCurrent(global_hdc, tempContext);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        assert(!"GLEW could not be initialized!");
    }

    i32 attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    if(wglewIsSupported("WGL_ARB_create_context") == 1) {
        global_hrc = wglCreateContextAttribsARB(global_hdc,0, attribs);
        wglMakeCurrent(NULL,NULL);
        wglDeleteContext(tempContext);
        wglMakeCurrent(global_hdc, global_hrc);
    } else {
        assert(!"Coudln't create a GL 3.x context.");
        return -1;
    }

    if (!global_hrc) {
        return -1;
    }

    // Get GL version
    // i32 OpenGLVersion[2];
    // glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    // glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

    wglSwapIntervalEXT(0);

    return 0;
}

static void
process_pending_messages() {
     MSG msg;
     while(PeekMessage(&msg, global_hwnd, 0, 0, PM_REMOVE)) {
        switch(msg.message) {
            case WM_CLOSE: {
                global_running = false;
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 vk_code = (u32)msg.wParam;

                b32 was_down = ((msg.lParam & (1 << 30)) != 0);
                b32 is_down = ((msg.lParam & (1 << 31)) == 0);
                if(was_down != is_down) {
                    if(vk_code == VK_ESCAPE) {
                        global_running = false;
                    }
                }
            } break;

            default:  {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}

f64
query_seconds() {
    LARGE_INTEGER performance_counter;
    LARGE_INTEGER performance_frequency;
    b32 has_performance_counter = true;

    //TODO: These might fail sometimes. Handle failure states somehow.
    // Maybe use some lower res timer instead?
    QueryPerformanceCounter(&performance_counter);
    QueryPerformanceFrequency(&performance_frequency);

    f64 seconds = (f64)performance_counter.QuadPart / (f64)performance_frequency.QuadPart;
    return seconds;
}

i32
WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, i32 cmd_show) {
    if(init_window_and_gl_context(instance)) {
        assert(false);
        return -1;
    }

    render_init();

    ShowWindow(global_hwnd, SW_SHOW);
    UpdateWindow(global_hwnd);

    MSG msg;


    f64 current_time;
    f64 delta_time;
    f64 last_time;
    f64 last_fps_time = 0;
    current_time = query_seconds();

    u32 frame_counter = 0;
    u32 last_frame_count = 0;

    while(global_running) {
        last_time = current_time;
        current_time = query_seconds();
        delta_time = current_time - last_time;

        // Count frames for every second and print it as the title of the window
        ++frame_counter;
        if(current_time >= (last_fps_time + 1.0)) {
            last_fps_time  = current_time;
            u32 delta_frames = frame_counter - last_frame_count;
            last_frame_count  = frame_counter;
            char title[64];
            sprintf(title, "FPS: %lu", delta_frames);
            // SDL_SetWindowTitle(window, title);
            SetWindowTextA(global_hwnd, title);
        }

        process_pending_messages();
        render_loop();
    }

    // wglMakeCurrent(NULL, NULL);
    // ReleaseDC(global_hwnd, global_hdc);
    // wglDeleteContext(global_hrc);
    // DestroyWindow(global_hwnd);

    return 0;
}
