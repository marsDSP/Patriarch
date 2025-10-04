#pragma once
#include "Pater_BandDisplay.h"
#include "../Processor/Pater_EQ.h"


namespace MarsDSP
{
BandDisplay::BandDisplay(int const i, const juce::Colour c)
    : index(i)
    , colour(c)
    , frequency(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    , quality(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    , gain(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    , solo(juce::translate("S"))
    , activate(juce::translate("A"))
{
    // Add all filter options to combo box
    type.clear();
    for (int j = 0; j < MarsDSP::EqualizerProcessor::LastFilterID; ++j)
    {
        using EQ               = MarsDSP::EqualizerProcessor;
        auto const type_string = MarsDSP::EqualizerProcessor::getFilterTypeName(static_cast<EQ::FilterType>(j));
        type.addItem(type_string, j + 1);
    }

    // Make controls visible
    addAndMakeVisible(type);
    addAndMakeVisible(gain);
    addAndMakeVisible(quality);
    addAndMakeVisible(frequency);
    addAndMakeVisible(activate);
    addAndMakeVisible(solo);

    // Name
    frequency.setName("Frequency");
    quality.setName("Quality");
    gain.setName("Gain");

    // Tooltip
    frequency.setTooltip(juce::translate("Filter's frequency"));
    quality.setTooltip(juce::translate("Filter's steepness (Quality)"));
    gain.setTooltip(juce::translate("Filter's gain"));

    // Solo
    solo.setClickingTogglesState(true);
    solo.setTooltip(juce::translate("Listen only through this filter (solo)"));

    // Activate / Bypass
    activate.setClickingTogglesState(true);
    activate.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    activate.setTooltip(juce::translate("Activate or deactivate this filter"));
}

void BandDisplay::paint(juce::Graphics& g)
{
    // Background
    const auto color = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    auto area        = getLocalBounds().reduced(5);
    g.setColour(colour);  // Use the band's colour, not undefined BLUE
    g.fillRect(area);
}

void BandDisplay::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(10, 20);

    const auto height      = bounds.getHeight();
    const auto width       = bounds.getWidth();
    const auto type_height = static_cast<int>(height / 12);

    // TYPE
    type.setBounds(bounds.removeFromTop(type_height));

    // FREQUENCY
    auto freq_bounds = bounds.removeFromBottom(bounds.getHeight() / 2);
    frequency.setBounds(freq_bounds);

    // BUTTONS
    const auto buttons_y = freq_bounds.getY() - type_height;
    auto buttons         = freq_bounds.withTop(buttons_y).withHeight(type_height);
    solo.setBounds(buttons.removeFromLeft(type_height));
    activate.setBounds(buttons.removeFromRight(type_height));

    // GAIN & QUALITY
    const auto quality_top    = static_cast<int>(bounds.getY() - type_height * 1.5);
    const auto quality_bounds = bounds.removeFromLeft(width / 2).withTop(quality_top);
    const auto gain_bounds    = bounds.withTop(bounds.getY() + type_height);
    quality.setBounds(quality_bounds);
    gain.setBounds(gain_bounds);
}
}