#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MarsDSP
{
    class InfoDisplay : public juce::Component
    {
    public:
        InfoDisplay();
        ~InfoDisplay() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        std::vector<juce::String> rows;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoDisplay)
    };

}
