#pragma once
#include "Pater_MSProcessor.h"


namespace MarsDSP
{
    ModulationSourceProcessor::ModulationSourceProcessor(int i, juce::AudioProcessorValueTreeState& vts)
        : BaseProcessor(vts)
        , index(i)
        , paramIDGain("lfo_" + juce::String(index) + "_gain")
        , paramIDFrequency("lfo_" + juce::String(index) + "_freq")
    {
        oscillator.setFrequency(1.f);
        oscillator.initialise([](float x) { return std::sin(x); });
    }

    ModulationSourceProcessor::~ModulationSourceProcessor() { analyser.stopThread(1000); }

    void ModulationSourceProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
    {
        sampleRate = newSampleRate;

        auto spec             = juce::dsp::ProcessSpec {};
        spec.sampleRate       = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels      = static_cast<juce::uint32>(getTotalNumOutputChannels());

        oscillator.prepare(spec);
        gain.prepare(spec);

        auto freqValue = state.getRawParameterValue(paramIDFrequency)->load();
        oscillator.setFrequency(freqValue);

        analyser.setupAnalyser(int(sampleRate), float(sampleRate));
    }

    void ModulationSourceProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
    {
        float freqValue = state.getRawParameterValue(paramIDFrequency)->load();
        float gainValue = state.getRawParameterValue(paramIDGain)->load();

        oscillator.setFrequency(freqValue);
        gain.setGainLinear(gainValue);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        oscillator.process(context);
        gain.process(context);

        analyser.addAudioData(buffer, 0, 1);
    }

    void ModulationSourceProcessor::parameterChanged(const juce::String& /*parameter*/, float /*newValue*/) { }

    void ModulationSourceProcessor::createAnalyserPlot(juce::Path& p, juce::Rectangle<int>& bounds, float minFreq)
    {
        analyser.createPath(p, bounds.toFloat(), minFreq);
    }

    bool ModulationSourceProcessor::checkForNewAnalyserData() { return analyser.checkForNewData(); }

}