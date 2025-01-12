/*
  ==============================================================================

    InOutGainKnob.cpp
    Created: 30 Nov 2022 5:01:11pm
    Author:  jorda

  ==============================================================================
*/

#include "InOutGainKnob.h"

void InOutGainLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));

    //Knob
    g.setColour(Colour(0.0f,0.0f,0.0f,0.0f));
    g.fillEllipse(bounds);

    //Knob border
    g.setGradientFill(ColourGradient(
        Colours::dimgrey,
        4.f,
        0.f,
        Colours::white,
        bounds.getCentre().getX(),
        bounds.getCentre().getY(),
        false));
    g.drawEllipse(bounds, 2.f);

    if (auto* ioslider = dynamic_cast<InOutGainSlider*>(&slider))
    {
        //Knob pointer
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float>r;
        r.setLeft(center.getX() - 4);
        r.setRight(center.getX() + 4);
        r.setTop(bounds.getY()+4);
        r.setBottom(bounds.getY()+12);

        p.addRoundedRectangle(r, 4.f);

        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.setColour(Colour(255u, 255u, 255u));
        g.fillPath(p);

        g.setFont(static_cast<float>(ioslider->getTextHeight()));
        juce::String text = ioslider->getDisplayString() + juce::newLine + ioslider->getLabel();
        auto strWidth = g.getCurrentFont().getStringWidth(text);

        r.setSize(static_cast<float>(strWidth), static_cast<float>(ioslider->getTextHeight()));
        r.setCentre(bounds.getCentre());
        r.setY(r.getCentreY() + 46.f);

        g.setColour(Colours::white);       
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void InOutGainSlider::paint(juce::Graphics& g)
{
    using namespace juce;
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto sliderBounds = getSliderBounds();

    // Knobs bounding boxes for debugging
    //g.setColour(Colours::red);
    //g.drawRect(getLocalBounds());
    //g.setColour(Colours::yellow);
    //g.drawRect(sliderBounds);

    getLookAndFeel().drawRotarySlider(
        g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        param->convertTo0to1(static_cast<float>(getValue())),
        startAng,
        endAng,
        *this);
}

juce::String InOutGainSlider::getTextFromValue(double value)
{
    return this->getDisplayString() + " " + this->getLabel();
}

juce::Rectangle<int> InOutGainSlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size *= 0.9f;

    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(20);

    return r;
}

juce::String InOutGainSlider::getDisplayString()
{

    if (suffix.equalsIgnoreCase("dB"))
    {
        return juce::String(getValue(), 2) + juce::String(" dB");
    } 
    else if (suffix.equalsIgnoreCase("%"))
    {
        return  juce::String( getValue() * 100 ) + juce::String ("%");
    }
    else
    {
        return juce::String(getValue(), 1);
    }
}
