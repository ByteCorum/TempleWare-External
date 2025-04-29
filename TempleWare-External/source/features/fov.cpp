#include "fov.h"
#include "../globals/globals.h"
#include "../offsets/offsets.h"
#include <thread>

namespace features 
{
    void FOVManager::AdjustFOV(const Memory& memory) noexcept
    {
        if (globals::lastFOV != globals::FOV)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            globals::lastFOV = globals::FOV;
        }

        std::uintptr_t localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
        if (!localPlayer)
            return;

        std::uintptr_t cameraServices = memory.Read<std::uintptr_t>(localPlayer + offsets::m_pCameraServices);
        if (!cameraServices)
            return;

        std::uint16_t currentFov = memory.Read<std::uint16_t>(cameraServices + offsets::m_iFOV);
        bool isScoped = memory.Read<bool>(localPlayer + offsets::m_bIsScoped);
        std::uint16_t desiredFov = static_cast<uint16_t>(globals::lastFOV);


        if (!isScoped && currentFov != desiredFov && globals::lastFOV != globals::writtenFOV)
        {
           // do
           // {
           memory.Write<uint16_t>(cameraServices + offsets::m_iFOV, desiredFov);
                //std::this_thread::sleep_for(std::chrono::milliseconds(5));
               // currentFov = memory.Read<std::uint16_t>(cameraServices + offsets::m_iFOV);
           // } while(currentFov != desiredFov);

            globals::writtenFOV = globals::lastFOV;
        }

    }
}
