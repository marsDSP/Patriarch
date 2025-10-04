#include "Pater_MCIDisplay.h"

namespace MarsDSP
{
    ModulationConnectItemDisplay::ModulationConnectItemDisplay(int i)
        : index(i), active("A"), amount(juce::Slider::LinearHorizontal, juce::Slider::NoTextBox)
    {
        // Toggle Button
        addAndMakeVisible(active);

        // Slider
        amount.setRange(-1.0, 1.0, 0.0);
        addAndMakeVisible(amount);

        // Label
        target.setJustificationType(juce::Justification::centred);
        target.setText("Target: " + juce::String(index), juce::NotificationType::dontSendNotification);
        addAndMakeVisible(target);
    }

    void ModulationConnectItemDisplay::resized()
    {
        auto area             = getLocalBounds();
        const auto sliderArea = area.removeFromRight(area.getWidth() / 2);

        // Button
        active.setBounds(area.removeFromRight(area.getWidth() / 6).reduced(0, 5));

        // Labels
        target.setBounds(area);

        // Sliders
        amount.setBounds(sliderArea);
    }

}
