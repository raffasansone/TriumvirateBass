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
    setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
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
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider) {


    if (auto* ioslider = dynamic_cast<RaffaLinearSlider*>(&slider)) {
        
        auto range = ioslider->getRange();
         
        float start = range.getStart();
        float end = range.getEnd();

        jassert(start > 20.f);
        jassert(end < 20000.f);

        float percentStart = juce::mapFromLog10(start, 20.f, 20000.f);
        float percentEnd = juce::mapFromLog10(end, 20.f, 20000.f);
        float percentValue = juce::mapFromLog10((float)ioslider->getValue(), 20.f, 20000.f);

        float padding = 50.f;
        auto width1 = slider.getLocalBounds().getWidth();
        float newWidth = width1 * (percentEnd - percentStart);
        float newX = width1 * percentStart + padding;
        float newSliderPos = width1 * percentValue + padding;

        juce::LookAndFeel_V4::drawLinearSlider(g, newX, y, newWidth, height, newSliderPos, 0, 0, style, slider);

        auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width1), static_cast<float>(height));
        auto center = bounds.getCentre();
        
        juce::Rectangle<float> r;
        r.setBounds(slider.getLocalBounds().getX(), slider.getLocalBounds().getY(), slider.getLocalBounds().getWidth(), slider.getLocalBounds().getHeight());

        g.setFont(static_cast<float>(ioslider->getTextHeight()));
        juce::String text = ioslider->getLabel() + ": " + ioslider->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);

        r.setSize(static_cast<float>(strWidth), static_cast<float>(ioslider->getTextHeight()));
        r.setCentre(bounds.getCentre());
        r.setY(r.getCentreY()-6);
        if (ioslider->isAlignedRight()) {
            r.setX(r.getRight()+36);
        }
        else {
            r.setX(0);
        }

        //g.setColour(juce::Colours::green);
        //g.drawRect(r);
        
        g.setColour(juce::Colours::white);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

bool RaffaLinearSlider::hitTest(int x, int y)
{
    // Sets the hitbox to just the middle of the slider
    juce::Point<int> centre = getLocalBounds().getCentre();

    auto range = getRange();

    float start = range.getStart();
    float end = range.getEnd();

    jassert(start > 20.f);
    jassert(end < 20000.f);

    float percentStart = juce::mapFromLog10(start, 20.f, 20000.f);
    float percentEnd = juce::mapFromLog10(end, 20.f, 20000.f);
    
    float padding = 40.f;
    //float newWidth = getLocalBounds().getWidth() * (percentEnd - percentStart);
    float newX = getLocalBounds().getWidth() * percentStart + padding;

    return y > centre.getY()-10 && y < centre.getY()+10 && x > newX && x < getLocalBounds().getWidth();
}

juce::String RaffaLinearSlider::getTextFromValue(double value)
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