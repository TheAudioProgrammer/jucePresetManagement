#pragma once

#include <JuceHeader.h>

namespace Service
{
	class PresetManager : ValueTree::Listener
	{
	public:
		static const File defaultDirectory;
		static const String extension;
		static const String presetNameProperty;

		PresetManager(AudioProcessorValueTreeState&);

		void savePreset(const String& presetName);
		void deletePreset(const String& presetName);
		void loadPreset(const String& presetName);
        // cedrata overloads
        void deletePreset(const int& id);
        void loadPreset(const int& id);
        // end cedrata overloads
		int loadNextPreset();
		int loadPreviousPreset();
        PopupMenu getPresetPopupMenu();
		StringArray getAllPresets() const;
		String getCurrentPreset() const;
	private:
		void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;
        PopupMenu buildSubMenuRecursive(File directoryToExplore);
        
        std::map<int, juce::File> avaiablePresets;
        
		AudioProcessorValueTreeState& valueTreeState;
		Value currentPreset;
	};
}
