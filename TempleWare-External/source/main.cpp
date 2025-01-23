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

    const auto memory = Memory("cs2.exe");
    globals::client = memory.GetModuleAddress("client.dll");

    gui::CreateHWindow("templecheats.xyz");
    gui::CreateDevice();
    gui::CreateImGui();

    std::thread miscThread(threads::RunMiscThread, std::ref(memory));
    miscThread.detach();

    std::thread visualThread(threads::RunVisualThread, std::ref(memory));
    visualThread.detach();

    std::thread aimThread(threads::RunAimThread, std::ref(memory));
    aimThread.detach();

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
            gui::Render();
        else 
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}
