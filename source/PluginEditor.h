#pragma once

#include "BinaryData.h"
#include "Pater_Gui.h"
#include "Pater_Params.h"
#include "Pater_RotaryKnob.h"
#include "PluginProcessor.h"
#include "melatonin_inspector/melatonin_inspector.h"

// Spectrum Analyzer displays
#include "Spectrum Analyzer/Display/Pater_MenuDisplay.h"
#include "Spectrum Analyzer/Display/Pater_SettingsDisplay.h"
#include "Spectrum Analyzer/Display/Pater_InfoDisplay.h"

//==============================================================================

// Spectrum Analyzer: forward declaration of simple view component
class SpectrumAnalyserComponent;

class PluginEditor : public juce::AudioProcessorEditor,
                    private juce::AudioProcessorParameter::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override
    {
    }

    // Helpers
    void updateDelayKnobs(bool tempoSyncActive);
    void updateOverlaysVisibility();

    MainLookAndFeel mainLF;

    PluginProcessor& patriarchProcessor;

    // Top menu bar and overlays
    MarsDSP::MenuDisplay menuBar;
    MarsDSP::SettingsDisplay settingsDisplay;
    MarsDSP::InfoDisplay infoDisplay;
    bool showSettings = false;
    bool showInfo = false;

    juce::GroupComponent phaserGroup;

    RotaryKnob phaserRateHzKnob;
    RotaryKnob phaserNoteKnob;
    RotaryKnob phaserCenterFreqHzKnob;
    RotaryKnob phaserDepthPercentKnob;
    RotaryKnob phaserFeedbackPercentKnob;
    RotaryKnob phaserWarmthPercentKnob;
    RotaryKnob phaserMixPercentKnob;

    juce::TextButton tempoSyncButton;

    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment
    {
        patriarchProcessor.apvts, tempoSyncParamID.getParamID(), tempoSyncButton
    };

    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    // Spectrum Analyzer: UI component instance (drawn above parameters)
    std::unique_ptr<SpectrumAnalyserComponent> spectrumView; // added for Spectrum Analyzer

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
