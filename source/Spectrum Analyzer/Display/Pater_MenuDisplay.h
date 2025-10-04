#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MarsDSP
{
    class MenuDisplay : public juce::Component
    {
    public:
        MenuDisplay();
        ~MenuDisplay() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

        juce::TextButton undoButton { "Undo" };
        juce::TextButton redoButton { "Redo" };
        juce::TextButton bypassButton { "Bypass" };
        juce::TextButton settingButton { "Settings" };
        juce::TextButton infoButton { "Info" };

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuDisplay)
    };

}
