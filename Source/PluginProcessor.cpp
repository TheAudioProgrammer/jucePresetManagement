#include "PluginProcessor.h"
#include "PluginEditor.h"

JucePresetManagerAudioProcessor::JucePresetManagerAudioProcessor() :
    AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    valueTreeState(*this, nullptr, ProjectInfo::projectName, Utility::ParameterHelper::createParameterLayout())
{
    valueTreeState.state.setProperty(Service::PresetManager::presetPathProperty, "", nullptr);
    valueTreeState.state.setProperty("version", ProjectInfo::versionString, nullptr);

    presetManager = std::make_unique<Service::PresetManager>(valueTreeState);
}

JucePresetManagerAudioProcessor::~JucePresetManagerAudioProcessor()
{
}

//==============================================================================
const juce::String JucePresetManagerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JucePresetManagerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JucePresetManagerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JucePresetManagerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JucePresetManagerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JucePresetManagerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JucePresetManagerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JucePresetManagerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JucePresetManagerAudioProcessor::getProgramName(int index)
{
    return {};
}

void JucePresetManagerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void JucePresetManagerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void JucePresetManagerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JucePresetManagerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
#endif

void JucePresetManagerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
}

//==============================================================================
bool JucePresetManagerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JucePresetManagerAudioProcessor::createEditor()
{
    return new JucePresetManagerAudioProcessorEditor(*this);
}

//==============================================================================
void JucePresetManagerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    const auto state = valueTreeState.copyState();
    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void JucePresetManagerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr)
        return;
    const auto newTree = ValueTree::fromXml(*xmlState);
    valueTreeState.replaceState(newTree);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JucePresetManagerAudioProcessor();
}
