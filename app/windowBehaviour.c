#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdbool.h>

// __declspec( dllexport )
void hide_window()
{
    GLFWwindow* win = glfwGetCurrentContext();
    glfwHideWindow(win);
    SetWindowLong(glfwGetWin32Window(win), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
}

void show_window()
{
    GLFWwindow* win = glfwGetCurrentContext();
    glfwShowWindow(win);
    SetWindowLong(glfwGetWin32Window(win), GWL_EXSTYLE, WS_EX_APPWINDOW);
}

void always_on_top(bool isAlways_on_top)
{
    glfwSetWindowAttrib(glfwGetCurrentContext(), GLFW_FLOATING, isAlways_on_top);
}
