#pragma once

#include "PluginProcessor.h"
// Try to include melatonin inspector headers if available
#if __has_include(<melatonin_inspector/melatonin_inspector.h>)
#include "melatonin_inspector/melatonin_inspector.h"
#define HAS_MELATONIN_INSPECTOR 1
#else
#define HAS_MELATONIN_INSPECTOR 0
#endif

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
#if HAS_MELATONIN_INSPECTOR
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
#endif
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
