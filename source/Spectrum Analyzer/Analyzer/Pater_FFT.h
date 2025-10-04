#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_graphics/juce_graphics.h>

namespace MarsDSP
{
template <typename Type> class FFTAnalyser : public juce::Thread
{
public:
    FFTAnalyser()
        : juce::Thread("PatriarchFFT")
        , abstractFifo(48000)
        , fft(12)
        , windowing(size_t(fft.getSize()), juce::dsp::WindowingFunction<Type>::kaiser)
    {
    }

    ~FFTAnalyser() override = default;

    void addAudioData(const juce::AudioBuffer<Type>& buffer, int startChannel, int numChannels)
    {
        // Return if not enough space is available
        if (abstractFifo.getFreeSpace() < buffer.getNumSamples()) { return; }

        int start1 {};
        int block1 {};
        int start2 {};
        int block2 {};
        abstractFifo.prepareToWrite(buffer.getNumSamples(), start1, block1, start2, block2);
        audioFifo.copyFrom(0, start1, buffer.getReadPointer(startChannel), block1);
        if (block2 > 0) { audioFifo.copyFrom(0, start2, buffer.getReadPointer(startChannel, block1), block2); }

        for (int channel = startChannel + 1; channel < startChannel + numChannels; ++channel)
        {
            if (block1 > 0) { audioFifo.addFrom(0, start1, buffer.getReadPointer(channel), block1); }
            if (block2 > 0) { audioFifo.addFrom(0, start2, buffer.getReadPointer(channel, block1), block2); }
        }
        abstractFifo.finishedWrite(block1 + block2);
        waitForData.signal();
    }

    void setupAnalyser(int audioFifoSize, Type sampleRateToUse)
    {
        sampleRate = sampleRateToUse;
        audioFifo.setSize(1, audioFifoSize);
        abstractFifo.setTotalSize(audioFifoSize);
        fftBuffer.setSize(1, fft.getSize() * 2);
        averager.setSize(5, fft.getSize() / 2, false, true);

        startThread();
    }

    void run() override
    {
        while (!threadShouldExit())
        {
            if (abstractFifo.getNumReady() >= fft.getSize())
            {
                fftBuffer.clear();

                int start1 {};
                int block1 {};
                int start2 {};
                int block2 {};

                abstractFifo.prepareToRead(fft.getSize(), start1, block1, start2, block2);
                if (block1 > 0) { fftBuffer.copyFrom(0, 0, audioFifo.getReadPointer(0, start1), block1); }
                if (block2 > 0) { fftBuffer.copyFrom(0, block1, audioFifo.getReadPointer(0, start2), block2); }
                abstractFifo.finishedRead(block1 + block2);

                windowing.multiplyWithWindowingTable(fftBuffer.getWritePointer(0), size_t(fft.getSize()));
                fft.performFrequencyOnlyForwardTransform(fftBuffer.getWritePointer(0));

                juce::ScopedLock lockedForWriting(pathCreationLock);
                averager.addFrom(0, 0, averager.getReadPointer(averagerPtr), averager.getNumSamples(), -1.0f);
                averager.copyFrom(averagerPtr, 0, fftBuffer.getReadPointer(0), averager.getNumSamples(),
                                  1.0f / (averager.getNumSamples() * (averager.getNumChannels() - 1)));
                averager.addFrom(0, 0, averager.getReadPointer(averagerPtr), averager.getNumSamples());
                if (++averagerPtr == averager.getNumChannels()) { averagerPtr = 1; }

                newDataAvailable = true;
            }

            if (abstractFifo.getNumReady() < fft.getSize()) { waitForData.wait(100); }
        }
    }

    void createPath(juce::Path& p, const juce::Rectangle<float> bounds, float minFreq)
    {
        p.clear();

        juce::ScopedLock lockedForReading(pathCreationLock);
        const auto* fftData = averager.getReadPointer(0);
        const auto factor   = bounds.getWidth() / 10.0f;

        p.startNewSubPath(bounds.getX() + factor * indexToX(0, minFreq), binToY(fftData[0], bounds));
        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            const auto x = bounds.getX() + factor * indexToX(static_cast<float>(i), minFreq);
            const auto y = binToY(fftData[i], bounds);

            p.lineTo(static_cast<float>(x), static_cast<float>(y));
        }
    }

    bool checkForNewData()
    {
        auto available   = newDataAvailable;
        newDataAvailable = false;
        return available;
    }

private:
    inline float indexToX(float index, float minFreq) const
    {
        const auto freq = (sampleRate * index) / fft.getSize();
        return (freq > 0.01f) ? std::log(freq / minFreq) / std::log(2.0f) : 0.0f;
    }

    inline float binToY(float bin, const juce::Rectangle<float> bounds) const
    {
        const float infinity = -80.0f;
        return juce::jmap(juce::Decibels::gainToDecibels(bin, infinity), infinity, 0.0f, bounds.getBottom(), bounds.getY());
    }

    Type sampleRate {};

    juce::AbstractFifo abstractFifo;
    juce::AudioBuffer<Type> audioFifo;
    juce::AudioBuffer<float> fftBuffer;
    juce::AudioBuffer<float> averager;
    int averagerPtr       = 1;
    bool newDataAvailable = false;

    juce::WaitableEvent waitForData;
    juce::CriticalSection pathCreationLock;

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<Type> windowing;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTAnalyser)
};
}