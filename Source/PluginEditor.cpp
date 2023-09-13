/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
TriumvirateBassAudioProcessorEditor::TriumvirateBassAudioProcessorEditor (TriumvirateBassAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    versionLabel("version", BUILD_VERSION),
    inputLevelMeter(foleys::LevelMeter::Minimal),
    inputGainSlider(*audioProcessor.apvts.getParameter("inputGain"), "Input", "dB"),
    lowVolumeSlider(*audioProcessor.apvts.getParameter("lowVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    lowGainSlider(*audioProcessor.apvts.getParameter("lowPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    midVolumeSlider(*audioProcessor.apvts.getParameter("midVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    midGainSlider(*audioProcessor.apvts.getParameter("midPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    highVolumeSlider(*audioProcessor.apvts.getParameter("highVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    highGainSlider(*audioProcessor.apvts.getParameter("highPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    outputGainSlider(*audioProcessor.apvts.getParameter("outputGain"),"Output", "dB"),
    bypassButton("bypassButton"),
    inputGainSliderAttachment(audioProcessor.apvts,"inputGain", inputGainSlider),
    lowVolumeSliderAttachment(audioProcessor.apvts,"lowVolume",lowVolumeSlider),
    lowGainSliderAttachment(audioProcessor.apvts,"lowPreampGain",lowGainSlider),
    midVolumeSliderAttachment(audioProcessor.apvts,"midVolume",midVolumeSlider),
    midGainSliderAttachment(audioProcessor.apvts,"midPreampGain",midGainSlider),
    highVolumeSliderAttachment(audioProcessor.apvts,"highVolume",highVolumeSlider),
    highGainSliderAttachment(audioProcessor.apvts,"highPreampGain",highGainSlider),
    outputLevelMeter(foleys::LevelMeter::Minimal),
    outputGainSliderAttachment(audioProcessor.apvts,"outputGain",outputGainSlider),
    bypassButtonAttachment(*audioProcessor.apvts.getParameter("bypass"),bypassButton),
    ledOff(juce::Image()),
    ledOn(juce::Image()),
    presetPanel(p.getPresetManager())
{
    
    // Initialize foley Level Meter Look n Feel
    initialiseLevelMeters();

    // Initialize internal components
    for (auto* comp : getComponents())
    {
        addAndMakeVisible(comp);
    }

    initialiseBypassButton();

    setSize (800, 520);
    setResizable(false, false);
}

TriumvirateBassAudioProcessorEditor::~TriumvirateBassAudioProcessorEditor()
{
    // foley Meter
    inputLevelMeter.setLookAndFeel(nullptr);
    outputLevelMeter.setLookAndFeel(nullptr);
}

//==============================================================================
void TriumvirateBassAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::triumvirate_new_png, BinaryData::triumvirate_new_pngSize);
    g.drawImageAt(background, 0, 0);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::white);
}

void TriumvirateBassAudioProcessorEditor::initialiseLevelMeters() 
{
    levelMeterLnF.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::white);
    inputLevelMeter.setLookAndFeel(&levelMeterLnF);
    outputLevelMeter.setLookAndFeel(&levelMeterLnF);
    inputLevelMeter.setMeterSource(&audioProcessor.getInputLevelMeterSource());
    outputLevelMeter.setMeterSource(&audioProcessor.getOutputLevelMeterSource());
}

void TriumvirateBassAudioProcessorEditor::initialiseBypassButton() 
{
    ledOff = juce::ImageCache::getFromMemory(BinaryData::led_off_new_png, BinaryData::led_off_new_pngSize);
    ledOn = juce::ImageCache::getFromMemory(BinaryData::led_on_new_png, BinaryData::led_on_new_pngSize);
    
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]() {paintBypassButton(); };

    paintBypassButton();
}

void TriumvirateBassAudioProcessorEditor::paintBypassButton() 
{
    auto bounds = bypassButton.getBounds();

    if (bypassButton.getToggleState()) {
        bypassButton.setImages(true, false, true, ledOff, 1.f, juce::Colours::transparentWhite, ledOff, 1.f, juce::Colours::transparentWhite, ledOff, 1.f, juce::Colours::transparentWhite);
    }
    else {
        bypassButton.setImages(true, false, true, ledOn, 1.f, juce::Colours::transparentWhite, ledOn, 1.f, juce::Colours::transparentWhite, ledOn, 1.f, juce::Colours::transparentWhite);
    }

    bypassButton.setBounds(bounds);
}

void TriumvirateBassAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto presetArea = bounds.removeFromTop(proportionOfHeight(0.169f));
    auto presetPanelArea = presetArea.removeFromTop(45).removeFromRight(400);
    presetPanelArea.setPosition(presetPanelArea.getX(), presetPanelArea.getY() + 6);

    auto inputArea = bounds.removeFromLeft(120);

    auto versionArea = inputArea.removeFromTop(14);
    versionArea.setPosition(versionArea.getX() + 10, versionArea.getY() - 36);

    inputArea = inputArea.removeFromBottom(300);
    auto inputMeterArea = inputArea.removeFromLeft(22);
    inputMeterArea = inputMeterArea.removeFromTop(120);

    inputArea.setPosition(inputArea.getX() + 4, inputArea.getY());
    inputMeterArea.setPosition(inputMeterArea.getX() + 12, inputMeterArea.getCentreY() - 64);

    auto outputArea = bounds.removeFromRight(120);
    outputArea = outputArea.removeFromBottom(300);
    auto outputMeterArea = outputArea.removeFromRight(22);
    outputMeterArea = outputMeterArea.removeFromTop(120);

    outputArea.setPosition(outputArea.getX() - 4, outputArea.getY());
    outputMeterArea.setPosition(outputMeterArea.getX() - 12, outputMeterArea.getCentreY() - 64);

    auto bottomArea = bounds.removeFromBottom(proportionOfHeight(0.225f));

    auto lowArea = bounds.removeFromLeft(proportionOfWidth(0.231f));
    auto highArea = bounds.removeFromLeft(proportionOfWidth(0.231f));
    auto midArea = bounds;

    auto lowVolumeArea = lowArea.removeFromTop(177);
    lowVolumeArea.setPosition(114, 90);

    auto lowGainArea = lowArea.removeFromBottom(130);
    lowGainArea.setPosition(121, 278);

    auto midVolumeArea = midArea.removeFromTop(178);
    midVolumeArea.setPosition(500, 91);

    auto midGainArea = midArea.removeFromBottom(126);
    midGainArea.setPosition(494, 280);

    auto highVolumeArea = highArea.removeFromBottom(179);
    highVolumeArea.setPosition(305, 215);

    auto highGainArea = highArea.removeFromTop(126);
    highGainArea.setPosition(301, 50);

    auto bypassButtonArea = bottomArea.removeFromTop(50);
    bypassButtonArea.setPosition(121, 369);

    presetPanel.setBounds(presetPanelArea);

    versionLabel.setBounds(versionArea);

    inputLevelMeter.setBounds(inputMeterArea);
    inputGainSlider.setBounds(inputArea);

    lowVolumeSlider.setBounds(lowVolumeArea);
    lowGainSlider.setBounds(lowGainArea);

    highVolumeSlider.setBounds(highVolumeArea);
    highGainSlider.setBounds(highGainArea);

    midVolumeSlider.setBounds(midVolumeArea);
    midGainSlider.setBounds(midGainArea);

    outputLevelMeter.setBounds(outputMeterArea);
    outputGainSlider.setBounds(outputArea);

    bypassButton.setBounds(bypassButtonArea);
}

std::vector<juce::Component*> TriumvirateBassAudioProcessorEditor::getComponents()
{
    return {
        &versionLabel,
        &presetPanel,
        &inputLevelMeter,
        &inputGainSlider,
        &lowVolumeSlider, 
        &midVolumeSlider, 
        &highVolumeSlider, 
        &lowGainSlider, 
        &midGainSlider, 
        &highGainSlider,
        &outputLevelMeter,
        &outputGainSlider,
        &bypassButton
    };
}
