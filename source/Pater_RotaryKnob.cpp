
#include "Pater_RotaryKnob.h"

RotaryKnob::RotaryKnob(const juce::String& text,
                       juce::AudioProcessorValueTreeState& apvts,
                       const juce::ParameterID& parameterID,
                       bool /*drawFromMiddle*/)
    : label({}, text)
    , attachment(apvts, parameterID.getParamID(), slider)
{
    addAndMakeVisible(label);
    label.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 18);
}

RotaryKnob::~RotaryKnob() = default;

void RotaryKnob::resized()
{
    auto area = getLocalBounds();
    auto labelHeight = 18;
    label.setBounds(area.removeFromTop(labelHeight));
    slider.setBounds(area.reduced(2));
}
