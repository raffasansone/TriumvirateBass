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
//#include "GraphicElements/BypassButton.h"

//==============================================================================
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
    void initialiseBypassButton();
    void paintBypassButton();
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TriumvirateBassAudioProcessor& audioProcessor;

    gui::PresetPanel presetPanel;

    juce::Label versionLabel;

    foleys::LevelMeterLookAndFeel levelMeterLnF;
    foleys::LevelMeter inputLevelMeter, outputLevelMeter;

    InOutGainSlider
        inputGainSlider,
        outputGainSlider;

    CustomGainSlider
        lowVolumeSlider,
        midVolumeSlider,
        highVolumeSlider,
        lowGainSlider,
        midGainSlider,
        highGainSlider;

    juce::ImageButton bypassButton;
    juce::Image ledOff, ledOn;

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
        outputGainSliderAttachment;

    juce::ButtonParameterAttachment bypassButtonAttachment;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriumvirateBassAudioProcessorEditor)
};
