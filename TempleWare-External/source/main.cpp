/************************************************************************************************
 *                                          Temple Cheat                                        *
 *                     (c) 2023-2024 Temple Client Cheats Team. All rights reserved.            *
 ************************************************************************************************/

#include "menu/menu.h"
#include "memory/memory.h"
#include "globals/globals.h"
#include "threads/threads.h"
#include "offsets/offsets.h"
#include <thread>

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow) 
{
    if (!offsets::UpdateOffset())
        return EXIT_FAILURE;

    Memory memory("cs2.exe");
    globals::client = memory.GetModuleAddress("client.dll");

    std::vector<std::thread> threads;
    threads.emplace_back(threads::RunMiscThread, std::ref(memory));
    threads.emplace_back(threads::RunVisualThread, std::ref(memory));
    threads.emplace_back(threads::RunAimThread, std::ref(memory));

    for (auto& thread : threads)
        thread.detach();

    gui::CreateHWindow("templecheats.xyz");
    gui::CreateDevice();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

    bool windowVisible = true;
    while (globals::isRunning)
    {
        if (GetAsyncKeyState(VK_END) & 0x8000) 
        {
            windowVisible = !windowVisible;
            ShowWindow(gui::window, windowVisible ? SW_SHOW : SW_HIDE);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (windowVisible) 
        {
            gui::BeginRender();
            gui::Render();
            ImGui::Render();
            gui::EndRender();
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ImGui::DestroyContext();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}