#pragma once

#include "BinaryData.h"
#include "Pater_Gui.h"
#include "PluginProcessor.h"
#include "Pater_Params.h"
#include "Pater_RotaryKnob.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================

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

    MainLookAndFeel mainLF;

    PluginProcessor& patriarchProcessor;

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

    void updateDelayKnobs(bool tempoSyncActive);

    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
