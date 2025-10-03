#include "PluginEditor.h"
#include "Pater_DSPOption.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p)
    , patriarchProcessor (p)
    , phaserRateHzKnob("Phaser Rate", patriarchProcessor.apvts, phaserRateHzParamID)
    , phaserNoteKnob("Phaser Note", patriarchProcessor.apvts, phaserNoteParamID)
    , phaserCenterFreqHzKnob("Phaser Center Freq", patriarchProcessor.apvts, phaserCenterFreqHzParamID)
    , phaserDepthPercentKnob("Phaser Depth", patriarchProcessor.apvts, phaserDepthPercentParamID)
    , phaserFeedbackPercentKnob("Phaser Feedback", patriarchProcessor.apvts, phaserFeedbackPercentParamID)
    , phaserWarmthPercentKnob("Phaser Warmth", patriarchProcessor.apvts, phaserWarmthPercentParamID)
    , phaserMixPercentKnob("Phaser Mix", patriarchProcessor.apvts, phaserMixPercentParamID)
{
    setLookAndFeel (&mainLF);

    setResizable (true, true);
    setResizeLimits (300, 200, 2000, 2000);


    phaserGroup.setText("Phaser");
    phaserGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    phaserGroup.addAndMakeVisible(phaserRateHzKnob);
    phaserGroup.addChildComponent(phaserNoteKnob);
    phaserGroup.addAndMakeVisible(phaserCenterFreqHzKnob);
    phaserGroup.addAndMakeVisible(phaserDepthPercentKnob);
    phaserGroup.addAndMakeVisible(phaserFeedbackPercentKnob);
    phaserGroup.addAndMakeVisible(phaserWarmthPercentKnob);
    phaserGroup.addAndMakeVisible(phaserMixPercentKnob);
    addAndMakeVisible(phaserGroup);

    // tempo sync toggle
    addAndMakeVisible(tempoSyncButton);
    if (auto* param = patriarchProcessor.apvts.getParameter(tempoSyncParamID.getParamID()))
    {
        param->addListener(this);
        if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param))
            updateDelayKnobs(boolParam->get());
    }

    // this chunk of code instantiates and opens the melatonin inspector
    // addAndMakeVisible (inspectButton);
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (690, 230);
}

PluginEditor::~PluginEditor()
{
    if (auto* param = patriarchProcessor.apvts.getParameter(tempoSyncParamID.getParamID()))
        param->removeListener(this);
    setLookAndFeel (nullptr);
}

void PluginEditor::updateDelayKnobs(bool tempoSyncActive)
{
    phaserRateHzKnob.setVisible(!tempoSyncActive);
    phaserNoteKnob.setVisible(tempoSyncActive);
}

void PluginEditor::parameterValueChanged(int, float value)
{
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        updateDelayKnobs(value != 0.0f);
    }

    else
    {
        juce::MessageManager::callAsync([this, value] {

            updateDelayKnobs(value != 0.0f);

        });
    }
}

void PluginEditor::paint (juce::Graphics& g)
{
    auto noise = juce::ImageCache::getFromMemory(BinaryData::Noise_png, BinaryData::Noise_pngSize);
    auto fillType = juce::FillType(noise, juce::AffineTransform::scale(0.5f));

    g.fillAll(juce::Colours::grey);

    // g.setFillType(fillType);
    // g.fillRect(getLocalBounds());

    // Draw the header bar on top
    auto rect = getLocalBounds().withHeight(40);
    g.setColour(Colors::header);
    g.fillRect(rect);

}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    int y = 50;
    int height = bounds.getHeight() - 60;

    phaserGroup.setBounds(10, y, bounds.getWidth() - 20, height);

    // Size and position knobs inside the group
    auto groupBounds = phaserGroup.getLocalBounds().reduced(10);
    groupBounds.removeFromTop(20); // Space for group title
    
    int knobSize = 100;
    int knobSpacing = 10;
    
    phaserRateHzKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));
    groupBounds.removeFromLeft(knobSpacing);
    
    phaserNoteKnob.setBounds(phaserRateHzKnob.getBounds()); // Same position as rate knob
    
    phaserCenterFreqHzKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));
    groupBounds.removeFromLeft(knobSpacing);
    
    phaserDepthPercentKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));
    groupBounds.removeFromLeft(knobSpacing);
    
    phaserFeedbackPercentKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));
    groupBounds.removeFromLeft(knobSpacing);
    
    phaserWarmthPercentKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));
    groupBounds.removeFromLeft(knobSpacing);
    
    phaserMixPercentKnob.setBounds(groupBounds.removeFromLeft(knobSize).withHeight(120));

    tempoSyncButton.setBounds(20, 10, 100, 30);
}
