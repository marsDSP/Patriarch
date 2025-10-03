#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================

static const juce::ParameterID tempoSyncParamID { "tempoSync", 1 };
static constexpr const char* tempoSyncParamIDString = "tempoSync";

//==============================================================================

static const juce::ParameterID phaserRateHzParamID { "phaserRateHz", 1 };
static constexpr const char* phaserRateHzParamIDString = "phaserRateHz";

static const juce::ParameterID phaserNoteParamID { "phaserNote", 1 };
static constexpr const char* phaserNoteParamIDString = "phaserNote";

static const juce::ParameterID phaserCenterFreqHzParamID { "phaserCenterFreqHz", 1 };
static constexpr const char* phaserCenterFreqHzParamIDString = "phaserCenterFreqHz";

static const juce::ParameterID phaserDepthPercentParamID { "phaserDepthPercent", 1 };
static constexpr const char* phaserDepthPercentParamIDString = "phaserDepthPercent";

static const juce::ParameterID phaserFeedbackPercentParamID { "phaserFeedbackPercent", 1 };
static constexpr const char* phaserFeedbackPercentParamIDString = "phaserFeedbackPercent";

static const juce::ParameterID phaserWarmthPercentParamID { "phaserWarmthPercent", 1 };
static constexpr const char* phaserWarmthPercentParamIDString = "phaserWarmthPercent";

static const juce::ParameterID phaserMixPercentParamID { "phaserMixPercent", 1 };
static constexpr const char* phaserMixPercentParamIDString = "phaserMixPercent";

//==============================================================================

class Parameters
{
public:
    //=========================
    // PHASER

    float phaserRate = 0.05f;
    float feedback = 0.0f;
    float centerFreq = 0.0f;
    float mix = 0.5f;
    float Warmth = 0.0f;
    float depth = 0.0f;

    //=========================

    int phaserNote = 0;
    bool tempoSync = false;

    explicit Parameters(juce::AudioProcessorValueTreeState& apvts); // {*this, nullptr, "Settings", createParameterLayout()};
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void update() noexcept;

    juce::AudioParameterBool* tempoSyncParam { nullptr };

private:

    juce::AudioParameterFloat* phaserParams { nullptr };

    juce::AudioParameterFloat* phaserRateHzParam { nullptr };
    juce::AudioParameterFloat* phaserCenterFreqHzParam { nullptr };
    juce::AudioParameterFloat* phaserDepthPercentParam { nullptr };
    juce::AudioParameterFloat* phaserFeedbackPercentParam { nullptr };
    juce::AudioParameterFloat* phaserWarmthPercentParam { nullptr };
    juce::AudioParameterFloat* phaserMixPercentParam { nullptr };

    juce::AudioParameterChoice* phaserNoteParam { nullptr };

    juce::LinearSmoothedValue<float> phaserSmoother;

    float phaser { 0.0f };

    float targetPhaserRate = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameters)
};



