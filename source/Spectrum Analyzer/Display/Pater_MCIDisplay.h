#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MarsDSP
{
    class ModulationConnectItemDisplay : public juce::Component
    {
    private:
        int index;

    public:
        ModulationConnectItemDisplay(int i);
        ~ModulationConnectItemDisplay() override = default;

        void resized() override;

        juce::TextButton active;
        juce::Slider amount;
        juce::Label target;

        #if MARSDSP_LIVE_MOCK
    public:
        ModulationConnectItemDisplay() : ModulationConnectItemDisplay(1) { setSize(100, 100); }
        #endif

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationConnectItemDisplay)
    };
}
