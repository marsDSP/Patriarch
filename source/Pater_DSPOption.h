#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Pater_Fifo.h"

class DSPOption
{
public:

    DSPOption();
    ~DSPOption();

    enum class DSP_Option
    {
        Phaser,
        Chorus,
        Reverb,
        Delay,
        END_OF_LIST
    };

    using DSP_Order = std::array<DSP_Option, static_cast<size_t>(DSP_Option::END_OF_LIST)>;
    Pater_Fifo::Fifo<DSP_Order> dspOrderFifo;

public:

    DSP_Order dspOrder;

    template<typename DSP>

    struct DSP_Choice : juce::dsp::ProcessorBase
    {
        void prepare (const juce::dsp::ProcessSpec& spec) override
        {
            dsp.prepare(spec);
        }

        void process (const juce::dsp::ProcessContextReplacing<float>& context) override
        {
            dsp.process(context);
        }

        void reset() override
        {
            dsp.reset();
        }

        DSP dsp;
    };


    DSP_Choice<juce::dsp::Phaser<float>> phaser;
    DSP_Choice<juce::dsp::Chorus<float>> chorus;
    DSP_Choice<juce::dsp::Reverb> reverb;
    DSP_Choice<juce::dsp::DelayLine<float>> delay;

    using DSP_Pointers = std::array<juce::dsp::ProcessorBase*,
        static_cast<size_t>(DSP_Option::END_OF_LIST)>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DSPOption)

};
