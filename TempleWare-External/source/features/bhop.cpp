#include "bhop.h"
#include "../offsets/offsets.h"
#include "../globals/globals.h"
#include <Windows.h>

namespace features {
    void Bhop::Run(const Memory& memory) noexcept {
        if (!globals::BunnyHopEnabled)
            return;

        const auto localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
        if (!localPlayer)
            return;

        static HWND hwndCs2 = FindWindowA(nullptr, "Counter-Strike 2");
        if (!hwndCs2)
            return;

        const bool spacePressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        const auto flags = memory.Read<int>(localPlayer + offsets::m_fFlags);
        const bool isOnGround = (flags & (1 << 0)) != 0;

        if (spacePressed) {
            if (isOnGround) {
                SendMessage(hwndCs2, WM_KEYDOWN, VK_SPACE, 0);
            }
            else {
                SendMessage(hwndCs2, WM_KEYUP, VK_SPACE, 0);
            }
        }
        else {
            SendMessage(hwndCs2, WM_KEYUP, VK_SPACE, 0);
        }
    }
}
