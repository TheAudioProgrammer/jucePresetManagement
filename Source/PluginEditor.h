#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class JucePresetManagerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JucePresetManagerAudioProcessorEditor (JucePresetManagerAudioProcessor&);
    ~JucePresetManagerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GenericAudioProcessorEditor genericAudioProcessorEditor;
    JucePresetManagerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePresetManagerAudioProcessorEditor)
};
