/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GraphicElements/CustomGainKnob.h"
#include "GraphicElements/InOutGainKnob.h"
#include "PresetGui/PresetPanel.h"

//==============================================================================
struct BypassButton : juce::ImageButton { 
    BypassButton(const juce::String& name = juce::String());
    bool hitTest(int x, int y) override; 
};

struct CustomLevelMeterLnF : foleys::LevelMeterLookAndFeel {
    CustomLevelMeterLnF();
    void setupDefaultMeterColours() override;
    void setupDefaultStereoFieldColours() override;
};

struct TriumviratePreferencesPanel : juce::PreferencesPanel {
    TriumviratePreferencesPanel(TriumvirateBassAudioProcessor&);
    ~TriumviratePreferencesPanel();

    virtual juce::Component* createComponentForPage(const juce::String& pageName) override;
    void paint(juce::Graphics&) override;
    std::vector<juce::Component*> getPreferencesComponents();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TriumvirateBassAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState::SliderAttachment* sliderAttachment;
};

/**
*/
class TriumvirateBassAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TriumvirateBassAudioProcessorEditor (TriumvirateBassAudioProcessor&);
    ~TriumvirateBassAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void initialiseLevelMeters();
    void initialiseButtons();
    void paintBypassButton();
    void paintPreferencesButton();
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TriumvirateBassAudioProcessor& audioProcessor;

    gui::PresetPanel presetPanel;

    juce::Label versionLabel;

    TriumviratePreferencesPanel preferencesPanel;

    CustomLevelMeterLnF levelMeterLnF;
    foleys::LevelMeter inputLevelMeter, outputLevelMeter;

    InOutGainSlider
        inputGainSlider,
        outputGainSlider,
        dryWetSlider;

    CustomGainSlider
        lowVolumeSlider,
        midVolumeSlider,
        highVolumeSlider,
        lowGainSlider,
        midGainSlider,
        highGainSlider;

    BypassButton bypassButton;
    juce::Image ledOff, ledOn;

    juce::ImageButton preferencesButton;
    juce::Image settingsImage;

    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    SliderAttachment
        inputGainSliderAttachment,
        lowVolumeSliderAttachment,
        midVolumeSliderAttachment,
        highVolumeSliderAttachment,
        lowGainSliderAttachment,
        midGainSliderAttachment,
        highGainSliderAttachment,
        outputGainSliderAttachment,
        dryWetSliderAttachment;

    juce::ButtonParameterAttachment
        bypassButtonAttachment;

    juce::TooltipWindow tooltipWindow;

    std::vector<juce::Component*> getDefaultComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriumvirateBassAudioProcessorEditor)
};
