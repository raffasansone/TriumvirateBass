#pragma once

#include <JuceHeader.h>

namespace gui
{
	using namespace juce;
	class PresetPanel : public Component, Button::Listener, ComboBox::Listener
	{
	public:
		PresetPanel(service::PresetManager& pm) : presetManager(pm)
		{
			configureButton(saveButton, "Save");
			configureButton(deleteButton, "Delete");
			configureButton(previousPresetButton, "<");
			configureButton(nextPresetButton, ">");

			presetList.setTextWhenNothingSelected("Presets");
			presetList.setMouseCursor(MouseCursor::PointingHandCursor);
			comboBoxLookAndFeelBuilder(presetList);
			addAndMakeVisible(presetList);
			presetList.addListener(this);

			loadPresetList();
		}

		~PresetPanel()
		{
			saveButton.removeListener(this);
			deleteButton.removeListener(this);
			previousPresetButton.removeListener(this);
			nextPresetButton.removeListener(this);
			presetList.removeListener(this);
		}

		void resized() override
		{
			const auto container = getLocalBounds().reduced(4);
			auto bounds = container;

			saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(2,8));
			previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(2,8));
			presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(2,8));
			nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(2,8));
			deleteButton.setBounds(bounds.reduced(2,8));
		}
	private:
		inline static const juce::Colour PRESET_COLOUR{0xff222222};

		void buttonClicked(Button* button) override
		{
			if (button == &saveButton)
			{
				fileChooser = std::make_unique<FileChooser>(
					"Please enter the name of the preset to save",
					service::PresetManager::defaultDirectory,
					"*." + service::PresetManager::extension
				);
				fileChooser->launchAsync(FileBrowserComponent::saveMode, [&](const FileChooser& chooser)
					{
						const auto resultFile = chooser.getResult();
						presetManager.savePreset(resultFile.getFileNameWithoutExtension());
						loadPresetList();
						comboBoxChanged(&presetList);
					});
			}
			if (button == &previousPresetButton)
			{
				const auto index = presetManager.loadPreviousPreset();
				presetList.setSelectedItemIndex(index, dontSendNotification);
				comboBoxChanged(&presetList);
			}
			if (button == &nextPresetButton)
			{
				const auto index = presetManager.loadNextPreset();
				presetList.setSelectedItemIndex(index, dontSendNotification);
				comboBoxChanged(&presetList);
			}
			if (button == &deleteButton)
			{
				presetManager.deletePreset(presetManager.getCurrentPreset());
				loadPresetList();
			}
		}
		void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
		{
			if (comboBoxThatHasChanged == &presetList)
			{
				presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));

				if (presetManager.valueTreeState.state.getProperty("default")) {
					deleteButton.setEnabled(false);
				}
				else {
					deleteButton.setEnabled(true);
				}
			}
		}

		void configureButton(TextButton& button, const String& buttonText) 
		{
			button.setButtonText(buttonText);
			textButtonLookAndFeelBuilder(button);
			addAndMakeVisible(button);
			button.addListener(this);
		}

		void textButtonLookAndFeelBuilder(TextButton& button)
		{
			button.setMouseCursor(MouseCursor::PointingHandCursor);
			button.setColour(TextButton::ColourIds::buttonColourId, PRESET_COLOUR);
		}

		void comboBoxLookAndFeelBuilder(ComboBox& box)
		{
			box.setColour(ComboBox::ColourIds::backgroundColourId, PRESET_COLOUR);
			box.setColour(ComboBox::ColourIds::buttonColourId, PRESET_COLOUR);
		}

		void loadPresetList()
		{
			presetList.clear(dontSendNotification);
			const auto allPresets = presetManager.getAllPresets();
			const auto currentPreset = presetManager.getCurrentPreset();
			presetList.addItemList(allPresets, 1);
			presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
		}

		service::PresetManager& presetManager;
		TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
		ComboBox presetList;
		std::unique_ptr<FileChooser> fileChooser;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
	};
}
