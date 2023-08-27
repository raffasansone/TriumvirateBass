/*
  ==============================================================================

    CustomGainKnob.cpp
    Created: 29 Nov 2022 10:17:13pm
    Author:  jorda

  ==============================================================================
*/

#include "CustomGainKnob.h"

void CustomGainLookAndFeel::drawRotarySlider(juce::Graphics& g,
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

    auto bounds = Rectangle<float>(x, y, width, height);

    //Knob
    g.setGradientFill(ColourGradient(
        Colour(15u, 15u, 15u),
        10.f,
        0.f,
        Colours::black,
        bounds.getBottomRight().getX() - 10.f,
        bounds.getBottomRight().getY(),
        false));
    g.fillEllipse(bounds);

    //Knob border
    g.setGradientFill(ColourGradient(
        Colours::dimgrey,
        10.f,
        0.f,
        Colours::black,
        bounds.getCentre().getX(),
        bounds.getCentre().getY(),
        false));
    g.drawEllipse(bounds, 2.f);

    if (auto* cgslider = dynamic_cast<CustomGainSlider*>(&slider))
    {
        //Knob pointer
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float>r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - center.getY() * 0.4f);

        p.addRoundedRectangle(r, 2.f);

        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        g.setColour(Colour(255u, 255u, 255u));
        g.fillPath(p);

        if (cgslider->isTextLabelToDraw()) {
            g.setFont(cgslider->getTextHeight());
            auto text = cgslider->getDisplayString();
            auto strWidth = g.getCurrentFont().getStringWidth(text);

            r.setSize(strWidth + 4, cgslider->getTextHeight() + 2);
            r.setCentre(bounds.getCentre());

            g.setColour(Colours::black);
            g.fillRect(r);

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

void CustomGainSlider::paint(juce::Graphics& g)
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
        param->convertTo0to1(getValue()),
        startAng,
        endAng,
        *this);
}

juce::Rectangle<int> CustomGainSlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size *= 0.9;

    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(20);

    return r;
}

juce::String CustomGainSlider::getDisplayString()
{

    if (suffix.isNotEmpty())
    {
        if (getValue() == minusInfinityValue) {
            return juce::String("-inf dB");
        }
        else {
            return juce::String(getValue(), 2) + juce::String(" dB");
        }
    }
    else
    {
        return juce::String(getValue(), 1);
    }
}