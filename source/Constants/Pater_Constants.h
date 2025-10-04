#pragma once
// namespace for all MarsDSP classes, functions & constants.
namespace MarsDSP
{
    // Audio
    constexpr auto MAX_DB      = 24.0f;
    inline auto const MAX_GAIN = Decibels::decibelsToGain(tobanteAudio::MAX_DB);

    // Gain
    constexpr auto GAIN_MIN       = 0.0f;
    constexpr auto GAIN_MAX       = 2.0f;
    constexpr auto GAIN_STEP_SIZE = 0.01f;
    constexpr auto GAIN_DEFAULT   = 1.0f;

    // Filter
    constexpr auto FILTER_GAIN_STEP_SIZE = 0.001f;
    constexpr auto FILTER_FREQ_MIN       = 20.0f;
    constexpr auto FILTER_FREQ_MAX       = 20'000.0f;
    constexpr auto FILTER_FREQ_STEP_SIZE = 0.1f;
    constexpr auto FILTER_Q_MIN          = 0.1f;
    constexpr auto FILTER_Q_MAX          = 10.0f;
    constexpr auto FILTER_Q_STEP_SIZE    = 0.1f;

    // LFO
    constexpr auto LFO_GAIN_MAX       = 1.0f;
    constexpr auto LFO_FREQ_MIN       = 0.01f;
    constexpr auto LFO_FREQ_MAX       = 10.0f;
    constexpr auto LFO_FREQ_STEP_SIZE = 0.01f;
    constexpr auto LFO_FREQ_SKEW      = 1.0f;
    constexpr auto LFO_FREQ_DEFAULT   = 0.3f;

    // UI Global frames per second.
    const int GLOBAL_REFRESH_RATE_HZ = 60;

    // Click radius for band handles in analyser plot.
    const int HANDLE_CLICK_RADIUS = 10;
}


// #endif //PAMPLEJUCE_PATER_CONSTANTS_H
