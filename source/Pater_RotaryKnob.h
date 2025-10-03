
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

// Custom LookAndFeel to draw the rotary slider using an SVG from assets
class SvgKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SvgKnobLookAndFeel();
    ~SvgKnobLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

private:
    std::unique_ptr<juce::Drawable> knobDrawable;
};

class RotaryKnob : public juce::Component
{
public:
    RotaryKnob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, bool drawFromMiddle = false);
    ~RotaryKnob() override;

    juce::Label label;
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    void resized() override;

private:
    SvgKnobLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};