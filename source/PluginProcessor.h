#pragma once

#include "Pater_DSPOption.h"
#include "Pater_Params.h"
#include <juce_audio_processors/juce_audio_processors.h>
// Spectrum Analyzer: include FFT analyser helper
#include "Spectrum Analyzer/Analyzer/Pater_FFT.h"

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", Parameters::createParameterLayout() };

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Spectrum Analyzer: expose simple helper API for the editor
    bool checkForNewAnalyserData() const;
    void createAnalyserPlot(juce::Path& p, juce::Rectangle<float> bounds, float minFreq) const;
    
    // Provide access to parameter tree for EQ controllers
    juce::AudioProcessorValueTreeState& getPluginState() { return apvts; }

private:

    DSPOption dsp;
    DSPOption::DSP_Order currentDspOrder{};

    Parameters params;

    // Spectrum Analyzer: analyser instance fed from processBlock
    std::unique_ptr<MarsDSP::FFTAnalyser<float>> spectrumAnalyser; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
