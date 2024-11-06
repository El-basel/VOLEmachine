#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "vole.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << error << ' ' << description << std::endl;
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
    {
        return 1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1300,820,"Hello world", nullptr, nullptr);
    if(window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    bool showDemoWindow = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf",15);

    /*********************************************/
    // Machine requirements
    bool enterFileName = false;
    MainUI mainUI;
    /*********************************************/
    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Load Program"))
                {
                    enterFileName = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        ImGui::SetNextWindowPos(ImVec2(0.0f,18.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x,io.DisplaySize.y/2));
        {
            ImGui::Begin("Memory",NULL, window_flags);
            ImGui::Text("Memory Cells");
            mainUI.displayMenu(MachineFlags::memory);
            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(0.0f,io.DisplaySize.y/2 + 18.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/3,io.DisplaySize.y/2));
        {
            ImGui::Begin("Options", NULL, window_flags);
            if(ImGui::Button("Enter Instruction"))
            {
                mainUI.insertInstruction(true);
            }
            mainUI.insertInstruction(false);
            if(mainUI.checkProgramRunning())
            {
                if(ImGui::Button("Execute Instruction"))
                {
                    mainUI.displayMenu(MachineFlags::execute);
                }
                if(ImGui::Button("Halt Program"))
                {
                    mainUI.displayMenu(MachineFlags::halt);
                }
            }
            else
            {
                ImGui::Text("Load a new program from the main menu bar");
            }
            mainUI.inputFileName(enterFileName);
            if(mainUI.inputError)
            {
                ImGui::OpenPopup("Error");
            }
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {

                ImGui::Text("Error Loading Instructions");

                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                    mainUI.inputError = false;
                }
                ImGui::EndPopup();
            }
            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x/3,io.DisplaySize.y/2 + 18.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/3,io.DisplaySize.y/2));
        {
            ImGui::Begin("IR and PC",NULL, window_flags);
            mainUI.displayMenu(MachineFlags::IrAndPc);
            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(2 * io.DisplaySize.x/3,io.DisplaySize.y/2 + 18.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/3,io.DisplaySize.y/2));
        {
            ImGui::Begin("Registers", NULL, window_flags);
            mainUI.displayMenu(MachineFlags::registers);
            ImGui::End();
        }


//        ImGui::ShowDemoWindow(&showDemoWindow);

        ImGui::Render();
        int display_width, display_height;
        glfwGetFramebufferSize(window, &display_width, &display_height);
        glViewport(0,0,display_width,display_height);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);
    }
    return 0;
}
