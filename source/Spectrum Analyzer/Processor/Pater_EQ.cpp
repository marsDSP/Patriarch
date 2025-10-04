#pragma once
#include "Pater_EQ.h"
#include "../Params/Pater_FFTParams.h"

namespace MarsDSP
{
EqualizerProcessor::EqualizerProcessor(juce::AudioProcessorValueTreeState& vts) : BaseProcessor(vts)
{
    frequencies.resize(300);
    for (size_t i = 0; i < frequencies.size(); ++i) { frequencies[i] = 20.0 * std::pow(2.0, i / 30.0); }
    magnitudes.resize(frequencies.size());

    bands.resize(6);

    setDefaults();

    // Create Ranges for parameters
    using MarsDSP::FILTER_FREQ_MAX;
    using MarsDSP::FILTER_FREQ_MIN;
    using MarsDSP::FILTER_FREQ_STEP_SIZE;
    using MarsDSP::FILTER_GAIN_STEP_SIZE;
    using MarsDSP::FILTER_Q_MAX;
    using MarsDSP::FILTER_Q_MIN;
    using MarsDSP::FILTER_Q_STEP_SIZE;

    juce::NormalisableRange<float> filterTypeRange(0, MarsDSP::EqualizerProcessor::LastFilterID - 1, 1);
    juce::NormalisableRange<float> frequencyRange(FILTER_FREQ_MIN, FILTER_FREQ_MAX, FILTER_FREQ_STEP_SIZE);
    juce::NormalisableRange<float> qualityRange(FILTER_Q_MIN, FILTER_Q_MAX, FILTER_Q_STEP_SIZE);
    juce::NormalisableRange<float> gainRange(1.0f / MAX_GAIN, MAX_GAIN, FILTER_GAIN_STEP_SIZE);
    juce::NormalisableRange<float> activeRange(0, 1, 1);

    frequencyRange.setSkewForCentre(2000.0f);
    qualityRange.setSkewForCentre(1.0f);
    gainRange.setSkewForCentre(1.0f);

    for (int i = 0; i < static_cast<int>(bands.size()); ++i)
    {
        auto& band = bands[size_t(i)];

        // Generate random rgb colour
        const auto colour = []() -> juce::Colour {
            auto& random = juce::Random::getSystemRandom();
            const auto r = static_cast<juce::uint8>(random.nextInt(256));
            const auto g = static_cast<juce::uint8>(random.nextInt(256));
            const auto b = static_cast<juce::uint8>(random.nextInt(256));
            return juce::Colour(r, g, b).brighter(0.6f);
        }();

        band.colour = colour;
        band.magnitudes.resize(frequencies.size(), 1.0);

        // ValueTree parameters
        using Parameter = juce::AudioProcessorValueTreeState::Parameter;

        state.createAndAddParameter(std::make_unique<Parameter>(
            getFrequencyParamID(i), band.name + " freq", "Frequency", frequencyRange, band.frequency,
            frequencyTextConverter, frequencyTextConverter, false, true, false));
        state.createAndAddParameter(
            std::make_unique<Parameter>(getQualityParamID(i), band.name + " Q", juce::translate("Quality"), qualityRange,
                                        band.quality, qualityTextConverter, qualityTextConverter, false, true, false));
        state.createAndAddParameter(
            std::make_unique<Parameter>(getGainParamID(i), band.name + " gain", juce::translate("Gain"), gainRange, band.gain,
                                        gainTextConverter, gainTextConverter, false, true, false));
        state.createAndAddParameter(
            std::make_unique<Parameter>(getActiveParamID(i), band.name + " active", juce::translate("Active"), activeRange,
                                        band.active, activeTextConverter, activeTextConverter, false, true, true));

        state.createAndAddParameter(std::make_unique<Parameter>(
            getTypeParamID(i), band.name + " Type", juce::translate("Filter Type"), filterTypeRange,
            static_cast<float>(band.type), filterTypeTextConverter, filterTypeTextConverter, false, true, true));

        state.addParameterListener(getTypeParamID(i), this);
        state.addParameterListener(getFrequencyParamID(i), this);
        state.addParameterListener(getQualityParamID(i), this);
        state.addParameterListener(getGainParamID(i), this);
        state.addParameterListener(getActiveParamID(i), this);
    }
}

EqualizerProcessor::~EqualizerProcessor()
{
    inputAnalyser.stopThread(1000);
    outputAnalyser.stopThread(1000);
}

void EqualizerProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;

