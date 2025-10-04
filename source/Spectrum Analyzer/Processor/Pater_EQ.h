#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Pater_BaseProcessor.h"
#include "../Pater_AnalyzerTypes.h"
#include "../Params/Pater_ValueConverter.h"
#include "../Params/Pater_FilterConstants.h"



namespace MarsDSP
{
class EqualizerProcessor : public BaseProcessor, public juce::ChangeBroadcaster, public juce::AudioProcessorValueTreeState::Listener
{
public:
    enum FilterType
    {
        NoFilter = 0,
        HighPass,
        LowShelf,
        BandPass,
        Peak,
        HighShelf,
        LowPass,
        LastFilterID
    };

    struct Band
    {
        juce::String name;
        juce::Colour colour;
        FilterType type = BandPass;
        float frequency = 1000.0f;
        float quality   = 1.0f;
        float gain      = 1.0f;
        bool active     = true;
        bool selected   = false;
        std::vector<double> magnitudes;
    };

    EqualizerProcessor(juce::AudioProcessorValueTreeState& vts);
    ~EqualizerProcessor() override;

    void prepareToPlay(double /*unused*/, int /*unused*/) override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    void parameterChanged(const juce::String& parameter, float newValue) override;

    static juce::String getFilterTypeName(MarsDSP::EqualizerProcessor::FilterType type);

    const juce::String getName() const override { return "Equalizer"; }

    Band* getBand(int index);

    void updateBand(size_t index);

    void updateBypassedStates();

    void updatePlots();

    const std::vector<double>& getMagnitudes();

    void createFrequencyPlot(juce::Path& p, const std::vector<double>& mags, juce::Rectangle<int> bounds, float pixelsPerDouble);

    void createAnalyserPlot(juce::Path& p, juce::Rectangle<int> bounds, float minFreq, bool input);

    bool checkForNewAnalyserData();

    juce::String getTypeParamID(int index) const;

    juce::String getFrequencyParamID(int index) const;

    juce::String getQualityParamID(int index) const;

    juce::String getGainParamID(int index) const;

    juce::String getActiveParamID(int index) const;

    int getNumBands() const;

    juce::String getBandName(int index) const;

    juce::Colour getBandColour(int index) const;

    void setBandSolo(int index);

    bool getBandSolo(int index) const;

    void setSelectedBand(int index);

    int getSelectedBand();

private:
    int soloed       = -1;
    bool wasBypassed = true;

    using FloatFilter       = juce::dsp::IIR::Filter<float>;
    using FloatCoefficients = juce::dsp::IIR::Coefficients<float>;
    using FBand             = juce::dsp::ProcessorDuplicator<FloatFilter, FloatCoefficients>;

    juce::dsp::ProcessorChain<FBand, FBand, FBand, FBand, FBand, FBand> filter;
    std::vector<Band> bands;

    std::vector<double> frequencies;
    std::vector<double> magnitudes;

    MarsDSP::SpectrumAnalyser<float> inputAnalyser;
    MarsDSP::SpectrumAnalyser<float> outputAnalyser;

    MarsDSP::GainTextConverter gainTextConverter;
    MarsDSP::ActiveTextConverter activeTextConverter;
    MarsDSP::QualityTextConverter qualityTextConverter;
    MarsDSP::FrequencyTextConverter frequencyTextConverter;
    MarsDSP::FilterTypeTextConverter filterTypeTextConverter;

    void setDefaults();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerProcessor)
};
}

