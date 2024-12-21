#include "glow.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>

namespace features {
    void Glow::Run(const Memory& memory) noexcept {
        if (!globals::Glow) {
            return;
        }

        const auto localPlayerController = memory.Read<uintptr_t>(globals::client + offsets::dwLocalPlayerController);
        if (!localPlayerController) {
            return;
        }

        const int localTeam = memory.Read<int>(localPlayerController + offsets::m_iTeamNum);
        const auto entityListBase = memory.Read<uintptr_t>(globals::client + offsets::dwEntityList);
        if (!entityListBase) {
            return;
        }

        for (int i = 1; i < 64; ++i) {
            const auto listEntry = memory.Read<uintptr_t>(entityListBase + ((i & 0x7FFF) >> 9) * 8 + 16);
            if (!listEntry) {
                continue;
            }

            const auto player = memory.Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
            if (!player) {
                continue;
            }

            const int playerTeam = memory.Read<int>(player + offsets::m_iTeamNum);
            if (playerTeam == localTeam) {
                continue;
            }

            const auto playerPawn = memory.Read<uint32_t>(player + offsets::m_hPlayerPawn);
            const auto listEntry2 = memory.Read<uintptr_t>(entityListBase + ((playerPawn & 0x7FFF) >> 9) * 8 + 16);
            if (!listEntry2) {
                continue;
            }

            const auto playerCsPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawn & 0x1FF));
            if (!playerCsPawn) {
                continue;
            }

            // Convert ImVec4 (RGBA) to ARGB DWORD
            const auto& color = globals::GlowColor;
            const DWORD colorArgb =
                (static_cast<DWORD>(color.w * 255) << 24) |
                (static_cast<DWORD>(color.x * 255)) |
                (static_cast<DWORD>(color.y * 255) << 8) |
                (static_cast<DWORD>(color.z * 255) << 16);

            // Apply glow settings
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_glowColorOverride, colorArgb);
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_bGlowing, 1);
        }
    }
}
