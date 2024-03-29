#pragma once

#include <JuceHeader.h>

namespace service
{
	using namespace juce;
	class PresetManager : ValueTree::Listener
	{
	public:
		static const File defaultDirectory;
		static const String extension;
		static const String presetNameProperty;

		PresetManager(AudioProcessorValueTreeState&);

		void savePreset(const String& presetName);
		void saveDefaultPreset(const String& presetName);
		void deletePreset(const String& presetName);
		void loadPreset(const String& presetName);
		int loadNextPreset();
		int loadPreviousPreset();
		StringArray getAllPresets() const;
		String getCurrentPreset() const;
		AudioProcessorValueTreeState& valueTreeState;
	private:
		void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;
		Value currentPreset;
	};
}