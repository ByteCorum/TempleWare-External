#pragma once

#include <cstddef>
#include <locale>
#include <Windows.h>

#include "../../external/imgui/imgui.h"

namespace globals {
    
    // TriggerBot
    inline bool TriggerBot = false;       
    inline int TriggerBotKey = VK_LSHIFT;    
    inline char TriggerBotKeyName[64] = "L-Shift"; 
    inline int TriggerBotMode = 0;       
    inline int TriggerBotDelay = 20;           
    inline bool TriggerBotToggled = false;    
    inline bool TriggerBotTeamCheck = true;  
    inline bool TriggerBotIgnoreFlash = false;  

    // Fov
    inline int FOV = 90;
    inline int lastFOV = 90;

    // Glow
    inline bool Glow = false;                     
    inline ImVec4 GlowColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // NoFlash
    inline bool NoFlashEnabled = false;          

    // Application state
    inline bool isRunning = true;            
    inline std::uintptr_t client = 0;     

    // Misc
    inline bool BunnyHopEnabled = false;     

    // Menu
    inline bool MenuVisible = true;             
    inline int ShowMenuKey = VK_END; 
    inline ImVec4 MenuAccentColor = ImVec4(0.0f, 0.4f, 1.0f, 1.0f);
}