    for (size_t i = 0; i < bands.size(); ++i) { updateBand(i); }

    updatePlots();

    inputAnalyser.setupAnalyser(int(sampleRate), float(sampleRate));
    outputAnalyser.setupAnalyser(int(sampleRate), float(sampleRate));

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = newSampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels      = static_cast<juce::uint32>(getTotalNumOutputChannels());

    filter.prepare(spec);
}

void EqualizerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer)
{
    juce::ignoreUnused(midiBuffer);

    inputAnalyser.addAudioData(buffer, 0, getTotalNumInputChannels());

    if (wasBypassed)
    {
        filter.reset();
        wasBypassed = false;
    }

    auto ioBuffer = juce::dsp::AudioBlock<float> {buffer};
    auto context  = juce::dsp::ProcessContextReplacing<float> {ioBuffer};
    filter.process(context);

    outputAnalyser.addAudioData(buffer, 0, getTotalNumOutputChannels());
}

void EqualizerProcessor::parameterChanged(const juce::String& parameter, float newValue)
{
    for (size_t i = 0; i < bands.size(); ++i)
    {
        if (parameter.startsWith(getBandName(int(i)) + "-"))
        {
            if (parameter.endsWith(MarsDSP::Parameters::Type))
            { bands[i].type = static_cast<FilterType>(static_cast<int>(newValue)); }
            else if (parameter.endsWith(MarsDSP::Parameters::Frequency))
            {
                bands[i].frequency = newValue;
            }
            else if (parameter.endsWith(MarsDSP::Parameters::Quality))
            {
                bands[i].quality = newValue;
            }
            else if (parameter.endsWith(MarsDSP::Parameters::Gain))
            {
                bands[i].gain = newValue;
            }
            else if (parameter.endsWith(MarsDSP::Parameters::Active))
            {
                bands[i].active = newValue >= 0.5f;
            }

            updateBand(i);
            return;
        }
    }
}

juce::String EqualizerProcessor::getFilterTypeName(const EqualizerProcessor::FilterType type)
{
    switch (type)
    {
    case NoFilter:
        return juce::translate("No Filter");
    case HighPass:
        return juce::translate("High Pass");
    case LowShelf:
        return juce::translate("Low Shelf");
    case BandPass:
        return juce::translate("Band Pass");
    case Peak:
        return juce::translate("Peak");
    case HighShelf:
        return juce::translate("High Shelf");
    case LowPass:
        return juce::translate("Low Pass");
    default:
        return juce::translate("unknown");
    }
}

int EqualizerProcessor::getNumBands() const { return static_cast<int>(bands.size()); }

juce::String EqualizerProcessor::getBandName(const int index) const
{
    if (juce::isPositiveAndBelow(index, bands.size())) { return bands[size_t(index)].name; }
    return juce::translate("unknown");
}
juce::Colour EqualizerProcessor::getBandColour(const int index) const
{
    if (juce::isPositiveAndBelow(index, bands.size())) { return bands[size_t(index)].colour; }
    return juce::Colours::silver;
}

void EqualizerProcessor::setBandSolo(const int index)
{
    soloed = index;
    updateBypassedStates();
}

void EqualizerProcessor::updateBypassedStates()
{
    if (juce::isPositiveAndBelow(soloed, bands.size()))
    {
        filter.setBypassed<0>(soloed != 0);
        filter.setBypassed<1>(soloed != 1);
        filter.setBypassed<2>(soloed != 2);
        filter.setBypassed<3>(soloed != 3);
        filter.setBypassed<4>(soloed != 4);
        filter.setBypassed<5>(soloed != 5);
    }
    else
    {
        filter.setBypassed<0>(!bands[0].active);
        filter.setBypassed<1>(!bands[1].active);
        filter.setBypassed<2>(!bands[2].active);
        filter.setBypassed<3>(!bands[3].active);
        filter.setBypassed<4>(!bands[4].active);
        filter.setBypassed<5>(!bands[5].active);
    }
    updatePlots();
}

