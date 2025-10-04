#pragma once
#include "Pater_MSController.h"
#include "../../PluginProcessor.h"

namespace MarsDSP
{
    ModulationSourceController::ModulationSourceController(const int i, EQProcessor& mp, ModulationSourceProcessor& p,
                                                           ModulationSourceView& v)
        : index(i), connectViewActive(false), mainProcessor(mp), processor(p), view(v)
    {
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        auto& state            = mainProcessor.getPluginState();

        attachments.add(new SliderAttachment(state, "lfo_" + juce::String(index) + "_freq", view.frequency));
        attachments.add(new SliderAttachment(state, "lfo_" + juce::String(index) + "_gain", view.gain));

        // view.modConnect1.setVisible(connectViewActive);  // TODO: modConnect members not implemented
        // view.modConnect2.setVisible(connectViewActive);
        view.toggleConnectView.onClick = [&]() {
            connectViewActive = !connectViewActive;
            // view.modConnect1.setVisible(connectViewActive);
            // view.modConnect2.setVisible(connectViewActive);
        };

        view.frequency.addListener(this);
        view.gain.addListener(this);

        startTimerHz(GLOBAL_REFRESH_RATE_HZ);
    }

    void ModulationSourceController::sliderValueChanged(juce::Slider* slider)
    {
        auto& frequency = view.frequency;
        auto& gain      = view.gain;
        auto& gainLabel = view.gainLabel;
        auto& freqLabel = view.freqLabel;

        if (slider == &frequency)
        { freqLabel.setText(frequency.getTextFromValue(frequency.getValue()), juce::NotificationType::dontSendNotification); }

        if (slider == &gain)
        { gainLabel.setText(gain.getTextFromValue(gain.getValue()), juce::NotificationType::dontSendNotification); }
    }
    void ModulationSourceController::timerCallback()
    {
        if (processor.checkForNewAnalyserData())
        { processor.createAnalyserPlot(view.modulationPath, view.plotFrame, 20.0f); }
        view.repaint(view.plotFrame);
    }
}


