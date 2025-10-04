#pragma once
#include "Pater_BandController.h"
#include "../../PluginProcessor.h"



namespace MarsDSP
{
BandController::BandController(const int i, EQProcessor& p, MarsDSP::EqualizerProcessor& sub,
                               MarsDSP::BandView& v)
    : index(i), view(v), mainProcessor(p), processor(sub)
{
    auto& state              = mainProcessor.getPluginState();
    const auto& type_id      = processor.getTypeParamID(index);
    const auto& active_id    = processor.getActiveParamID(index);
    const auto& frequency_id = processor.getFrequencyParamID(index);
    const auto& quality_id   = processor.getQualityParamID(index);
    const auto& gain_id      = processor.getGainParamID(index);

    // Link GUI components to ValueTree
    using SliderAttachment   = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment   = juce::AudioProcessorValueTreeState::ButtonAttachment;

    // Type & Bypass
    boxAttachments.add(new ComboBoxAttachment(state, type_id, view.type));
    buttonAttachments.add(new ButtonAttachment(state, active_id, view.activate));

    // Slider
    attachments.add(new SliderAttachment(state, frequency_id, view.frequency));
    attachments.add(new SliderAttachment(state, quality_id, view.quality));
    attachments.add(new SliderAttachment(state, gain_id, view.gain));

    // Solo button
    const auto color = processor.getBandColour(index);
    view.solo.setColour(juce::TextButton::buttonOnColourId, color);
    view.solo.onClick = [&]() { processor.setBandSolo(view.solo.getToggleState() ? index : -1); };
}

void BandController::setUIControls(MarsDSP::EqualizerProcessor::FilterType type)
{
    switch (type)
    {
    case MarsDSP::EqualizerProcessor::LowPass:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(false);
        break;

    case MarsDSP::EqualizerProcessor::LowShelf:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(false);
        view.gain.setEnabled(true);
        break;
    case MarsDSP::EqualizerProcessor::BandPass:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(false);
        break;
    case MarsDSP::EqualizerProcessor::Peak:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(true);
        break;
    case MarsDSP::EqualizerProcessor::HighShelf:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(true);
        break;
    case MarsDSP::EqualizerProcessor::HighPass:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(false);
        break;
    default:
        view.frequency.setEnabled(true);
        view.quality.setEnabled(true);
        view.gain.setEnabled(true);
        break;
    }
}

void BandController::setSolo(bool isSolo) { view.solo.setToggleState(isSolo, juce::dontSendNotification); }

void BandController::setFrequency(float newFreq) { view.frequency.setValue(newFreq, juce::sendNotification); }

void BandController::setGain(float newGain) { view.gain.setValue(newGain, juce::sendNotification); }

void BandController::setType(int newType) { view.type.setSelectedId(newType + 1, juce::sendNotification); }

}