void EqualizerProcessor::updatePlots()
{
    const auto gain = 1.0f;
    std::fill(magnitudes.begin(), magnitudes.end(), gain);

    if (juce::isPositiveAndBelow(soloed, bands.size()))
    {
        juce::FloatVectorOperations::multiply(magnitudes.data(), bands[static_cast<size_t>(soloed)].magnitudes.data(),
                                        static_cast<int>(magnitudes.size()));
    }
    else
    {
        for (const auto& band : bands)
        {
            if (band.active)
            {
                juce::FloatVectorOperations::multiply(magnitudes.data(), band.magnitudes.data(),
                                                static_cast<int>(magnitudes.size()));
            }
        }
    }

    sendChangeMessage();
}

void EqualizerProcessor::setSelectedBand(int index)
{
    // Set all bands to not selected
    std::for_each(bands.begin(), bands.end(), [](auto& item) { item.selected = false; });

    // Return if no band (-1) was selected
    if (index == -1) { return; }

    // Select new band
    bands.at(index).selected = true;
    DBG("Selected: " + juce::String(index));
}

int EqualizerProcessor::getSelectedBand()
{
    for (size_t i = 0; i < bands.size(); ++i)
    {
        if (bands[i].selected) { return static_cast<int>(i); }
    }

    return -1;
}

bool EqualizerProcessor::getBandSolo(const int index) const { return index == soloed; }

void EqualizerProcessor::setDefaults()
{
    // setting defaults
    {
        auto& band     = bands[0];
        band.name      = "Lowest";
        band.colour    = juce::Colours::blue;
        band.frequency = 20.0f;
        band.quality   = 0.707f;
        band.type      = MarsDSP::EqualizerProcessor::HighPass;
    }
    {
        auto& band     = bands[1];
        band.name      = "Low";
        band.colour    = juce::Colours::brown;
        band.frequency = 250.0f;
        band.type      = MarsDSP::EqualizerProcessor::LowShelf;
    }
    {
        auto& band     = bands[2];
        band.name      = "Low Mids";
        band.colour    = juce::Colours::green;
        band.frequency = 500.0f;
        band.type      = MarsDSP::EqualizerProcessor::Peak;
    }
    {
        auto& band     = bands[3];
        band.name      = "High Mids";
        band.colour    = juce::Colours::coral;
        band.frequency = 1000.0f;
        band.type      = MarsDSP::EqualizerProcessor::Peak;
    }
    {
        auto& band     = bands[4];
        band.name      = "High";
        band.colour    = juce::Colours::orange;
        band.frequency = 5000.0f;
        band.type      = MarsDSP::EqualizerProcessor::HighShelf;
    }
    {
        auto& band     = bands[5];
        band.name      = "Highest";
        band.colour    = juce::Colours::red;
        band.frequency = 12000.0f;
        band.quality   = 0.707f;
        band.type      = MarsDSP::EqualizerProcessor::LowPass;
    }
}

EqualizerProcessor::Band* EqualizerProcessor::getBand(const int index)
{
    if (juce::isPositiveAndBelow(index, bands.size())) { return &bands[size_t(index)]; }
    return nullptr;
}

