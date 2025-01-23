#pragma once

#include <d3d9.h>
#include <string>
#include <windows.h>
#include "../globals/globals.h"
#include "../config/config.h"
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_dx9.h"
#include "../../external/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND, UINT, WPARAM, LPARAM
);

namespace gui
{

    constexpr int WIDTH = 500;
    constexpr int HEIGHT = 300;

    inline bool isRunning = true;
    inline HWND  window = nullptr;
    inline WNDCLASSEX windowClass = { };
    inline POINTS position = { };

    inline PDIRECT3D9              d3d = nullptr;
    inline LPDIRECT3DDEVICE9       device = nullptr;
    inline D3DPRESENT_PARAMETERS   presentParameters = { };

    void CreateHWindow(const char* windowName) noexcept;
    void DestroyHWindow() noexcept;
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;
    void BeginRender() noexcept;
    void EndRender() noexcept;
    void Render() noexcept;

    void SetupImGuiStyle() noexcept;
    void ApplyCustomStyle() noexcept;

    static std::string GetKeyName(int vk)
    {
        UINT scanCode = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
        char keyName[128];
        if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0)
            return std::string(keyName);
        return "Unknown";
    }

    LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (device && wParam != SIZE_MINIMIZED)
            {
                presentParameters.BackBufferWidth = LOWORD(lParam);
                presentParameters.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        case WM_SYSCOMMAND:

            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
            position = MAKEPOINTS(lParam);
            return 0;
        case WM_MOUSEMOVE:
            if (wParam == MK_LBUTTON)
            {
                const auto points = MAKEPOINTS(lParam);
                RECT rect = { };
                GetWindowRect(window, &rect);

                rect.left += points.x - position.x;
                rect.top += points.y - position.y;

                if (position.x >= 0 && position.x <= WIDTH &&
                    position.y >= 0 && position.y <= 19)
                {
                    SetWindowPos(window, HWND_TOPMOST, rect.left, rect.top, 0, 0,
                        SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
                }
            }
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void CreateHWindow(const char* windowName) noexcept
    {
        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), CS_CLASSDC, WindowProcess, 0L, 0L,
                           GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                           "ObelusClass", nullptr };
        RegisterClassExA(&wc);

        window = CreateWindowExA(
            0, "ObelusClass", windowName,
            WS_POPUP, 100, 100, WIDTH, HEIGHT,
            0, 0, wc.hInstance, 0
        );

        SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        ShowWindow(window, SW_SHOWDEFAULT);
        UpdateWindow(window);
    }

    void DestroyHWindow() noexcept
    {
        DestroyWindow(window);
        UnregisterClassA("ObelusClass", GetModuleHandle(nullptr));
    }

    bool CreateDevice() noexcept
    {
        d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d) return false;

        ZeroMemory(&presentParameters, sizeof(presentParameters));
        presentParameters.Windowed = TRUE;
        presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
        presentParameters.EnableAutoDepthStencil = TRUE;
        presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
        presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

        if (d3d->CreateDevice(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &presentParameters,
            &device) < 0)
        {
            return false;
        }
        return true;
    }

    void ResetDevice() noexcept
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        if (device->Reset(&presentParameters) == D3DERR_INVALIDCALL)
            IM_ASSERT(0);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    void DestroyDevice() noexcept
    {
        if (device) { device->Release(); device = nullptr; }
        if (d3d) { d3d->Release();    d3d = nullptr; }
    }

    void CreateImGui() noexcept
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        (void)io;

        ImFontAtlas* fontAtlas = new ImFontAtlas();
        ImFontConfig arialConfig;
        arialConfig.FontDataOwnedByAtlas = false;

        const char* fontPath = "C:\\Windows\\Fonts\\segoeui.ttf";
        float fontSize = 18.0f;
        ImFont* arialFont = fontAtlas->AddFontFromFileTTF(fontPath,fontSize, &arialConfig);
        io.Fonts = fontAtlas;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX9_Init(device);

        SetupImGuiStyle();
    }

    void DestroyImGui() noexcept
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void BeginRender() noexcept
    {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                isRunning = false;
                return;
            }
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EndRender() noexcept
    {
        ImGui::EndFrame();

        device->SetRenderState(D3DRS_ZENABLE, FALSE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

        if (device->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            device->EndScene();
        }
        device->Present(nullptr, nullptr, nullptr, nullptr);
    }

    void Render() noexcept
    {

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(WIDTH), static_cast<float>(HEIGHT)), ImGuiCond_Always);

        ImGui::Begin("TempleWare - External                                                             templecheats.xyz",
            &globals::isRunning,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar
        );

        static int currentTab = 0;
        {

            if (ImGui::Button("Aim", ImVec2(80, 0)))   currentTab = 0; ImGui::SameLine();
            if (ImGui::Button("Visuals", ImVec2(80, 0))) currentTab = 1; ImGui::SameLine();
            if (ImGui::Button("Misc", ImVec2(80, 0)))    currentTab = 2; ImGui::SameLine();
            if (ImGui::Button("Config", ImVec2(80, 0)))  currentTab = 3;
        }

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BeginChild("MainChild", ImGui::GetContentRegionAvail(), true);

        if (currentTab == 0)
        {
            ImGui::Text("Aimbot / TriggerBot");
            ImGui::Separator();
            ImGui::Checkbox("TriggerBot", &globals::TriggerBot);

            if (globals::TriggerBot)
            {

                ImGui::Text("Trigger Key: ");
                ImGui::SameLine();
                if (ImGui::Button(globals::TriggerBotKeyName))
                {
                    ImGui::OpenPopup("##SelectTriggerKey");
                }
                if (ImGui::BeginPopup("##SelectTriggerKey"))
                {
                    ImGuiIO& io = ImGui::GetIO();
                    ImGui::Text("Press a key to select it...");
                    for (int i = 0; i < 256; i++)
                    {
                        if (ImGui::IsKeyPressed(i))
                        {
                            globals::TriggerBotKey = i;
                            std::string keyName = GetKeyName(i);
                            snprintf(globals::TriggerBotKeyName, sizeof(globals::TriggerBotKeyName), "%s", keyName.c_str());
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }

                const char* modeItems[] = { "Hold", "Toggle" };
                ImGui::Combo("Mode", &globals::TriggerBotMode, modeItems, IM_ARRAYSIZE(modeItems));
                ImGui::SliderInt("Delay (ms)", &globals::TriggerBotDelay, 1, 1000);
                ImGui::Checkbox("TeamCheck", &globals::TriggerBotTeamCheck);
                ImGui::Checkbox("IgnoreFlash", &globals::TriggerBotIgnoreFlash);
            }
        }

        else if (currentTab == 1)
        {
            ImGui::Text("Visuals");
            ImGui::Separator();

            ImGui::SliderInt("FOV", &globals::FOV, 60, 160);
            ImGui::Separator();

            ImGui::Checkbox("Glow##enable", &globals::Glow);
            if (globals::Glow)
            {
                ImGui::ColorEdit4("Glow Color", (float*)&globals::GlowColor, ImGuiColorEditFlags_NoInputs);
            }
            ImGui::Separator();

            ImGui::Checkbox("NoFlash", &globals::NoFlashEnabled);
        }

        else if (currentTab == 2)
        {
            ImGui::Text("Misc");
            ImGui::Separator();
            ImGui::Checkbox("BunnyHop", &globals::BunnyHopEnabled);
            ImGui::Separator();

            ImGui::Separator();
            ImGui::Text("Menu Color");
            ImGui::ColorEdit4("##MenuAccentColor",
                (float*)&globals::MenuAccentColor,
                ImGuiColorEditFlags_NoInputs);
        }

        else if (currentTab == 3)
        {
            ImGui::Text("Config");
            ImGui::Separator();

            static char configNameBuf[64] = "";

            static std::vector<std::string> configs;
            static int selectedConfigIndex = -1;

            auto refreshConfigs = [&]()
                {
                    configs = config::ConfigSystem::ListConfigs();

                    selectedConfigIndex = -1;
                };

            if (ImGui::Button("Refresh Config List"))
            {
                refreshConfigs();
            }

            ImGui::Text("Available configs:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##ConfigList",
                (selectedConfigIndex >= 0 && selectedConfigIndex < (int)configs.size())
                ? configs[selectedConfigIndex].c_str()
                : "Select config"))
            {
                for (int i = 0; i < (int)configs.size(); i++)
                {
                    bool isSelected = (selectedConfigIndex == i);
                    if (ImGui::Selectable(configs[i].c_str(), isSelected))
                    {
                        selectedConfigIndex = i;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (selectedConfigIndex >= 0)
            {

                if (ImGui::Button("Load Config"))
                {
                    config::ConfigSystem::Load(configs[selectedConfigIndex]);
                }
                ImGui::SameLine();

                if (ImGui::Button("Delete Config"))
                {
                    config::ConfigSystem::Remove(configs[selectedConfigIndex]);
                    refreshConfigs();
                }
            }

            ImGui::Separator();

            ImGui::Text("Create new config:");
            ImGui::InputText("##NewConfigName", configNameBuf, IM_ARRAYSIZE(configNameBuf));
            ImGui::SameLine();
            if (ImGui::Button("Save Config"))
            {

                if (std::strlen(configNameBuf) > 0)
                {
                    config::ConfigSystem::Save(configNameBuf);

                    refreshConfigs();
                }
            }
        }

        ImGui::EndChild();
        ImGui::End();

        ApplyCustomStyle();
    }

    void SetupImGuiStyle() noexcept
    {
        ImGuiStyle& style = ImGui::GetStyle();

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.0f, 0.4f, 1.0f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.6f, 1.0f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.30f, 1.00f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.6f, 1.0f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.6f, 1.0f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.40f, 1.00f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.35f, 0.90f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.35f, 0.90f, 1.0f);

    }

    void ApplyCustomStyle() noexcept
    {

        ImGuiStyle& style = ImGui::GetStyle();

        style.Colors[ImGuiCol_Button] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_ButtonHovered] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_ButtonActive] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_CheckMark] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_SliderGrab] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_SliderGrabActive] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_Header] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_HeaderHovered] = globals::MenuAccentColor;
        style.Colors[ImGuiCol_HeaderActive] = globals::MenuAccentColor;
    }
}