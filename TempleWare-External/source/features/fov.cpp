#include "fov.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"

namespace features {
    void FOVManager::AdjustFOV(const Memory& memory) noexcept {
        const auto localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
        if (!localPlayer) return;

        const auto cameraServices = memory.Read<std::uintptr_t>(localPlayer + offsets::m_pCameraServices);
        if (!cameraServices) return;

        const auto currentFov = memory.Read<std::uint16_t>(cameraServices + offsets::m_iFOV);
        const bool isScoped = memory.Read<bool>(localPlayer + offsets::m_bIsScoped);

        const auto desiredFov = static_cast<std::uint16_t>(globals::FOV);
        if (!isScoped && currentFov != desiredFov) {
            memory.Write<std::uint16_t>(cameraServices + offsets::m_iFOV, desiredFov);
        }
    }
}
