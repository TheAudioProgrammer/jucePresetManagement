#include "PluginProcessor.h"
#include "PluginEditor.h"

JucePresetManagerAudioProcessorEditor::JucePresetManagerAudioProcessorEditor (JucePresetManagerAudioProcessor& p) :
	AudioProcessorEditor(&p),
	genericAudioProcessorEditor(p),
	presetPanel(p.getPresetManager()),
    audioProcessor(p)
{
    addAndMakeVisible(genericAudioProcessorEditor);
    addAndMakeVisible(presetPanel);

    setResizable(true, true);
    setSize (600, 500);
}

JucePresetManagerAudioProcessorEditor::~JucePresetManagerAudioProcessorEditor()
{
}

void JucePresetManagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JucePresetManagerAudioProcessorEditor::resized()
{
    genericAudioProcessorEditor.setBounds(getLocalBounds()
        .withSizeKeepingCentre(getLocalBounds().proportionOfWidth(0.9f), getLocalBounds().proportionOfHeight(0.5f)));
    presetPanel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1f)));
}
