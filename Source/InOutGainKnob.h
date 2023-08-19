/*
  ==============================================================================

    InOutGainKnob.h
    Created: 30 Nov 2022 4:02:28pm
    Author:  jorda

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

struct InOutGainLookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;
};

struct InOutGainSlider : juce::Slider
{
    InOutGainSlider() = delete;

    InOutGainSlider(juce::RangedAudioParameter& rap, const juce::String& label, const juce::String& unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        label(label),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~InOutGainSlider()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 12; };
    juce::String getLabel() const {return label;};
    juce::String getDisplayString();

private:
    InOutGainLookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String label, suffix;
};