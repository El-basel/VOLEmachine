#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "vole.h"
int mainUI(std::string& fileName) {
    MainUI menu;
    std::cout << "----------------------------\n";
    std::cout << "| VOLE machine powering up |\n";
    std::cout << "----------------------------\n";
    while(menu.displayMenu())
    {

    }
    return 0;
}