/*
  ==============================================================================

    CustomGainKnob.h
    Created: 29 Nov 2022 10:16:54pm
    Author:  jorda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct CustomGainLookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};

struct CustomGainSlider : juce::Slider
{
    CustomGainSlider() = delete;

    CustomGainSlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix, const float minusInfinityValue, bool showTextLabel = false) : 
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix),
        minusInfinityValue(minusInfinityValue),
        showTextLabel(showTextLabel)
    {
        setLookAndFeel(&lnf);
    }

    ~CustomGainSlider()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; };
    virtual juce::String getDisplayString();
    bool isTextLabelToDraw() const { return showTextLabel; };

private:
    CustomGainLookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
    const float minusInfinityValue;
    bool showTextLabel = false;
};