#pragma once
#include "Pater_StereoPhaser.h"

StereoPhaser::StereoPhaser()
{
    phasers[0] = std::make_unique<SingleChannelPhaser> (magicState, "left_");
    phasers[1] = std::make_unique<SingleChannelPhaser> (magicState, "right_");
}

void StereoPhaser::addParameters (Parameters& params)
{
    SingleChannelPhaser::addParameters (params, "left_");
    SingleChannelPhaser::addParameters (params, "right_");
}

void StereoPhaser::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        phasers[ch]->prepareToPlay (sampleRate, samplesPerBlock);

    noModBuffer.setSize (1, samplesPerBlock);
}

void StereoPhaser::releaseResources()
{
}

void StereoPhaser::processBlock (AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    noModBuffer.setSize (1, numSamples, false, false, true);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* monoPtr = buffer.getWritePointer (ch);
        auto* noModPtr = noModBuffer.getWritePointer (0);
        auto* yMod = buffer.getWritePointer (ch);

        phasers[ch]->processBlock (monoPtr, yMod, noModPtr, numSamples);
    }
}

bool StereoPhaser::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // only supports stereo
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // input and output layout must be the same
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

// AudioProcessorEditor* StereoPhaser::createEditor()
// {
//     auto builder = std::make_unique<foleys::MagicGUIBuilder> (magicState);
//
//     builder->registerJUCEFactories();
//     builder->registerJUCELookAndFeels();
//
//     builder->registerLookAndFeel ("MyLNF", std::make_unique<MyLNF>());
//     builder->registerFactory ("SliderLink", &foleys::SliderLink::factory);
//     builder->registerFactory ("TooltipComp", &TooltipItem::factory);
//     builder->registerFactory ("InfoComp", &InfoItem::factory);
//     builder->registerFactory ("TitleComp", &TitleItem::factory);
//
//     return new foleys::MagicPluginEditor (magicState, BinaryData::stereo_gui_xml, BinaryData::stereo_gui_xmlSize, std::move (builder));
// }
//
// // This creates new instances of the plugin...
// AudioProcessor* JUCE_CALLTYPE createPluginFilter()
// {
//     return new StereoPhaser();
// }

