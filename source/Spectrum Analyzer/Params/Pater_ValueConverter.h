
#pragma once

#include <juce_core/juce_core.h>

namespace MarsDSP
{

    struct ActiveTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };

    struct FrequencyTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };

    struct QualityTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };

    struct GainTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };

    struct InvertPhaseTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };

    struct FilterTypeTextConverter
    {
        juce::String operator()(float value) const;

        float operator()(const juce::String& text) const;
    };
}

