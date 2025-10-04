#include "Pater_SettingsDisplay.h"

namespace MarsDSP
{
    SettingsDisplay::SettingsDisplay() { rows.emplace_back(juce::String("Settings")); }

    void SettingsDisplay::paint(juce::Graphics& g)
    {
        // Background
        const auto bgColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
        g.fillAll(bgColor.brighter().withAlpha(0.5f));

        // Draw rows
        g.setColour(juce::Colours::black);
        g.setFont(32.0f);

        auto bounds       = getLocalBounds();
        const auto height = static_cast<int>(bounds.getHeight() / rows.size());
        for (const auto& row : rows)
            g.drawText(row, bounds.removeFromTop(height), juce::Justification::centred, true);
    }

    void SettingsDisplay::resized() { }

}
