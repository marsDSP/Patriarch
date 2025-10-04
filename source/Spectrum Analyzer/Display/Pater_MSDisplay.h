#pragma once


#include <juce_gui_basics/juce_gui_basics.h>


namespace MarsDSP
{

    class ModulationSourceDisplay : public juce::Component
    {
    private:
        int index;

    public:
        ModulationSourceDisplay(int);
        ~ModulationSourceDisplay() override = default;

        void paint(juce::Graphics& /*g*/) override;
        void resized() override;

        // Controls
        juce::Slider frequency;
        juce::Slider gain;
        juce::Label freqLabel, gainLabel;

        // Plot
        juce::Rectangle<int> plotFrame;
        juce::Path modulationPath;

        // Connect
        juce::TextButton toggleConnectView;  // activates the mapping view.
        // ModulationSourceDisplay::ModulationConnectItemDisplay modConnect1, modConnect2;  // TODO: ModulationConnectItemDisplay class not implemented

        #if MARSDSP_LIVE_MOCK
    public:
        ModulationSourceDisplay() : ModulationSourceDisplay(1) { setSize(100, 100); }
        #endif

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationSourceDisplay)
    };

}