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
    lowVolumeSlider(*audioProcessor.apvts.getParameter("lowVolume"), "dB"),
    lowGainSlider(*audioProcessor.apvts.getParameter("lowPreampGain"), ""),
    midVolumeSlider(*audioProcessor.apvts.getParameter("midVolume"), "dB"),
    midGainSlider(*audioProcessor.apvts.getParameter("midPreampGain"), ""),
    highVolumeSlider(*audioProcessor.apvts.getParameter("highVolume"), "dB"),
    highGainSlider(*audioProcessor.apvts.getParameter("highPreampGain"), ""),
    outputGainSlider(*audioProcessor.apvts.getParameter("outputGain"),"Output", "dB"),
    inputGainSliderAttachment(audioProcessor.apvts,"inputGain", inputGainSlider),
    lowVolumeSliderAttachment(audioProcessor.apvts,"lowVolume",lowVolumeSlider),
    lowGainSliderAttachment(audioProcessor.apvts,"lowPreampGain",lowGainSlider),
    midVolumeSliderAttachment(audioProcessor.apvts,"midVolume",midVolumeSlider),
    midGainSliderAttachment(audioProcessor.apvts,"midPreampGain",midGainSlider),
    highVolumeSliderAttachment(audioProcessor.apvts,"highVolume",highVolumeSlider),
    highGainSliderAttachment(audioProcessor.apvts,"highPreampGain",highGainSlider),
    outputLevelMeter(foleys::LevelMeter::Minimal),
    outputGainSliderAttachment(audioProcessor.apvts,"outputGain",outputGainSlider)
{
    
    // Initialize foley Level Meter Look n Feel
    levelMeterLnF.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::white);
    inputLevelMeter.setLookAndFeel(&levelMeterLnF);
    outputLevelMeter.setLookAndFeel(&levelMeterLnF);
    inputLevelMeter.setMeterSource(&p.getInputLevelMeterSource());
    outputLevelMeter.setMeterSource(&p.getOutputLevelMeterSource());

    // Initialize internal components
    for (auto* comp : getComponents())
    {
        addAndMakeVisible(comp);
    }

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
    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::triumvirate_on_png, BinaryData::triumvirate_on_pngSize);
    g.drawImageAt(background, 0, 0);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::white);
}

void TriumvirateBassAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto presetArea = bounds.removeFromTop(bounds.getHeight() * 0.169);

    auto inputArea = bounds.removeFromLeft(120);

    auto versionArea = inputArea.removeFromTop(14);
    versionArea.setPosition(versionArea.getX() + 10, versionArea.getY() - 36);

    inputArea = inputArea.removeFromBottom(300);
    auto inputMeterArea = inputArea.removeFromLeft(22);
    inputMeterArea = inputMeterArea.removeFromTop(120);

    inputArea.setPosition(inputArea.getX() + 4, inputArea.getY());
    inputMeterArea.setPosition(inputMeterArea.getX()+12,inputMeterArea.getCentreY()-64);

    auto outputArea = bounds.removeFromRight(120);
    outputArea = outputArea.removeFromBottom(300);
    auto outputMeterArea = outputArea.removeFromRight(22);
    outputMeterArea = outputMeterArea.removeFromTop(120);

    outputArea.setPosition(outputArea.getX() - 4, outputArea.getY());
    outputMeterArea.setPosition(outputMeterArea.getX() - 12, outputMeterArea.getCentreY() - 64);

    auto bottomArea = bounds.removeFromBottom(bounds.getHeight() * 0.271);

    auto lowArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto midArea = bounds;

    auto lowVolumeArea = lowArea.removeFromTop(lowArea.getHeight() * 0.60);
    auto lowGainArea = lowArea;

    auto midVolumeArea = midArea.removeFromTop(midArea.getHeight() * 0.60);
    auto midGainArea = midArea;

    auto highVolumeArea = highArea.removeFromBottom(highArea.getHeight() * 0.60); 
    auto highGainArea = highArea;
    highGainArea.setPosition(highGainArea.getX(), highGainArea.getY() - 30);

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
}

std::vector<juce::Component*> TriumvirateBassAudioProcessorEditor::getComponents()
{
    return {
        &versionLabel,
        &inputLevelMeter,
        &inputGainSlider,
        &lowVolumeSlider, 
        &midVolumeSlider, 
        &highVolumeSlider, 
        &lowGainSlider, 
        &midGainSlider, 
        &highGainSlider,
        &outputLevelMeter,
        &outputGainSlider
    };
}
