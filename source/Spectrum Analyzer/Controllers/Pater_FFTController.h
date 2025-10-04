#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Pater_AnalyzerTypes.h"
#include "../Pater_AnalyzerConstants.h"
#include "../Processor/Pater_EQ.h"
#include "Pater_BandController.h"



namespace MarsDSP
{
    class AnalyserController : public juce::ChangeListener, public juce::Timer, public juce::MouseListener
    {
    public:

        AnalyserController(MarsDSP::EqualizerProcessor&, juce::OwnedArray<MarsDSP::BandController>&,
                           MarsDSP::AnalyserView&);
        ~AnalyserController();

        void changeListenerCallback(juce::ChangeBroadcaster* sender) override;
        void timerCallback() override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    private:
        void updateFrequencyResponses();
        MarsDSP::EqualizerProcessor& processor;
        juce::OwnedArray<MarsDSP::BandController>& bandControllers;
        MarsDSP::AnalyserView& view;

        int draggingBand  = -1;
        bool draggingGain = false;
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserController)
    };
}
