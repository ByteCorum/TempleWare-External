#include "hacks.h"
#include "../menu/gui.h"
#include "../offsets/globals.h"

#include <thread>

void hacks::MiscThread(const Memory& memory) noexcept
{
	while (gui::isRunning)
	{
		std::uint16_t desiredFov = (uint16_t)globals::FOV;
		std::uintptr_t localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
		std::uintptr_t CameraServices = memory.Read<std::uintptr_t>(localPlayer + offsets::m_pCameraServices); \

		std::uint16_t currentFov = memory.Read<std::uint64_t>(CameraServices + offsets::m_iFOV);

		bool isScoped = memory.Read<bool>(localPlayer + offsets::m_bIsScoped);

		if (!isScoped && currentFov != desiredFov)
		{
			memory.Write<uint64_t>(CameraServices + offsets::m_iFOV, desiredFov);
		}
	}
}

void hacks::VisualThread(const Memory& memory) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void triggerbot::TriggerBot()
{
	static uintptr_t localPlayer = memory.Read<uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
	static BYTE team = memory.Read<BYTE>(localPlayer + offsets::m_iTeamNum);
	static uintptr_t entityList = memory.Read<uintptr_t>(globals::client + offsets::dwEntityList);

	int crosshairEntityIndex = memory.Read<int>(localPlayer + offsets::m_iIDEntIndex);
	if (crosshairEntityIndex == 0)
		return;

	uintptr_t listEntry = memory.Read<uintptr_t>(entityList + 0x8 * (crosshairEntityIndex >> 9) + 0x10);
	uintptr_t entity = memory.Read<uintptr_t>(listEntry + 120 * (crosshairEntityIndex & 0x1ff));

	if (!entity)
		return;	
	if (!team == memory.Read<BYTE>(entity + offsets::m_iTeamNum))
		return;
	if (memory.Read<int>(entity + offsets::m_iHealth) <= 0)
		return;

	std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
	memory.Write<int>(globals::client + offsets::attack, 65537);
	std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
	memory.Write<int>(globals::client + offsets::attack, 256);
	std::this_thread::sleep_for(std::chrono::milliseconds(globals::TriggerBotDelay));
}

void hacks::AimThread(const Memory& memory) noexcept
{
	while (gui::isRunning)
	{
		if (globals::TriggerBot)
		{
			if (GetAsyncKeyState(VK_LSHIFT))
			{
				triggerbot::TriggerBot();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

