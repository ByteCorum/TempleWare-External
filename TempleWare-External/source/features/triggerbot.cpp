#include "TriggerBot.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>
#include <Windows.h> 

namespace features 
{
    void TriggerBot::Run(const Memory& memory) noexcept {
        while (globals::isRunning) 
        {
            if (!globals::TriggerBot) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            bool keyDown = (GetAsyncKeyState(globals::TriggerBotKey) & 0x8000) != 0;
            if (globals::TriggerBotMode == 0) 
            {
                if (!keyDown) 
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }
            else if (globals::TriggerBotMode == 1) 
            {
                if (keyDown) 
                {
                    globals::TriggerBotToggled = !globals::TriggerBotToggled;
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

                if (!globals::TriggerBotToggled) 
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }

            std::uintptr_t localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
            BYTE team = memory.Read<BYTE>(localPlayer + offsets::m_iTeamNum);

            if (!globals::TriggerBotIgnoreFlash) 
            {
                float flashDuration = memory.Read<float>(localPlayer + offsets::flFlashDuration);
                if (flashDuration > 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }

            int crosshairEntityIndex = memory.Read<int>(localPlayer + offsets::m_iIDEntIndex);
            if (crosshairEntityIndex == 0)
                continue;

            std::uintptr_t entityList = memory.Read<std::uintptr_t>(globals::client + offsets::dwEntityList);
            std::uintptr_t entity = memory.Read<std::uintptr_t>(memory.Read<std::uintptr_t>(entityList + 0x8 * (crosshairEntityIndex >> 9) + 0x10) + 120 * (crosshairEntityIndex & 0x1ff));
            if (!entity)
                continue;

            // Skip teammate or dead target (optional)
            if (globals::TriggerBotTeamCheck && team == memory.Read<BYTE>(entity + offsets::m_iTeamNum))
                continue;

            if (memory.Read<int>(entity + offsets::m_iHealth) <= 0)
                continue;

            // Simulate shooting
            memory.Write<int>(globals::client + offsets::attack, 65537); // Set attack command
            std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
            memory.Write<int>(globals::client + offsets::attack, 256);   // Clear attack command
            std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
        }
    }
}