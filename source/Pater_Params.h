#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

static const juce::ParameterID phaserParamID { "phaser", 1 };
static constexpr const char* phaserParamIDString = "phaser";

class Parameters
{
public:

    explicit Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate) noexcept;

    void reset() noexcept;

    void smoothen() noexcept;

    void update() noexcept;

private:

    juce::AudioParameterFloat* phaserParam { nullptr };

    juce::LinearSmoothedValue<float> phaserSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameters)
};