void EqualizerProcessor::updateBand(const size_t index)
{
    if (sampleRate > 0)
    {
        juce::dsp::IIR::Coefficients<float>::Ptr newCoefficients;
        switch (bands[index].type)
        {
        case MarsDSP::EqualizerProcessor::NoFilter:
            newCoefficients = new juce::dsp::IIR::Coefficients<float>(1, 0, 1, 0);
            break;
        case MarsDSP::EqualizerProcessor::LowPass:
            newCoefficients
                = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, bands[index].frequency, bands[index].quality);
            break;
        case MarsDSP::EqualizerProcessor::LowShelf:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, bands[index].frequency,
                                                                          bands[index].quality, bands[index].gain);
            break;
        case MarsDSP::EqualizerProcessor::BandPass:
            newCoefficients
                = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, bands[index].frequency, bands[index].quality);
            break;
        case MarsDSP::EqualizerProcessor::Peak:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, bands[index].frequency,
                                                                            bands[index].quality, bands[index].gain);
            break;
        case MarsDSP::EqualizerProcessor::HighShelf:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, bands[index].frequency,
                                                                           bands[index].quality, bands[index].gain);
            break;
        case MarsDSP::EqualizerProcessor::HighPass:
            newCoefficients
                = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, bands[index].frequency, bands[index].quality);
            break;
        default:
            break;
        }

        if (newCoefficients != nullptr)
        {
            {
                // minimise lock scope, get<0>() needs to be a  compile time
                // constant
                juce::ScopedLock processLock(getCallbackLock());
                if (index == 0) { *filter.get<0>().state = *newCoefficients; }
                else if (index == 1)
                {
                    *filter.get<1>().state = *newCoefficients;
                }
                else if (index == 2)
                {
                    *filter.get<2>().state = *newCoefficients;
                }
                else if (index == 3)
                {
                    *filter.get<3>().state = *newCoefficients;
                }
                else if (index == 4)
                {
                    *filter.get<4>().state = *newCoefficients;
                }
                else if (index == 5)
                {
                    *filter.get<5>().state = *newCoefficients;
                }
            }
            newCoefficients->getMagnitudeForFrequencyArray(frequencies.data(), bands[index].magnitudes.data(),
                                                           frequencies.size(), sampleRate);
        }
        updateBypassedStates();
        updatePlots();
    }
}

juce::String EqualizerProcessor::getTypeParamID(const int index) const
{
    return getBandName(index) + "-" + MarsDSP::Parameters::Type;
}

juce::String EqualizerProcessor::getFrequencyParamID(const int index) const
{
    return getBandName(index) + "-" + MarsDSP::Parameters::Frequency;
}

juce::String EqualizerProcessor::getQualityParamID(const int index) const
{
    return getBandName(index) + "-" + MarsDSP::Parameters::Quality;
}

juce::String EqualizerProcessor::getGainParamID(const int index) const
{
    return getBandName(index) + "-" + MarsDSP::Parameters::Gain;
}

juce::String EqualizerProcessor::getActiveParamID(const int index) const
{
    return getBandName(index) + "-" + MarsDSP::Parameters::Active;
}

const std::vector<double>& EqualizerProcessor::getMagnitudes() { return magnitudes; }

void EqualizerProcessor::createFrequencyPlot(juce::Path& p, const std::vector<double>& mags, juce::Rectangle<int> bounds,
                                             float pixelsPerDouble)
{
    p.startNewSubPath(
        static_cast<float>(bounds.getX()),
        static_cast<float>(juce::roundToInt(bounds.getCentreY() - pixelsPerDouble * std::log(mags[0]) / std::log(2))));
    const double xFactor = static_cast<double>(bounds.getWidth()) / frequencies.size();
    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        const auto x = juce::roundToInt(bounds.getX() + i * xFactor);
        const auto y = juce::roundToInt(bounds.getCentreY() - pixelsPerDouble * std::log(mags[i]) / std::log(2));

        p.lineTo(static_cast<float>(x), static_cast<float>(y));
    }
}

void EqualizerProcessor::createAnalyserPlot(juce::Path& p, juce::Rectangle<int> bounds, float minFreq, bool input)
{
    if (input) { inputAnalyser.createPath(p, bounds.toFloat(), minFreq); }
    else
    {
        outputAnalyser.createPath(p, bounds.toFloat(), minFreq);
    }
}

bool EqualizerProcessor::checkForNewAnalyserData()
{
    return inputAnalyser.checkForNewData() || outputAnalyser.checkForNewData();
}
}

