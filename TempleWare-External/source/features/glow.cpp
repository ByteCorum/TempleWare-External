#include "glow.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>

namespace features {
    void Glow::Run(const Memory& memory) noexcept 
    {
        if (!globals::Glow) return;

        const uintptr_t localPlayerController = memory.Read<uintptr_t>(globals::client + offsets::dwLocalPlayerController);
        if (!localPlayerController) return;

        const int localTeam = memory.Read<int>(localPlayerController + offsets::m_iTeamNum);
        const uintptr_t entityList = memory.Read<uintptr_t>(globals::client + offsets::dwEntityList);
        if (!entityList) return;

        for (int i = 1; i < 64; ++i) {
            const uintptr_t listEntry = memory.Read<uintptr_t>(entityList + ((8 * (i & 0x7FFF)) >> 9) + 16);
            if (!listEntry) continue;

            const uintptr_t player = memory.Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
            if (!player) continue;

            const int playerTeam = memory.Read<int>(player + offsets::m_iTeamNum);
            if (playerTeam == localTeam) continue;

            const uint32_t playerPawn = memory.Read<uint32_t>(player + offsets::m_hPlayerPawn);
            if (!playerPawn) continue;

            const uintptr_t listEntry2 = memory.Read<uintptr_t>(entityList + ((8 * (playerPawn & 0x7FFF)) >> 9) + 16);
            if (!listEntry2) continue;

            const uintptr_t playerCsPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawn & 0x1FF));
            if (!playerCsPawn) continue;

            const int health = memory.Read<int>(playerCsPawn + offsets::m_iHealth);
            if (health < 1) continue;

            const ImVec4& color = globals::GlowColor;
            const DWORD colorArgb = (
                (static_cast<DWORD>(color.w * 255) << 24) |
                (static_cast<DWORD>(color.z * 255) << 16) |
                (static_cast<DWORD>(color.y * 255) << 8) |
                static_cast<DWORD>(color.x * 255)
                );

            const uintptr_t glowOffset = playerCsPawn + offsets::m_Glow;
            memory.Write<DWORD64>(glowOffset + offsets::m_glowColorOverride, colorArgb);
            memory.Write<DWORD64>(glowOffset + offsets::m_bGlowing, 1);
        }
    }
}
