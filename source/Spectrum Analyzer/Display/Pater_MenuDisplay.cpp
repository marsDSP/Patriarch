#include "Pater_MenuDisplay.h"

namespace MarsDSP
{
MenuDisplay::MenuDisplay()
{
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(settingButton);
    addAndMakeVisible(infoButton);
}

void MenuDisplay::paint(juce::Graphics& g) { juce::ignoreUnused(g); }

void MenuDisplay::resized()
{
    const auto area    = getLocalBounds();
    const auto height  = area.getHeight();
    const auto width   = area.getWidth();
    const auto spacing = 8;

    // Undo & Redo (left)
    undoButton.setBounds(juce::Rectangle<int>(spacing, 0, height, height));
    redoButton.setBounds(juce::Rectangle<int>(1 * height + 2 * spacing, 0, height, height));

    // Power (middle)
    const auto power_x = static_cast<int>(width / 2 - height / 2);
    bypassButton.setBounds(juce::Rectangle<int>(power_x, 0, height, height));

    // Settings (right)
    const auto settings_x = width - height - spacing;
    const auto info_x     = width - height * 2 - 2 * spacing;
    settingButton.setBounds(juce::Rectangle<int>(info_x, 0, height, height));
    infoButton.setBounds(juce::Rectangle<int>(settings_x, 0, height, height));
}

}