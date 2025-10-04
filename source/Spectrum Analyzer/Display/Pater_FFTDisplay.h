#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../Pater_AnalyzerConstants.h"



namespace MarsDSP
{
    class AnalyzerDisplay : public juce::Component, public juce::ChangeBroadcaster
    {
    public:

        struct BandHandle
        {
            int id {};
            float x {};
            float y {};
            float label_x {};
            float label_y {};
            juce::Colour color {juce::Colours::white};
        };

        AnalyzerDisplay() = default;
        ~AnalyzerDisplay() override = default;

        void paint(juce::Graphics&) override;

        void resized() override;

        juce::Rectangle<int> plotFrame;
        juce::Path frequencyResponse;
        juce::Path in_analyser;
        juce::Path out_analyser;

        juce::PopupMenu contextMenu;

        std::vector<BandHandle> handles;

        inline static float get_position_for_frequency(float freq) noexcept
        {
            return (std::log(freq / 20.0f) / std::log(2.0f)) / 10.0f;
        }
        inline static float get_frequency_for_position(float pos) noexcept { return 20.0f * std::pow(2.0f, pos * 10.0f); }
        inline static float get_position_for_gain(float gain, float top, float bottom) noexcept
        {
            return juce::jmap(juce::Decibels::gainToDecibels(gain, -MAX_DB), -MAX_DB, MAX_DB, bottom, top);
        }
        inline static float get_gain_for_position(float pos, float top, float bottom) noexcept
        {
            return juce::Decibels::decibelsToGain(juce::jmap(pos, bottom, top, -MAX_DB, MAX_DB), -MAX_DB);
        }

        inline static bool overlap_with_radius(float obj_pos, float mouse_pos, int radius) noexcept
        {
            return std::abs(obj_pos - mouse_pos) < radius;
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerDisplay)
    };

}

