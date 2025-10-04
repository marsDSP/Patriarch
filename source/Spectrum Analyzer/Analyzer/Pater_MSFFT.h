#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_graphics/juce_graphics.h>

namespace MarsDSP
{
template <typename Type> class ModulationSourceAnalyser : public juce::Thread
{
public:
    ModulationSourceAnalyser() : juce::Thread("ModulationSource-Analyser"), abstractFifo(48000) { }
    ~ModulationSourceAnalyser() override = default;

    void addAudioData(const juce::AudioBuffer<Type>& buffer, int startChannel, int numChannels)
    {
        if (abstractFifo.getFreeSpace() < buffer.getNumSamples()) return;

        int start1, block1, start2, block2;
        abstractFifo.prepareToWrite(buffer.getNumSamples(), start1, block1, start2, block2);
        audioFifo.copyFrom(0, start1, buffer.getReadPointer(startChannel), block1);
        if (block2 > 0) audioFifo.copyFrom(0, start2, buffer.getReadPointer(startChannel, block1), block2);

        for (int channel = startChannel + 1; channel < startChannel + numChannels; ++channel)
        {
            if (block1 > 0) audioFifo.addFrom(0, start1, buffer.getReadPointer(channel), block1);
            if (block2 > 0) audioFifo.addFrom(0, start2, buffer.getReadPointer(channel, block1), block2);
        }
        abstractFifo.finishedWrite(block1 + block2);
        waitForData.signal();
    }

    void setupAnalyser(int audioFifoSize, Type sampleRateToUse)
    {
        sampleRate = sampleRateToUse;
        audioFifo.setSize(1, audioFifoSize);
        abstractFifo.setTotalSize(audioFifoSize);
        analyserBuffer.setSize(1, int(sampleRate));

        audioFifo.clear();
        analyserBuffer.clear();

        startThread();
    }

    void run() override
    {
        while (!threadShouldExit())
        {
            if (abstractFifo.getNumReady() >= int(sampleRate / 30))
            {
                analyserBuffer.clear();

                int start1, block1, start2, block2;
                abstractFifo.prepareToRead(int(sampleRate / 30), start1, block1, start2, block2);

                if (block1 > 0) analyserBuffer.copyFrom(0, 0, audioFifo.getReadPointer(0, start1), block1);
                if (block2 > 0) analyserBuffer.copyFrom(0, block1, audioFifo.getReadPointer(0, start2), block2);

                abstractFifo.finishedRead(block1 + block2);

                juce::ScopedLock lockedForWriting(pathCreationLock);

                newDataAvailable = true;
            }

            if (abstractFifo.getNumReady() < int(sampleRate / 30)) waitForData.wait(100);
        }
    }

    void createPath(juce::Path& p, const juce::Rectangle<float> bounds, float /*minFreq*/)
    {
        p.clear();
        juce::ScopedLock lockedForReading(pathCreationLock);

        const auto* reader    = analyserBuffer.getReadPointer(0);
        const auto numSamples = analyserBuffer.getNumSamples();

        const auto factor = bounds.getWidth() / 10.0f;

        p.startNewSubPath(bounds.getX(), ampToY(reader[1], bounds));

        for (int i = 0; i < numSamples; ++i)
        {
            p.lineTo(bounds.getX() + factor * indexToX(i, numSamples, bounds), ampToY(reader[i], bounds));
            i += 25;
        }
    }

    bool checkForNewData()
    {
        auto available   = newDataAvailable;
        newDataAvailable = false;
        return available;
    }

private:
    inline float indexToX(int index, int numSamples, const juce::Rectangle<float> bounds) const
    {
        return juce::jmap(static_cast<float>(index), 0.0f, static_cast<float>(numSamples), bounds.getX(), bounds.getRight());
    }

    inline float ampToY(float bin, const juce::Rectangle<float> bounds) const
    {
        return juce::jmap(bin, -1.f, 1.0f, bounds.getBottom(), bounds.getY());
    }

    Type sampleRate {};

    juce::AbstractFifo abstractFifo;
    juce::AudioBuffer<Type> audioFifo;
    juce::AudioBuffer<float> analyserBuffer;
    bool newDataAvailable = false;

    juce::WaitableEvent waitForData;
    juce::CriticalSection pathCreationLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationSourceAnalyser)
};
}
