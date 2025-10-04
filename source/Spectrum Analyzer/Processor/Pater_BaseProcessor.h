#pragma once

#include <juce_audio_processors/juce_audio_processors.h>



namespace MarsDSP
{
    class BaseProcessor : public juce::AudioProcessor
    {
    public:
        BaseProcessor(juce::AudioProcessorValueTreeState& vts) : state(vts) { }
        ~BaseProcessor() override = default;

        void prepareToPlay(double /*sampleRate*/, int /*maximumExpectedSamplesPerBlock*/) override { }
        void releaseResources() override { }
        void processBlock(juce::AudioBuffer<float>& /*buffer*/, juce::MidiBuffer& /*midiMessages*/) override { }

        juce::AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }

        const juce::String getName() const override { return {}; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        double getTailLengthSeconds() const override { return 0; }

        int getNumPrograms() override { return 0; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int /*index*/) override { }
        const juce::String getProgramName(int /*index*/) override { return {}; }
        void changeProgramName(int /*index*/, const juce::String& /*newName*/) override { }

        void getStateInformation(juce::MemoryBlock& /*destData*/) override { }
        void setStateInformation(const void* /*data*/, int /*sizeInBytes*/) override { }

        juce::AudioProcessorValueTreeState& state;

        double sampleRate {0};

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProcessor)
    };
}
