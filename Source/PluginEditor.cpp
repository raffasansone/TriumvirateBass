/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
BypassButton::BypassButton(const juce::String& name) : juce::ImageButton(name)
{
}

bool BypassButton::hitTest(int x, int y) 
{
    // Sets the hitbox to a centered square the size of the component's height
    int widthPadding = (getLocalBounds().getWidth() - getLocalBounds().getHeight() ) / 2;
    return x > widthPadding && x < (widthPadding + getLocalBounds().getHeight());
}

TriumviratePreferencesPanel::TriumviratePreferencesPanel(TriumvirateBassAudioProcessor& p) : audioProcessor(p)
{
    sliderAttachment = nullptr;
}

TriumviratePreferencesPanel::~TriumviratePreferencesPanel()
{
    delete sliderAttachment;
}

juce::Component* TriumviratePreferencesPanel::createComponentForPage(const juce::String& pageName)
{
    if (pageName.equalsIgnoreCase("Cabinet"))
    {
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

        // TODO Remove this logic. It's wrong because the slider attachment will delete the attachment
        // to the actual input gain on the GUI when exited. This is just a reminder to add the new parameters here
        juce::Slider* inputGainSlider = new InOutGainSlider(*audioProcessor.apvts.getParameter("inputGain"), "Input", "dB");
        sliderAttachment = new SliderAttachment(audioProcessor.apvts, "inputGain", *inputGainSlider);
        
        return inputGainSlider;
    }

    if (pageName.equalsIgnoreCase("Circuit"))
        return new juce::Label(pageName, "This is page 2 of my settings.");
}

void TriumviratePreferencesPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(juce::uint8(0x12), juce::uint8(0x12), juce::uint8(0x12), juce::uint8(0xEE)));
}

std::vector<juce::Component*> TriumviratePreferencesPanel::getPreferencesComponents()
{
    return std::vector<juce::Component*>();
}

CustomLevelMeterLnF::CustomLevelMeterLnF()
{
    CustomLevelMeterLnF::setupDefaultMeterColours();
    CustomLevelMeterLnF::setupDefaultStereoFieldColours();
}

void CustomLevelMeterLnF::setupDefaultMeterColours()
{
    setColour(foleys::LevelMeter::lmTextColour, juce::Colours::lightgrey);
    setColour(foleys::LevelMeter::lmTextClipColour, juce::Colours::white);
    setColour(foleys::LevelMeter::lmTextDeactiveColour, juce::Colours::darkgrey);
    setColour(foleys::LevelMeter::lmTicksColour, juce::Colours::orange);
    setColour(foleys::LevelMeter::lmOutlineColour, juce::Colours::orange);
    setColour(foleys::LevelMeter::lmBackgroundColour, juce::Colour(0x00000000));
    setColour(foleys::LevelMeter::lmBackgroundClipColour, juce::Colours::red);
    setColour(foleys::LevelMeter::lmMeterForegroundColour, juce::Colours::green);
    setColour(foleys::LevelMeter::lmMeterOutlineColour, juce::Colours::lightgrey);
    setColour(foleys::LevelMeter::lmMeterBackgroundColour, juce::Colours::darkgrey);
    setColour(foleys::LevelMeter::lmMeterMaxNormalColour, juce::Colours::lightgrey);
    setColour(foleys::LevelMeter::lmMeterMaxWarnColour, juce::Colours::orange);
    setColour(foleys::LevelMeter::lmMeterMaxOverColour, juce::Colours::darkred);
    setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    setColour(foleys::LevelMeter::lmMeterGradientMidColour, juce::Colours::lightgoldenrodyellow);
    setColour(foleys::LevelMeter::lmMeterGradientMaxColour, juce::Colours::red);
    setColour(foleys::LevelMeter::lmMeterReductionColour, juce::Colours::orange);
}

void CustomLevelMeterLnF::setupDefaultStereoFieldColours()
{
    setColour(foleys::StereoFieldComponent::backgroundColour, juce::Colour(0x00000000));
    setColour(foleys::StereoFieldComponent::borderColour, juce::Colours::silver);
    setColour(foleys::StereoFieldComponent::outlineColour, juce::Colours::silver);
    setColour(foleys::StereoFieldComponent::gonioColour, juce::Colours::silver);
    setColour(foleys::StereoFieldComponent::currentValuesColour, juce::Colours::silver);
    setColour(foleys::StereoFieldComponent::maxValuesColour, juce::Colours::darkgrey);
};

