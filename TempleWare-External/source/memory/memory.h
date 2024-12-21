#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>

class Memory {
private:
    std::uintptr_t process_id_;  // Use more descriptive variable names
    void* process_handle_;

public:
    Memory(const std::string_view& process_name) noexcept {
        PROCESSENTRY32 entry = {};
        entry.dwSize = sizeof(PROCESSENTRY32);
        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (!snapshot) {
            // Handle error
            return;
        }

        while (Process32Next(snapshot, &entry)) {
            if (!process_name.compare(entry.szExeFile)) {
                process_id_ = entry.th32ProcessID;
                process_handle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id_);
                break;
            }
        }

        CloseHandle(snapshot);
    }

    ~Memory() {
        if (process_handle_) {
            CloseHandle(process_handle_);
        }
    }

    std::uintptr_t GetModuleAddress(const std::string_view& module_name) const noexcept {
        MODULEENTRY32 entry = {};
        entry.dwSize = sizeof(MODULEENTRY32);

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id_);
        if (!snapshot) {
            // Handle error
            return 0;
        }

        std::uintptr_t result = 0;
        while (Module32Next(snapshot, &entry)) {
            if (!module_name.compare(entry.szModule)) {
                result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
                break;
            }
        }

        CloseHandle(snapshot);
        return result;
    }

    template <typename T>
    constexpr T Read(const std::uintptr_t& address) const noexcept {
        T value = {};
        SIZE_T bytes_read = 0;
        if (ReadProcessMemory(process_handle_, reinterpret_cast<const void*>(address), &value, sizeof(T), &bytes_read) && bytes_read == sizeof(T)) {
            return value;
        }
        else {
            // Handle read error
            return T();
        }
    }

    template <typename T>
    constexpr void Write(const std::uintptr_t& address, const T& value) const noexcept {
        SIZE_T bytes_written = 0;
        if (!WriteProcessMemory(process_handle_, reinterpret_cast<void*>(address), &value, sizeof(T), &bytes_written) || bytes_written != sizeof(T)) {
            // Handle write error
        }
    }
};