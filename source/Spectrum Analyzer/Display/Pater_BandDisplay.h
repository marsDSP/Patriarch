#pragma once

#include <juce_gui_basics/juce_gui_basics.h>



namespace MarsDSP
{
    class BandDisplay : public juce::Component
    {
    private:
        int index;
        juce::Colour colour;

    public:
        BandDisplay(int i, juce::Colour = juce::Colours::white);
        ~BandDisplay() override = default;

        void paint(juce::Graphics&) override;
        void resized() override;

        juce::ComboBox type;
        juce::Slider frequency;
        juce::Slider quality;
        juce::Slider gain;
        juce::TextButton solo;
        juce::TextButton activate;

        #if MARSDSP_LIVE_MOCK
    public:
        BandDisplay() : BandView(1) { setSize(100, 100); }
        #endif

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandDisplay)
    };

}
