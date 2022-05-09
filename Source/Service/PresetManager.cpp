#include "PresetManager.h"

namespace Service
{
	const File PresetManager::defaultDirectory{ File::getSpecialLocation(
		File::SpecialLocationType::commonDocumentsDirectory)
			.getChildFile(ProjectInfo::companyName)
			.getChildFile(ProjectInfo::projectName)
	};
	const String PresetManager::extension{ "preset" };
	const String PresetManager::presetNameProperty{ "presetName" };

	PresetManager::PresetManager(AudioProcessorValueTreeState& apvts) :
		valueTreeState(apvts)
	{
		// Create a default Preset Directory, if it doesn't exist
		if (!defaultDirectory.exists())
		{
			const auto result = defaultDirectory.createDirectory();
			if (result.failed())
			{
				DBG("Could not create preset directory: " + result.getErrorMessage());
				jassertfalse;
			}
		}

		valueTreeState.state.addListener(this);
		currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
	}

	void PresetManager::savePreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		currentPreset.setValue(presetName);
		const auto xml = valueTreeState.copyState().createXml();
		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!xml->writeTo(presetFile))
		{
			DBG("Could not create preset file: " + presetFile.getFullPathName());
			jassertfalse;
		}
	}

	void PresetManager::deletePreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		if (!presetFile.deleteFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " could not be deleted");
			jassertfalse;
			return;
		}
		currentPreset.setValue("");
	}

	void PresetManager::loadPreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		// presetFile (XML) -> (ValueTree)
		XmlDocument xmlDocument{ presetFile }; 
		const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

		valueTreeState.replaceState(valueTreeToLoad);
		currentPreset.setValue(presetName);

	}
    
    // cedrata overloads
    void PresetManager::deletePreset(const int& id)
    {
        const auto it = avaiablePresets.find(id);
        if (it == avaiablePresets.end())
        {
            DBG("Preset with ID " << id << " does not exist");
            jassertfalse;
            return;
        }
        
        if (it->second.deleteFile())
        {
            DBG("Prset file " + it->second.getFullPathName() + " could not be deleted");
            return;
        }
        
        currentPreset.setValue("");
    }

    void PresetManager::loadPreset(const int& id)
    {
        const auto it = avaiablePresets.find(id);
        if (it == avaiablePresets.end())
        {
            DBG("Preset with ID " << id << " does not exist");
            jassertfalse;
            return;
        }
        
        XmlDocument xmlDocument{ it->second };
        const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());
        
        valueTreeState.replaceState(valueTreeToLoad);
        currentPreset.setValue(it->second.getFileNameWithoutExtension());
    }
    // end cedrata overloads

	int PresetManager::loadNextPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
		loadPreset(allPresets.getReference(nextIndex));
		return nextIndex;
	}

	int PresetManager::loadPreviousPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
		loadPreset(allPresets.getReference(previousIndex));
		return previousIndex;
	}
    
    PopupMenu PresetManager::getPresetPopupMenu()
    {
        avaiablePresets.clear();
        return buildSubMenuRecursive(defaultDirectory);
    }

	StringArray PresetManager::getAllPresets() const
	{
		StringArray presets;
		const auto fileArray = defaultDirectory.findChildFiles(
			File::TypesOfFileToFind::findFiles, false, "*." + extension);
		for (const auto& file : fileArray)
		{
			presets.add(file.getFileNameWithoutExtension());
		}
		return presets;
	}

	String PresetManager::getCurrentPreset() const
	{
		return currentPreset.toString();
	}

	void PresetManager::valueTreeRedirected(ValueTree& treeWhichHasBeenChanged)
	{
		currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
	}
    
    PopupMenu PresetManager::buildSubMenuRecursive(File directoryToExplore)
    {
        PopupMenu subMenu;
        auto directoryPresets = directoryToExplore.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*." + extension, File::FollowSymlinks::no);
        directoryPresets.sort();
        
       for (auto p: directoryPresets)
       {
           const auto presetId = (int)avaiablePresets.size() + 1;
           avaiablePresets.insert(std::pair<int, File>(presetId, p));
           subMenu.addItem(PopupMenu::Item(p.getFileNameWithoutExtension())
                           .setID(presetId)
                           .setTicked(false)
                           .setEnabled(true));
       }
        
        auto directorySubMenus = directoryToExplore.findChildFiles(File::TypesOfFileToFind::findDirectories, false, "*", File::FollowSymlinks::no);
        
        for (auto s: directorySubMenus) subMenu.addSubMenu(s.getFileName(), buildSubMenuRecursive(s));
        
        return subMenu;
    }
}
