#ifndef __EMSCRIPTEN__
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif
#include <stdbool.h>

// __declspec( dllexport )
void hide_window()
{
#ifndef __EMSCRIPTEN__
    GLFWwindow* win = glfwGetCurrentContext();
    glfwHideWindow(win);
    SetWindowLong(glfwGetWin32Window(win), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
#endif
}

void show_window()
{
#ifndef __EMSCRIPTEN__
    GLFWwindow* win = glfwGetCurrentContext();
    glfwShowWindow(win);
    SetWindowLong(glfwGetWin32Window(win), GWL_EXSTYLE, WS_EX_APPWINDOW);
#endif
}

void always_on_top(bool isAlways_on_top)
{
#ifndef __EMSCRIPTEN__
    glfwSetWindowAttrib(glfwGetCurrentContext(), GLFW_FLOATING, isAlways_on_top);
#endif
}
