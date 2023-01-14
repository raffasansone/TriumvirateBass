/*
  ==============================================================================

    CustomGainKnob.h
    Created: 29 Nov 2022 10:16:54pm
    Author:  jorda

  ==============================================================================
*/

#pragma once

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
    CustomGainSlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix, bool showTextLabel = false) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix),
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
    bool showTextLabel = false;
};