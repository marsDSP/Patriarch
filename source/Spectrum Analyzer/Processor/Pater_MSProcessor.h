#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_graphics/juce_graphics.h>
#include "Pater_BaseProcessor.h"
#include "../Analyzer/Pater_MSFFT.h"
#include "../Params/Pater_ValueConverter.h"



namespace MarsDSP
{
    class ModulationSourceProcessor : public BaseProcessor, public juce::AudioProcessorValueTreeState::Listener

    {
    public:
        ModulationSourceProcessor(int, juce::AudioProcessorValueTreeState&);
        ~ModulationSourceProcessor() override;

        void prepareToPlay(double /*unused*/, int /*unused*/) override;
        void processBlock(juce::AudioBuffer<float>&, [[maybe_unused]] juce::MidiBuffer& /*unused*/) override;

        void parameterChanged(const juce::String& parameter, float newValue) override;

        void createAnalyserPlot(juce::Path&, juce::Rectangle<int>&, float);
        bool checkForNewAnalyserData();

        void reset() override { oscillator.reset(); }

    private:
        int index;
        juce::String paramIDGain, paramIDFrequency;
        juce::dsp::Oscillator<float> oscillator;
        juce::dsp::Gain<float> gain;
        MarsDSP::ModulationSourceAnalyser<float> analyser;
        FrequencyTextConverter frequencyTextConverter;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationSourceProcessor)
    };

}
