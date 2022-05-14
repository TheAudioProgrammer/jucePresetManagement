#include "PresetManager.h"

namespace Service
{
	const File PresetManager::defaultDirectory{ File::getSpecialLocation(
		File::SpecialLocationType::commonDocumentsDirectory)
			.getChildFile(ProjectInfo::companyName)
			.getChildFile(ProjectInfo::projectName)
	};
	const String PresetManager::extension{ "preset" };
	const String PresetManager::presetPathProperty{ "presetPath" };

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
		currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetPathProperty, nullptr));
	}

	void PresetManager::savePreset(const String& presetPath)
	{
		if (presetPath.isEmpty())
			return;

		const auto xml = valueTreeState.copyState().createXml();
//		const auto presetFile = defaultDirectory.getChildFile(presetPath + "." + extension);
        const auto presetFile = File(presetPath);
		if (!xml->writeTo(presetFile))
		{
			DBG("Could not create preset file: " + presetFile.getFullPathName());
			jassertfalse;
        }
        
        currentPreset.setValue(presetFile.getFullPathName());
    }

	void PresetManager::deletePreset(const String& presetPath)
	{
		if (presetPath.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetPath + "." + extension);
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
        setCurrentPresetId();
	}

	void PresetManager::loadPreset(const String& presetPath)
	{
		if (presetPath.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetPath + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		XmlDocument xmlDocument{ presetFile }; 
		const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());

		valueTreeState.replaceState(valueTreeToLoad);
		currentPreset.setValue(presetPath);
        setCurrentPresetId();
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
        
        if (!it->second.deleteFile())
        {
            DBG("Prset file " + it->second.getFullPathName() + " could not be deleted");
            jassertfalse;
            return;
        }
            currentPreset.setValue("");
            setCurrentPresetId();
    }

    void PresetManager::loadPreset(const int& id)
    {
        if (id == 0) return;
        
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
        auto menu = buildSubMenuRecursive(defaultDirectory);
        setCurrentPresetId();
        return menu;
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
    
	void PresetManager::valueTreeRedirected(ValueTree& treeWhichHasBeenChanged)
	{
		currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetPathProperty, nullptr));
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
    
    void PresetManager::setCurrentPresetId()
    {
        if (currentPreset.toString() == "")
        {
            currentPresetId = -1;
            return;
        }
        
        for (auto p: avaiablePresets)
        {
            if (p.second.getFullPathName() == currentPreset.toString())
            {
                currentPresetId = p.first;
                break;
            }
        }
    }

}
