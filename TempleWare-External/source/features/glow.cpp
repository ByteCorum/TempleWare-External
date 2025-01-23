#include "glow.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>

namespace features
{

    uintptr_t GetEntityInListOffset(int index) 
    {
        return 0x8 * (index & 0x7FFF) >> 9 + 16;
    }

    DWORD PackColor(const ImVec4& color) 
    {
        return ((DWORD)(color.w * 255) << 24) |
            ((DWORD)(color.z * 255) << 16) |
            ((DWORD)(color.y * 255) << 8) |
            (DWORD)(color.x * 255);
    }

    void Glow::Run(const Memory& memory) noexcept
    {
        if (!globals::Glow)
            return;

        const uintptr_t localPlayerController = memory.Read<uintptr_t>(globals::client + offsets::dwLocalPlayerController);
        if (!localPlayerController)
            return;

        int localTeam = memory.Read<int>(localPlayerController + offsets::m_iTeamNum);


        for (int i = 1; i < 64; ++i) 
        {
            uintptr_t entityList = memory.Read<uintptr_t>(globals::client + offsets::dwEntityList);
            if (!entityList)
                continue;

            uintptr_t entity = memory.Read<uintptr_t>(entityList + GetEntityInListOffset(i));
            if (!entity)
                continue;

            int entityTeam = memory.Read<int>(entity + offsets::m_iTeamNum);
            if (entityTeam == localTeam)
                continue;

            uint32_t playerPawn = memory.Read<uint32_t>(entity + offsets::m_hPlayerPawn);
            if (!playerPawn)
                continue;

            uintptr_t playerCsPawn = memory.Read<uintptr_t>(entityList + GetEntityInListOffset(playerPawn));
            if (!playerCsPawn)
                continue;

            int health = memory.Read<int>(playerCsPawn + offsets::m_iHealth);
            if (health < 1)
                continue;



            ImVec4 color = globals::GlowColor;
            DWORD colorArgb = PackColor(color);
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_glowColorOverride, colorArgb);
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_bGlowing, 1);
        }
    }
}