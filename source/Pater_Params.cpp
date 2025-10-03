
#include "Pater_Params.h"
#include "Pater_DSPOption.h"


// Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
// {
//     // Bind parameter pointers from the APVTS using the declared ParameterIDs
//     phaserRateHz        = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserRateHzParamID.getParamID()));
//     phaserCenterFreqHz  = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserCenterFreqHzParamID.getParamID()));
//     phaserDepthPercent  = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserDepthPercentParamID.getParamID()));
//     phaserFeedbackPercent = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserFeedbackPercentParamID.getParamID()));
//     phaserWarmthPercent = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserWarmthPercentParamID.getParamID()));
//     phaserMixPercent    = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(phaserMixPercentParamID.getParamID()));
//
//     jassert(phaserRateHz && phaserCenterFreqHz && phaserDepthPercent && phaserFeedbackPercent && phaserWarmthPercent && phaserMixPercent);
// }

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, phaserRateHzParamID, phaserRateHzParam);
    castParameter(apvts, phaserNoteParamID, phaserNoteParam);
    castParameter(apvts, phaserCenterFreqHzParamID, phaserCenterFreqHzParam);
    castParameter(apvts, phaserDepthPercentParamID, phaserDepthPercentParam);
    castParameter(apvts, phaserFeedbackPercentParamID, phaserFeedbackPercentParam);
    castParameter(apvts, phaserWarmthPercentParamID, phaserWarmthPercentParam);
    castParameter(apvts, phaserMixPercentParamID, phaserMixPercentParam);
    castParameter(apvts, tempoSyncParamID, tempoSyncParam);
}

static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue();
    if (value < 20.0f)
    {
        return value * 1000.0f;
    }
    return value;
}

// changes from Hz to kHz based on user input
static juce::String stringFromHz(float value, int)
{
    if (value < 1000.0f)
    {
        return juce::String (int(value)) + "Hz";
    }
    else if (value < 10000.0f)
    {
        return juce::String (value / 1000.0f, 2) + "kHz";
    }
    else
    {
        return juce::String (value / 1000.0f, 1) + "kHz";
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Define Phaser parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserRateHzParamID, "Phaser RateHz",
        juce::NormalisableRange<float>{ 0.01f, 1.0f, 0.01f, 1.0f }, 0.20,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromHz)
        .withValueFromStringFunction(hzFromString)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserCenterFreqHzParamID, "Phaser Center FreqHz",
        juce::NormalisableRange<float>{ 20.0f, 20000.0f, 1.0f, 0.5f }, 1000.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromHz)
        .withValueFromStringFunction(hzFromString)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserDepthPercentParamID, "Phaser Depth%",
        juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f, 1.0f }, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserFeedbackPercentParamID, "Phaser Feedback%",
        juce::NormalisableRange<float>{ -1.0f, 1.0f, 0.01f, 1.0f }, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserWarmthPercentParamID, "Phaser Warmth%",
        juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f, 1.0f }, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserMixPercentParamID, "Phaser Mix%",
        juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f, 1.0f }, 1.0f));

    layout.add(std::make_unique<juce::AudioParameterBool>(tempoSyncParamID, "Tempo Sync", false));

    juce::StringArray noteLengths = {

        "1/32",
        "1/16T",
        "1/32.",
        "1/16",
        "1/8T",
        "1/16.",
        "1/8",
        "1/4T",
        "1/8.",
        "1/4",
        "1/2T",
        "1/4.",
        "1/2",
        "1/1T",
        "1/2.",
        "1/1",
    };

    layout.add(std::make_unique<juce::AudioParameterChoice>(phaserNoteParamID, "Phaser Note", noteLengths, 9));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    const double duration = 0.02; // 20 ms
    phaserSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    // Initialize smoother target to current mix value (arbitrary; adjust as needed)
    if (phaserMixPercentParam != nullptr)
        phaserSmoother.setTargetValue(phaserMixPercentParam->get());
    else
        phaserSmoother.setTargetValue(0.0f);
}

void Parameters::smoothen() noexcept
{
    phaserRate = targetPhaserRate;
    phaser = phaserSmoother.getNextValue();

}

void Parameters::update() noexcept
{
    phaserSmoother.setTargetValue(phaserParams->get() * 0.01f);

    phaserNote = phaserNoteParam->getIndex();
    tempoSync = tempoSyncParam->get();

    if (phaserRate == 0.0f)
    {
        phaserRate = targetPhaserRate;
    }
}