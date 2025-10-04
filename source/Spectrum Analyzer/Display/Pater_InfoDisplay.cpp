#include "Pater_InfoDisplay.h"

namespace MarsDSP
{
    InfoDisplay::InfoDisplay()
    {
        rows.emplace_back(juce::String("MarsDSP Patriarch"));

        // Version
        const juce::String version = JucePlugin_VersionString;
        rows.emplace_back(juce::String("Version: " + version));

        // License
        rows.emplace_back(juce::String("License: MIT"));
        rows.emplace_back(juce::String("Copyright 2022-2025 marsDSP"));
    }

    void InfoDisplay::paint(juce::Graphics& g)
    {
        // Background
        const auto bgColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
        g.fillAll(bgColor.brighter().withAlpha(0.5f));

        // Draw rows
        g.setColour(juce::Colours::black);
        g.setFont(24.0f);

        auto bounds = getLocalBounds();
        bounds.reduce(50, 100);
        const auto height = static_cast<int>(bounds.getHeight() / rows.size());
        for (const auto& row : rows)
            g.drawText(row, bounds.removeFromTop(height), juce::Justification::centred, true);
    }

    void InfoDisplay::resized() { }

}
