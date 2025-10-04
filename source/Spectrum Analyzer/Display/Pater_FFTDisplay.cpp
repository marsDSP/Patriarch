#include "Pater_FFTDisplay.h"

namespace MarsDSP
{
void AnalyzerDisplay::paint(juce::Graphics& g)
{
    // Save graphics state
    juce::Graphics::ScopedSaveState state(g);

    // Background
    const auto bgColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.fillAll(bgColor);

    // Vertical lines & frequency labels
    g.setFont(15.0f);
    for (int i = 0; i < 10; ++i)
    {
        g.setColour(juce::Colours::silver.withAlpha(0.4f));
        auto x = plotFrame.getX() + plotFrame.getWidth() * i * 0.1f;
        if (i > 0)
        {
            const auto y      = static_cast<float>(plotFrame.getY());
            const auto bottom = static_cast<float>(plotFrame.getBottom());
            g.drawVerticalLine(juce::roundToInt(x), y, bottom);
        }

        const auto freq = get_frequency_for_position(i * 0.1f);
        g.setColour(juce::Colour(0xffb9f6ca));
        g.drawFittedText((freq < 1000) ? juce::String(freq) + " Hz" : juce::String(freq / 1000, 1) + " kHz",
                         juce::roundToInt(x + 3), plotFrame.getBottom() - 18, 50, 15, juce::Justification::left, 1);
    }

    // Horizontal lines
    g.setColour(juce::Colours::silver.withAlpha(0.4f));
    g.drawHorizontalLine(juce::roundToInt(plotFrame.getY() + 0.25 * plotFrame.getHeight()),
                         static_cast<float>(plotFrame.getX()), static_cast<float>(plotFrame.getRight()));
    g.drawHorizontalLine(juce::roundToInt(plotFrame.getY() + 0.75 * plotFrame.getHeight()),
                         static_cast<float>(plotFrame.getX()), static_cast<float>(plotFrame.getRight()));

    // dB labels
    g.setColour(juce::Colours::silver);
    g.drawFittedText(juce::String(MAX_DB) + " dB", plotFrame.getX() + 3, plotFrame.getY() + 2, 50, 14,
                     juce::Justification::left, 1);
    g.drawFittedText(juce::String(MAX_DB / 2) + " dB", plotFrame.getX() + 3,
                     juce::roundToInt(plotFrame.getY() + 2 + 0.25 * plotFrame.getHeight()), 50, 14,
                     juce::Justification::left, 1);
    g.drawFittedText(" 0 dB", plotFrame.getX() + 3,
                     juce::roundToInt(plotFrame.getY() + 2 + 0.5 * plotFrame.getHeight()), 50, 14,
                     juce::Justification::left, 1);
    g.drawFittedText(juce::String(-MAX_DB / 2) + " dB", plotFrame.getX() + 3,
                     juce::roundToInt(plotFrame.getY() + 2 + 0.75 * plotFrame.getHeight()), 50, 14,
                     juce::Justification::left, 1);

    g.reduceClipRegion(plotFrame);

    // Analysers
    const juce::Colour inputColour  = juce::Colours::yellow;
    const juce::Colour outputColour = juce::Colours::purple;
    g.setFont(18.0f);

    const float corner_radius = 10.0f;

    // Input Analyser
    g.setColour(inputColour);
    g.drawFittedText("Input", plotFrame.reduced(8), juce::Justification::topRight, 1);
    g.strokePath(in_analyser.createPathWithRoundedCorners(corner_radius), juce::PathStrokeType(1.0));

    // Output Analyser
    g.setColour(outputColour);
    g.drawFittedText("Output", plotFrame.reduced(8, 28), juce::Justification::topRight, 1);
    g.strokePath(out_analyser.createPathWithRoundedCorners(corner_radius), juce::PathStrokeType(2.0));

    // Frequency Response
    g.setColour(juce::Colour(0xff00ff08).withMultipliedAlpha(0.9f).brighter());
    g.strokePath(frequencyResponse.createPathWithRoundedCorners(corner_radius), juce::PathStrokeType(3.5f));

    for (const auto& handle : handles)
    {
        const int size {30};
        g.setColour(handle.color);
        // Label
        g.drawFittedText(juce::String(handle.id), static_cast<int>(handle.label_x), static_cast<int>(handle.label_y),
                         size, size, juce::Justification::left, 1);

        // Handle
        g.drawEllipse(static_cast<float>(handle.x), static_cast<float>(handle.y), 8.0f, 8.0f, 5.0f);
    }
}

void AnalyzerDisplay::resized()
{
    auto area = getLocalBounds();
    plotFrame = area.reduced(3, 3);
    sendChangeMessage();
}
}