/*
  ==============================================================================

    RaffaLinearSlider.h
    Created: 31 Jan 2025 9:09:05pm
    Author:  jorda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class RaffaLinearSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RaffaLinearSliderLookAndFeel();
    
    void drawLinearSlider(juce::Graphics& g, 
        int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider&) override;
    
    int getSliderThumbRadius(juce::Slider& slider) override;
};

class RaffaLinearSlider : public juce::Slider
{
public:
    RaffaLinearSlider() = delete;

    RaffaLinearSlider
    (juce::RangedAudioParameter& rap, const juce::String& label, const juce::String& unitSuffix, const bool alignRight=false)
        : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        label(label),
        suffix(unitSuffix),
        alignRight(alignRight)
    {
        setLookAndFeel(&lnf);
        gridAlreadyVisible = false;
        setName("slider");
    }

    ~RaffaLinearSlider()
    {
        setLookAndFeel(nullptr);
    }

    bool hitTest(int x, int y) override;
    double proportionOfLengthToValue(double proportion) override;
    double valueToProportionOfLength(double value) override;

    //void paint(juce::Graphics& g) override;
    juce::String getTextFromValue(double value) override;
    //juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 12; };
    juce::String getLabel() const { return label; };
    juce::String getDisplayString();
    bool isAlignedRight() const { return alignRight; };

    inline static bool gridAlreadyVisible;

private:
    RaffaLinearSliderLookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String label, suffix;

    float getLinearSliderPos(double value) const;

    double lastCurrentValue = 0, lastValueMin = 0, lastValueMax = 0;
    juce::NormalisableRange<double> normRange{ 0.0, 10.0 };
    int sliderRegionStart = 0, sliderRegionSize = 1;

    bool alignRight{ false };
};