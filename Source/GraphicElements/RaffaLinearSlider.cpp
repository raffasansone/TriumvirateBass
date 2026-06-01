/*
  ==============================================================================

    RaffaLinearSlider.cpp
    Created: 31 Jan 2025 9:09:24pm
    Author:  jorda

  ==============================================================================
*/

#include "RaffaLinearSlider.h"

RaffaLinearSliderLookAndFeel::RaffaLinearSliderLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colour(juce::uint8(240),240,240,0.5f));
    setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);//juce::Colour(juce::uint8(32), 32, 32, 0.5f));
}

int RaffaLinearSliderLookAndFeel::getSliderThumbRadius(juce::Slider& slider)
{
    return juce::jmin(8, slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.5f)
        : static_cast<int> ((float)slider.getWidth() * 0.5f));
}

void RaffaLinearSliderLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float /*minSliderPos*/,
    float /*maxSliderPos*/,
    const juce::Slider::SliderStyle style, juce::Slider& slider) {


    if (auto* ioslider = dynamic_cast<RaffaLinearSlider*>(&slider)) {
        
        auto range = ioslider->getRange();
         
        float start = static_cast<float>(range.getStart());
        float end = static_cast<float>(range.getEnd());

        jassert(start > 20.f);
        jassert(end < 20000.f);

        float percentStart = juce::mapFromLog10(start, 20.f, 20000.f);
        float percentEnd = juce::mapFromLog10(end, 20.f, 20000.f);
        float percentValue = juce::mapFromLog10((float)ioslider->getValue(), 20.f, 20000.f);

        auto width1 = slider.getLocalBounds().getWidth();
        float newWidth = width1 * (percentEnd - percentStart);
        float newX = width1 * percentStart;
        float newSliderPos = width1 * percentValue;

        juce::LookAndFeel_V4::drawLinearSlider(g, static_cast<int>(newX), y, static_cast<int>(newWidth), height, newSliderPos, 0, 0, style, slider);
    }
}

bool RaffaLinearSlider::hitTest(int x, int y)
{
    // Sets the hitbox to just the middle of the slider
    juce::Point<int> centre = getLocalBounds().getCentre();

    auto range = getRange();

    float start = static_cast<float>(range.getStart());
    float end = static_cast<float>(range.getEnd());

    jassert(start > 20.f);
    jassert(end < 20000.f);

    int margin = 20;
    float percentStart = juce::mapFromLog10(start, 20.f, 20000.f);
    float newXStart = getLocalBounds().getWidth() * percentStart;

    float percentEnd = juce::mapFromLog10(end, 20.f, 20000.f);
    float newXEnd = getLocalBounds().getWidth() * percentEnd;

    return y > centre.getY()-10 && y < centre.getY()+10 && x > (newXStart - margin) && x < (newXEnd + margin);
}

double RaffaLinearSlider::valueToProportionOfLength(double value)
{
    return (std::log(value / 20.0)/ std::log(20000.0 / 20.0));
}

double RaffaLinearSlider::proportionOfLengthToValue(double proportion)
{
    return 20.0 * std::pow(20000.0 / 20.0, proportion);
}

juce::String RaffaLinearSlider::getTextFromValue(double /*value*/)
{
    return this->getDisplayString() + " " + this->getLabel();
}

juce::String RaffaLinearSlider::getDisplayString()
{

    if (suffix.equalsIgnoreCase("dB"))
    {
        return juce::String(getValue(), 2) + juce::String(" dB");
    }
    else if (suffix.equalsIgnoreCase("%"))
    {
        return  juce::String(getValue() * 100) + juce::String("%");
    }
    else
    {
        return juce::String(getValue(), 1);
    }
}