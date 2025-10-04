#include "Pater_MSDisplay.h"

namespace MarsDSP
{
ModulationSourceDisplay::ModulationSourceDisplay(int i)
    : index(i)
    , frequency(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    , gain(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    , toggleConnectView("Connect")
{
    // Slider
    addAndMakeVisible(frequency);
    addAndMakeVisible(gain);

    // Label
    freqLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(gainLabel);

    // Button
    addAndMakeVisible(toggleConnectView);
}

void ModulationSourceDisplay::paint(juce::Graphics& g)
{
    // Save graphics state
    juce::Graphics::ScopedSaveState state(g);

    // Background
    const auto backgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.setColour(backgroundColour.darker());
    g.fillRect(plotFrame.toFloat());

    // Frame
    g.setFont(12.0f);
    g.setColour(juce::Colours::silver);
    g.drawRoundedRectangle(plotFrame.toFloat(), 6, 5);

    // Vertical lines
    for (int i = 0; i < 10; ++i)
    {
        g.setColour(juce::Colours::silver.withAlpha(0.4f));
        auto x = plotFrame.getX() + plotFrame.getWidth() * i * 0.1f;
        if (i > 0)
            g.drawVerticalLine(juce::roundToInt(x), static_cast<float>(plotFrame.getY()),
                               static_cast<float>(plotFrame.getBottom()));
    }

    // Horizontal lines
    g.setColour(juce::Colours::silver.withAlpha(0.4f));
    g.drawHorizontalLine(juce::roundToInt(plotFrame.getY() + 0.25 * plotFrame.getHeight()),
                         static_cast<float>(plotFrame.getX()), static_cast<float>(plotFrame.getRight()));
    g.drawHorizontalLine(juce::roundToInt(plotFrame.getY() + 0.75 * plotFrame.getHeight()),
                         static_cast<float>(plotFrame.getX()), static_cast<float>(plotFrame.getRight()));

    g.reduceClipRegion(plotFrame);

    // Label
    g.setFont(16.0f);
    g.setColour(juce::Colour(0xff00ff08));
    g.drawFittedText("LFO", plotFrame.reduced(12), juce::Justification::topRight, 1);

    // LFO path
    g.setColour(juce::Colour(0xff00ff08).withMultipliedAlpha(0.9f).brighter());
    g.strokePath(modulationPath, juce::PathStrokeType(3.0));
}

void ModulationSourceDisplay::resized()
{
    auto area       = getLocalBounds();
    auto sliderArea = area.removeFromRight(area.getWidth() / 4);

    int const labelHeight = sliderArea.getHeight() / 2 / 5;

    // Labels
    freqLabel.setBounds(sliderArea.removeFromTop(labelHeight));
    gainLabel.setBounds(sliderArea.removeFromBottom(labelHeight));

    // Sliders
    frequency.setBounds(sliderArea.removeFromTop(sliderArea.getHeight() / 2));
    gain.setBounds(sliderArea);

    // Button
    auto button_area = area.removeFromBottom(area.getHeight() / 6).reduced(1);
    toggleConnectView.setBounds(button_area.removeFromLeft(button_area.getWidth() / 2));

    // LFO plot
    auto reduced_area = area.reduced(3, 3);
    plotFrame         = reduced_area;
}
}

