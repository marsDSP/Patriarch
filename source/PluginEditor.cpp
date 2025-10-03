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
    setSize (600, 600);
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    int y = 50;
    int height = bounds.getHeight() - 60;

    phaserGroup.setBounds(10, y, 110, height);

    phaserRateHzKnob.setTopLeftPosition(20, 20);

    tempoSyncButton.setTopLeftPosition(20, 10);

}
