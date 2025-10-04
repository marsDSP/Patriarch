#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Pater_AnalyzerTypes.h"
#include "../Processor/Pater_MSProcessor.h"
#include "../Display/Pater_MSDisplay.h"

namespace MarsDSP
{
    class ModulationSourceController : public juce::Slider::Listener, public juce::Timer
    {
    public:
        ModulationSourceController(int, EQProcessor&, MarsDSP::ModulationSourceProcessor&,
                                   MarsDSP::ModulationSourceView&);

        void sliderValueChanged(juce::Slider* slider) override;
        void timerCallback() override;

    private:
        int index;
        bool connectViewActive;

        EQProcessor& mainProcessor;
        MarsDSP::ModulationSourceProcessor& processor;
        MarsDSP::ModulationSourceView& view;

        juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationSourceController)
    };
}

