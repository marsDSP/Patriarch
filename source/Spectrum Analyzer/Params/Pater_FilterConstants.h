#pragma once

namespace MarsDSP
{
    // Filter parameter ranges for EQ bands
    constexpr float FILTER_FREQ_MIN = 20.0f;
    constexpr float FILTER_FREQ_MAX = 20000.0f;
    constexpr float FILTER_FREQ_STEP_SIZE = 1.0f;
    
    constexpr float FILTER_Q_MIN = 0.1f;
    constexpr float FILTER_Q_MAX = 10.0f;
    constexpr float FILTER_Q_STEP_SIZE = 0.01f;
    
    constexpr float MAX_GAIN = 10.0f;  // ±20 dB (gain factor of 10)
    constexpr float FILTER_GAIN_STEP_SIZE = 0.01f;
}
