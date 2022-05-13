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

		const auto xml = valueTreeState.copyState().createXml();
//		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        const auto presetFile = File(presetName);
		if (!xml->writeTo(presetFile))
		{
			DBG("Could not create preset file: " + presetFile.getFullPathName());
			jassertfalse;
        }
        
        currentPreset.setValue(presetFile.getFullPathName());
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
        currentPresetId = getCurrentPresetId("");
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
		XmlDocument xmlDocument{ presetFile }; 
		const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

		valueTreeState.replaceState(valueTreeToLoad);
		currentPreset.setValue(presetName);
        currentPresetId = getCurrentPresetId(presetName);
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
            currentPreset.setValue("");
            currentPresetId = getCurrentPresetId("");
            return;
        }
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
        currentPreset.setValue(it->second.getFullPathName());
        currentPresetId = id;
    }
    
    void PresetManager::loadNextPreset()
    {
        if (avaiablePresets.empty())
            return;
        
        if (currentPresetId == (int)avaiablePresets.size())
            currentPresetId = 1;
        else
            ++currentPresetId;
        
        loadPreset(currentPresetId);
    }

    void PresetManager::loadPreviousPreset()
    {
        
        if (avaiablePresets.empty())
            return;

        if (currentPresetId == 1)
            currentPresetId = (int)avaiablePresets.size();
        else
            --currentPresetId;
        
        loadPreset(currentPresetId);
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
    
    int PresetManager::getCurrentPresetId() const
    {
        return currentPresetId;
    }
    
    int PresetManager::getCurrentPresetId(const String &presetName) const
    {
        for (auto p: avaiablePresets)
        {
            if (p.second.getFullPathName() == presetName) return p.first;
        }
        return -1;
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
        
        for (auto s: directorySubMenus)
        {
            auto const recusiveSubMenu = buildSubMenuRecursive(s);
            if (recusiveSubMenu.getNumItems() > 0)
                subMenu.addSubMenu(s.getFileName(), recusiveSubMenu);
        }
        
        return subMenu;
    }
}
