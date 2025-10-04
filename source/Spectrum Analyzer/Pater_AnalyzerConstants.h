#pragma once

namespace MarsDSP
{
    // Maximum dB range for spectrum analyzer display
    constexpr float MAX_DB = 24.0f;
    
    // Refresh rate for spectrum analyzer UI updates (Hz)
    constexpr int GLOBAL_REFRESH_RATE_HZ = 30;
    
    // Click radius for band handle interaction (pixels)
    constexpr int HANDLE_CLICK_RADIUS = 10;
}
