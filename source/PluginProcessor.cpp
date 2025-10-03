#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Pater_DSPOption.h"
#include "Pater_Fifo.h"
#include "Pater_Params.h"

// compile-time constants, Integrate with createParameterLayout()

constexpr auto getPhaserRateName() { return "Phaser RateHz"; }
constexpr auto getPhaserCenterFreqName() { return "Phaser Center FreqHz"; }
constexpr auto getPhaserDepthName() { return "Phaser Depth%"; }
constexpr auto getPhaserFeedbackName() { return "Phaser Feedback%"; }
constexpr auto getPhaserWarmthName() { return "Phaser Warmth%"; }
constexpr auto getPhaserMixName() { return "Phaser Mix%"; }
//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), params(apvts)
{
    // Parameter pointers are retrieved via the APVTS when needed.
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare all DSP processors with the current playback configuration.
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = static_cast<double>(sampleRate);
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    dsp.phaser.prepare(spec);
    dsp.chorus.prepare(spec);
    dsp.reverb.prepare(spec);
    dsp.delay.prepare(spec);

    // Set a sensible default order in case none has been pushed yet.
    dsp.dspOrder = { DSPOption::DSP_Option::Phaser,
                     DSPOption::DSP_Option::Chorus,
                     DSPOption::DSP_Option::Reverb,
                     DSPOption::DSP_Option::Delay };
    currentDspOrder = dsp.dspOrder;
}

void PluginProcessor::releaseResources()
{
    // Reset processors to release any internal buffers.
    dsp.phaser.reset();
    dsp.chorus.reset();
    dsp.reverb.reset();
    dsp.delay.reset();
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const int versionHint = 1;

    auto name = getPhaserRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, versionHint}, name,
          juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f),
          0.2f, "Hz"));

    name = getPhaserDepthName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, versionHint}, name,
        juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
        0.05f, "%"));

    name = getPhaserFeedbackName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, versionHint}, name,
        juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),
        0.0f, "Hz"));

    name = getPhaserRateName();
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, versionHint}, name,
        juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f),
        0.05f, "%"));

    return layout;
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto newDSPOrder = DSPOption::DSP_Order();

    //try to pull
    while(dsp.dspOrderFifo.pull(newDSPOrder))
    {
    }
    //if you pulled, replace dspOrder;
    if (newDSPOrder != DSPOption::DSP_Order())
        currentDspOrder = newDSPOrder;

    //now convert dspOrder into an array of pointers.
    std::array<juce::dsp::ProcessorBase*, static_cast<size_t>(DSPOption::DSP_Option::END_OF_LIST)> dspPointers{};

    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
        switch (currentDspOrder[i])
        {
            case DSPOption::DSP_Option::Phaser:
                dspPointers[i] = &dsp.phaser;
                break;

            case DSPOption::DSP_Option::Chorus:
                dspPointers[i] = &dsp.chorus;
                break;

            case DSPOption::DSP_Option::Reverb:
                dspPointers[i] = &dsp.reverb;
                break;

            case DSPOption::DSP_Option::Delay:
                dspPointers[i] = &dsp.delay;
                break;

            case DSPOption::DSP_Option::END_OF_LIST:
                jassertfalse;
                break;
        }
    }
    //now process:
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    for (size_t i = 0; i < dspPointers.size(); ++i)
    {
        if (dspPointers[i] != nullptr)
        {
            dspPointers[i]->process(context);
        }
    }
}
//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
