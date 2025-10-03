
#include "Pater_Params.h"
#include "Pater_DSPOption.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, phaserParamID, phaserParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        phaserParamID, "Phaser",
        juce::NormalisableRange<float> { -12.0f, 12.0f }, 0.0f));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02; //20ms
    phaserSmoother.reset(sampleRate, duration); // 48000 × 0.02 = 960
}

void Parameters::reset() noexcept
{
    phaserSmoother.setTargetValue(juce::Decibels::decibelsToGain(phaserParam->get()));
}

void Parameters::smoothen() noexcept
{
    phaser = phaserSmoother.getNextValue();
}