#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>

class Memory {
private:
	std::uintptr_t processId_;
	HANDLE processHandle_;

public:
	Memory(const std::string& processName) noexcept
	{
		PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (Process32Next(snapshot, &entry)) 
		{
			if (entry.szExeFile == processName) 
			{
				processId_ = entry.th32ProcessID;
				processHandle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId_);
				break;
			}
		}

		CloseHandle(snapshot);
	}

	~Memory() noexcept 
	{
		if (processHandle_)
			CloseHandle(processHandle_);
	}

	std::uintptr_t GetModuleAddress(const std::string& moduleName) const noexcept
	{
		MODULEENTRY32 entry = { sizeof(MODULEENTRY32) };
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId_);
		std::uintptr_t result = 0;

		while (Module32Next(snapshot, &entry)) 
		{
			if (entry.szModule == moduleName) 
			{
				result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		CloseHandle(snapshot);
		return result;
	}

	template <typename T>
	T Read(const std::uintptr_t& address) const noexcept 
	{
		T value;
		SIZE_T bytesRead;
		ReadProcessMemory(processHandle_, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead);
		return value;
	}

	template <typename T>
	void Write(const std::uintptr_t& address, const T& value) const noexcept
	{
		SIZE_T bytesWritten;
		WriteProcessMemory(processHandle_, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytesWritten);
	}
};