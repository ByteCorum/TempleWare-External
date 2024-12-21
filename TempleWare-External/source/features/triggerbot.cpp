#include "TriggerBot.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>
#include <Windows.h>

namespace features {
    void TriggerBot::Run(const Memory& memory) noexcept {
        while (globals::isRunning) {
            if (!globals::TriggerBot) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            bool keyState = GetAsyncKeyState(globals::TriggerBotKey) & 0x8000;

            switch (globals::TriggerBotMode) {
            case 0: 
                if (!keyState) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                break;

            case 1: 
                if (keyState) {
                    globals::TriggerBotToggled = !globals::TriggerBotToggled;
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

                if (!globals::TriggerBotToggled) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                break;

            default:
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            auto localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
            if (!localPlayer) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            auto team = memory.Read<BYTE>(localPlayer + offsets::m_iTeamNum);

            if (!globals::TriggerBotIgnoreFlash) {
                auto flashDuration = memory.Read<float>(localPlayer + offsets::flFlashDuration);
                if (flashDuration > 0.0f) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }

            int crosshairEntityIndex = memory.Read<int>(localPlayer + offsets::m_iIDEntIndex);
            if (crosshairEntityIndex == 0) {
                continue;
            }

            auto entityList = memory.Read<std::uintptr_t>(globals::client + offsets::dwEntityList);
            auto listEntry = memory.Read<std::uintptr_t>(entityList + 0x8 * (crosshairEntityIndex >> 9) + 0x10);
            auto entity = memory.Read<std::uintptr_t>(listEntry + 120 * (crosshairEntityIndex & 0x1ff));

            if (!entity) {
                continue;
            }

            if (globals::TriggerBotTeamCheck && team == memory.Read<BYTE>(entity + offsets::m_iTeamNum)) {
                continue;
            }

            if (memory.Read<int>(entity + offsets::m_iHealth) <= 0) {
                continue;
            }

            memory.Write<int>(globals::client + offsets::attack, 65537);
            std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
            memory.Write<int>(globals::client + offsets::attack, 256);

            std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
        }
    }
}
