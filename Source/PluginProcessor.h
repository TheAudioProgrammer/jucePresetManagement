#pragma once

#include <JuceHeader.h>
#include "Utility/ParameterHelper.h"
#include "Service/PresetManager.h"

class JucePresetManagerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    JucePresetManagerAudioProcessor();
    ~JucePresetManagerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    Service::PresetManager& getPresetManager() { return *presetManager; }

private:
    AudioProcessorValueTreeState valueTreeState;
    std::unique_ptr<Service::PresetManager> presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePresetManagerAudioProcessor)
};
