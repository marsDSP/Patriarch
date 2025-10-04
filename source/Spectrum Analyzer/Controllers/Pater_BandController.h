#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include "../Pater_AnalyzerTypes.h"
#include "../Processor/Pater_EQ.h"



namespace MarsDSP
{
    class BandController
    {
    public:
        BandController(int, EQProcessor&, MarsDSP::EqualizerProcessor&,
                       MarsDSP::BandView&);

        void setUIControls(MarsDSP::EqualizerProcessor::FilterType type);
        void setSolo(bool isSolo);
        void setFrequency(float newFreq);
        void setGain(float newGain);
        void setType(int newType);
        juce::Path frequencyResponse;

    private:
        int index;
        MarsDSP::BandView& view;
        EQProcessor& mainProcessor;
        MarsDSP::EqualizerProcessor& processor;

        juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> boxAttachments;
        juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;
        juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandController)
    };
}
