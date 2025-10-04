#include "PluginEditor.h"
#include "Pater_DSPOption.h"
#include "Spectrum Analyzer/Display/Pater_FFTDisplay.h"
#include "Spectrum Analyzer/Display/Pater_MenuDisplay.h"
#include "Spectrum Analyzer/Display/Pater_SettingsDisplay.h"
#include "Spectrum Analyzer/Display/Pater_InfoDisplay.h"

// Spectrum Analyzer: enhanced component using the dedicated AnalyzerDisplay to draw grid/labels
class SpectrumAnalyserComponent : public MarsDSP::AnalyzerDisplay, private juce::Timer
{
public:
    explicit SpectrumAnalyserComponent(PluginProcessor& proc) : processor(proc)
    {
        startTimerHz(30); // update ~30 fps
    }

private:
    void timerCallback() override
    {
        if (plotFrame.isEmpty())
            return;

        if (processor.checkForNewAnalyserData())
        {
            // Fill both input and output paths using the available analyser data
            // (Processor currently provides a single stream; duplicate for visualisation)
            processor.createAnalyserPlot(in_analyser, plotFrame.toFloat(), 20.0f);
            processor.createAnalyserPlot(out_analyser, plotFrame.toFloat(), 20.0f);
            repaint(plotFrame);
        }
    }

    PluginProcessor& processor;
};

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
    setResizeLimits (300, 200, 4000, 3000);

    // Top menu bar and overlays
    addAndMakeVisible(menuBar);
    addChildComponent(settingsDisplay);
    addChildComponent(infoDisplay);

    menuBar.settingButton.onClick = [this]
    {
        showSettings = !showSettings;
        if (showSettings) showInfo = false;
        updateOverlaysVisibility();
    };
    menuBar.infoButton.onClick = [this]
    {
        showInfo = !showInfo;
        if (showInfo) showSettings = false;
        updateOverlaysVisibility();
    };
    menuBar.bypassButton.onClick = [this]
    {
        // Placeholder: local toggle UI only
        menuBar.bypassButton.setToggleState(!menuBar.bypassButton.getToggleState(), juce::NotificationType::dontSendNotification);
    };

    // Spectrum Analyzer: create and add the analyser view above parameters
    spectrumView = std::make_unique<SpectrumAnalyserComponent>(patriarchProcessor);
    addAndMakeVisible(*spectrumView);

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

    // Ensure initial overlay visibility
    updateOverlaysVisibility();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (670, 630);
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

void PluginEditor::updateOverlaysVisibility()
{
    settingsDisplay.setVisible(showSettings);
    infoDisplay.setVisible(showInfo);

    if (showSettings) settingsDisplay.toFront(true);
    if (showInfo) infoDisplay.toFront(true);
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

    const int headerHeight = 40;
    const int margin = 8;

    // Menu bar at the top
    menuBar.setBounds(margin, 0, bounds.getWidth() - 2 * margin, headerHeight);

    // Overlay panels occupy the area below the header
    auto overlayArea = juce::Rectangle<int>(margin, headerHeight + margin, bounds.getWidth() - 2 * margin, bounds.getHeight() - headerHeight - 2 * margin);
    settingsDisplay.setBounds(overlayArea);
    infoDisplay.setBounds(overlayArea);

    // Calculate available space below the header and split between analyser and phaser params
    const int top = headerHeight + margin;
    const int totalBelow = bounds.getHeight() - top - margin; // area for analyser + phaser

    const int internalGap = margin;
    int analyserHeight = juce::roundToInt((totalBelow - internalGap) * 0.65f);
    int phaserHeight = (totalBelow - internalGap) - analyserHeight;

    // Ensure the phaser section has a minimum height so it's always visible
    const int phaserMin = 180;
    if (phaserHeight < phaserMin)
    {
        phaserHeight = juce::jmin(phaserMin, totalBelow - internalGap);
        analyserHeight = (totalBelow - internalGap) - phaserHeight;
    }

    // Ensure analyser has a sane minimum
    analyserHeight = juce::jmax(100, analyserHeight);

    // position spectrum analyser below header
    if (spectrumView)
        spectrumView->setBounds(10, top, bounds.getWidth() - 20, analyserHeight);

    // Place phaser controls below the analyser
    const int phaserY = top + analyserHeight + internalGap;
    phaserGroup.setVisible(true);
    phaserGroup.setBounds(10, phaserY, bounds.getWidth() - 20, juce::jmax(0, phaserHeight));

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

    // Ensure overlays reflect current flags on resize
    updateOverlaysVisibility();
}
