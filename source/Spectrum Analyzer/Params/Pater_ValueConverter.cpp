
#include "Pater_ValueConverter.h"

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace MarsDSP
{
// ACTIVE / BYPASS
juce::String ActiveTextConverter::operator()(float const value) const
{
    return value > 0.5f ? juce::translate("active") : juce::translate("bypassed");
}

float ActiveTextConverter::operator()(const juce::String& text) const
{
    return static_cast<float>(text == juce::translate("active"));
}

// FREQUENCY
juce::String FrequencyTextConverter::operator()(float const value) const
{
    return (value < 1000) ? juce::String(value, 0) + " Hz" : juce::String(value / 1000.0, 2) + " kHz";
}

float FrequencyTextConverter::operator()(const juce::String& text) const
{
    return text.endsWith(" kHz") ? static_cast<float>(text.dropLastCharacters(4).getFloatValue() * 1000.0)
                                 : static_cast<float>(text.dropLastCharacters(3).getFloatValue());
}

// QUALITY
juce::String QualityTextConverter::operator()(float const value) const { return juce::String(value, 1); }

float QualityTextConverter::operator()(const juce::String& text) const { return text.getFloatValue(); }

// GAIN
juce::String GainTextConverter::operator()(float const value) const
{
    return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB";
}

float GainTextConverter::operator()(const juce::String& text) const
{
    return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue());
}

// PHASE INVERT
juce::String InvertPhaseTextConverter::operator()(float const value) const { return value < 0.5 ? "Normal" : "Inverted"; }

float InvertPhaseTextConverter::operator()(const juce::String& text) const
{
    if (text == "Normal") { return 0.0f; }
    if (text == "Inverted") { return 1.0f; }
    return 0.0f;
}

// FILTER TYPE
// Note: These methods reference EqualizerProcessor which is not included in this build.
// Providing stub implementations that return basic filter type names.
juce::String FilterTypeTextConverter::operator()(float const value) const
{
    const int filterType = static_cast<int>(value);
    switch (filterType)
    {
        case 0: return "None";
        case 1: return "High Pass";
        case 2: return "Low Shelf";
        case 3: return "Band Pass";
        case 4: return "Peak";
        case 5: return "High Shelf";
        case 6: return "Low Pass";
        default: return "Unknown";
    }
}

float FilterTypeTextConverter::operator()(const juce::String& text) const
{
    if (text == "None") return 0.0f;
    if (text == "High Pass") return 1.0f;
    if (text == "Low Shelf") return 2.0f;
    if (text == "Band Pass") return 3.0f;
    if (text == "Peak") return 4.0f;
    if (text == "High Shelf") return 5.0f;
    if (text == "Low Pass") return 6.0f;
    return 0.0f;
}
}
