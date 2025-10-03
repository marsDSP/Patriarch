
#include "Pater_RotaryKnob.h"
#include "BinaryData.h"

// ================= SvgKnobLookAndFeel =================
SvgKnobLookAndFeel::SvgKnobLookAndFeel()
{
    // Load the SVG from the embedded binary assets by name
    int dataSize = 0;
    if (auto* data = BinaryData::getNamedResource("ReaPlugs (vst GUI) (Community).svg", dataSize))
    {
        juce::String xmlString = juce::String::fromUTF8(reinterpret_cast<const char*>(data), dataSize);
        juce::XmlDocument doc(xmlString);
        auto svgXml = doc.getDocumentElement();
        if (svgXml != nullptr)
            knobDrawable = juce::Drawable::createFromSVG(*svgXml);
    }
}

void SvgKnobLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                          int x, int y, int width, int height,
                                          float sliderPosProportional,
                                          float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                         static_cast<float>(width), static_cast<float>(height)).reduced(2.0f);

    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto lineWidth = juce::jmax(2.0f, radius * 0.08f);
    auto arcRadius = radius - lineWidth * 0.8f;

    // Draw background arc (track)
    {
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                    rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw value arc
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    {
        juce::Path valueArc;
        valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                               rotaryStartAngle, angle, true);
        auto activeColour = slider.findColour(juce::Slider::rotarySliderFillColourId).withAlpha(0.95f);
        g.setColour(activeColour);
        g.strokePath(valueArc, juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Draw the knob SVG rotated by current angle
    if (knobDrawable != nullptr)
    {
        juce::Graphics::ScopedSaveState save(g);
        g.addTransform(juce::AffineTransform::rotation(angle, centre.x, centre.y));
        knobDrawable->drawWithin(g, bounds, juce::RectanglePlacement::centred, 1.0f);
    }
    else
    {
        // Fallback if SVG failed to load: draw a default ellipse
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.2f);
    }

    // Draw a dial indicator line on top at the current angle
    {
        auto innerGap = juce::jmax(arcRadius * 0.3f, 8.0f);
        juce::Point<float> start(centre.x + innerGap * std::sin(angle),
                                 centre.y - innerGap * std::cos(angle));
        juce::Point<float> end(centre.x + (arcRadius - lineWidth * 0.5f) * std::sin(angle),
                               centre.y - (arcRadius - lineWidth * 0.5f) * std::cos(angle));
        juce::Path needle;
        needle.startNewSubPath(start);
        needle.lineTo(end);
        g.setColour(juce::Colours::white.withAlpha(0.95f));
        g.strokePath(needle, juce::PathStrokeType(juce::jmax(2.0f, lineWidth * 0.7f), juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}

// ================= RotaryKnob =================
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

    // Apply SVG LookAndFeel to the slider
    slider.setLookAndFeel(&lookAndFeel);
}

RotaryKnob::~RotaryKnob()
{
    // Detach LAF to avoid dangling pointer during teardown
    slider.setLookAndFeel(nullptr);
}

void RotaryKnob::resized()
{
    auto area = getLocalBounds();
    auto labelHeight = 18;
    label.setBounds(area.removeFromTop(labelHeight));
    slider.setBounds(area.reduced(2));
}
