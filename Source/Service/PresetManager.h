#pragma once

#include <JuceHeader.h>

namespace Service
{
	class PresetManager : ValueTree::Listener
	{
	public:
		static const File defaultDirectory;
		static const String extension;
		static const String presetPathProperty;

		PresetManager(AudioProcessorValueTreeState&);

		void savePreset(const String& presetName);
        void deletePreset(const int& id);
        void loadPreset(const int& id);
        void loadNextPreset();
        void loadPreviousPreset();
        PopupMenu getPresetPopupMenu();
		StringArray getAllPresets() const;
		String getCurrentPreset() const;
        int getCurrentPresetId() const;
	private:
		void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;
        PopupMenu buildSubMenuRecursive(File directoryToExplore);
        void setCurrentPresetId();
        
        // Assumption 1: the keys in this map will always be ordered from 1 to N
        // where N is the size of the map. The keys are unique because of the way
        // the map is built inside the buildSuMenuRecursive method.
        // Given this assumption it will be easier to find the next and
        // the previous preset in this map given the currentPresetId.
        // Assumption 2: the values int this map will always be
        // ordered and unique because of the way the map is constructed
        // inside the buildSubMenuRecursive method. This last assumption will
        // be usefull while searching items in map by value and not key.
        std::map<int, juce::File> avaiablePresets;
        
		AudioProcessorValueTreeState& valueTreeState;
		Value currentPreset;
        int currentPresetId;
	};
}
