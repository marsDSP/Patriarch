#pragma once

#include <juce_core/juce_core.h>
#include "../Pater_AnalyzerTypes.h"
#include "../Display/Pater_MenuDisplay.h"

namespace MarsDSP
{
    class MenuBarController
    {
    public:

        MenuBarController(EQProcessor&, MarsDSP::MenuBarView&);
        std::function<void()> toggleBypass;
        std::function<void()> toggleSettings;
        std::function<void()> toggleInfo;

    private:
        EQProcessor& processor;
        MarsDSP::MenuBarView& view;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarController)
    };
}