//==============================================================================
TriumvirateBassAudioProcessorEditor::TriumvirateBassAudioProcessorEditor(TriumvirateBassAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    versionLabel("version", BUILD_VERSION),
    inputLevelMeter(foleys::LevelMeter::Minimal),
    inputGainSlider(*audioProcessor.apvts.getParameter("inputGain"), "Input", "dB"),
    lowVolumeSlider(*audioProcessor.apvts.getParameter("lowVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    lowGainSlider(*audioProcessor.apvts.getParameter("lowPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    midVolumeSlider(*audioProcessor.apvts.getParameter("midVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    midGainSlider(*audioProcessor.apvts.getParameter("midPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    highVolumeSlider(*audioProcessor.apvts.getParameter("highVolume"), "dB", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    highGainSlider(*audioProcessor.apvts.getParameter("highPreampGain"), "", TriumvirateBassAudioProcessor::MINUS_INFINITY_DB),
    outputGainSlider(*audioProcessor.apvts.getParameter("outputGain"), "Output", "dB"),
    dryWetSlider(*audioProcessor.apvts.getParameter("dryWet"), "Dry/Wet", "%"),
    bypassButton("bypassButton"),
    preferencesButton("preferencesButton"),
    preferencesPanel(p),
    inputGainSliderAttachment(audioProcessor.apvts,"inputGain", inputGainSlider),
    lowVolumeSliderAttachment(audioProcessor.apvts,"lowVolume",lowVolumeSlider),
    lowGainSliderAttachment(audioProcessor.apvts,"lowPreampGain",lowGainSlider),
    midVolumeSliderAttachment(audioProcessor.apvts,"midVolume",midVolumeSlider),
    midGainSliderAttachment(audioProcessor.apvts,"midPreampGain",midGainSlider),
    highVolumeSliderAttachment(audioProcessor.apvts,"highVolume",highVolumeSlider),
    highGainSliderAttachment(audioProcessor.apvts,"highPreampGain",highGainSlider),
    outputLevelMeter(foleys::LevelMeter::Minimal),
    outputGainSliderAttachment(audioProcessor.apvts,"outputGain",outputGainSlider),
    dryWetSliderAttachment(audioProcessor.apvts,"dryWet", dryWetSlider),
    bypassButtonAttachment(*audioProcessor.apvts.getParameter("bypass"),bypassButton),
    ledOff(juce::ImageCache::getFromMemory(BinaryData::led_off_new_png, BinaryData::led_off_new_pngSize)),
    ledOn(juce::ImageCache::getFromMemory(BinaryData::led_on_new_png, BinaryData::led_on_new_pngSize)),
    settingsImage(juce::ImageCache::getFromMemory(BinaryData::Settings_icon_EFEFEF_40x40_png, BinaryData::Settings_icon_EFEFEF_40x40_pngSize)),
    presetPanel(p.getPresetManager()),
    tooltipWindow(this)
{
    
    // Initialize foley Level Meter Look n Feel
    initialiseLevelMeters();
    

    // Initialize internal components
    for (auto* comp : getDefaultComponents())
    {
        addAndMakeVisible(comp);
    }
    preferencesPanel.setVisible(false);

    dryWetSlider.setPopupDisplayEnabled(true, true, this);

    initialiseButtons();
    
    // PREFERENCES
    preferencesPanel.addSettingsPage("Cabinet", BinaryData::Speaker_EFEFEF_40x40_png, BinaryData::Speaker_EFEFEF_40x40_pngSize);
    preferencesPanel.addSettingsPage("Circuit", BinaryData::Crossover_EFEFEF_40x40_png, BinaryData::Crossover_EFEFEF_40x40_pngSize);

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

void TriumvirateBassAudioProcessorEditor::initialiseButtons() 
{
    //ledOff = juce::ImageCache::getFromMemory(BinaryData::led_off_new_png, BinaryData::led_off_new_pngSize);
    //ledOn = juce::ImageCache::getFromMemory(BinaryData::led_on_new_png, BinaryData::led_on_new_pngSize);
    
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]() {
        paintBypassButton(); 
    };

    paintBypassButton();  

    preferencesButton.setClickingTogglesState(true);
    preferencesButton.setTooltip("Preferences");
    preferencesButton.onClick = [this]() {
        paintPreferencesButton(); 
        if (preferencesPanel.isVisible()) {
            preferencesPanel.setVisible(false);
            preferencesButton.setTooltip("Preferences");
        }
        else {
            preferencesPanel.setVisible(true);
            preferencesButton.setTooltip("Close Preferences");
        }
    };

    paintPreferencesButton();
}

void TriumvirateBassAudioProcessorEditor::paintBypassButton() 
{
    auto bounds = bypassButton.getBounds();

    if (bypassButton.getToggleState()) {
        bypassButton.setImages(true, false, true, 
            ledOff, 1.f, juce::Colours::transparentWhite,  // Normal
            ledOff, 1.f, juce::Colours::transparentWhite,  // Hover
            ledOff, 1.f, juce::Colours::transparentWhite); // Selected
    }
    else {
        bypassButton.setImages(true, false, true, 
            ledOn, 1.f, juce::Colours::transparentWhite,  // Normal
            ledOn, 1.f, juce::Colours::transparentWhite,  // Hover
            ledOn, 1.f, juce::Colours::transparentWhite); // Selected
    }

    bypassButton.setBounds(bounds);
}

void TriumvirateBassAudioProcessorEditor::paintPreferencesButton() 
{
    auto bounds = preferencesButton.getBounds();
    preferencesButton.setImages(true, false, true, 
        settingsImage, 1.f, juce::Colours::transparentWhite, // Normal
        settingsImage, .8f, juce::Colours::transparentWhite, // Hover
        settingsImage, 1.f, juce::Colours::darkslategrey);   // Selected
    preferencesButton.setBounds(bounds);
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
    midVolumeArea.setPosition(499, 91);

    auto midGainArea = midArea.removeFromBottom(126);
    midGainArea.setPosition(493, 279);

    auto highVolumeArea = highArea.removeFromBottom(179);
    highVolumeArea.setPosition(306, 215);

    auto highGainArea = highArea.removeFromTop(126);
    highGainArea.setPosition(304, 49);

    auto bypassButtonArea = bottomArea.removeFromTop(50);
    bypassButtonArea.setPosition(121, 369);

    auto preferencesArea = bottomArea.removeFromRight(80);
    preferencesArea.setPosition(670, 455);
    auto dryWetArea = bottomArea.removeFromRight(80);
    dryWetArea.setPosition(730, 450);

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

    preferencesPanel.setBounds(getLocalBounds().reduced(100, 80));
    preferencesButton.setBounds(preferencesArea);
    dryWetSlider.setBounds(dryWetArea);

}

std::vector<juce::Component*> TriumvirateBassAudioProcessorEditor::getDefaultComponents()
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
        &bypassButton,
        &preferencesPanel,
        &preferencesButton,
        &dryWetSlider
    };
}
