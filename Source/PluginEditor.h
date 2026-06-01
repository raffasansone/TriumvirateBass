/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GraphicElements/CustomGainKnob.h"
#include "GraphicElements/InOutGainKnob.h"
#include "GraphicElements/RaffaLinearSlider.h"
#include "PresetGui/PresetPanel.h"

//==============================================================================
class SlidingToggleButton : public juce::Button {
public:
  
    SlidingToggleButton(juce::String name, bool isInverted, bool isVertical = false)
        : Button("SwitchButton"), isInverted(isInverted), isVertical(isVertical)
    {
        setClickingTogglesState(true);
        addAndMakeVisible(switchCircle);
        switchCircle.setWantsKeyboardFocus(false);
        switchCircle.setInterceptsMouseClicks(false, false);
    }

    void setMillisecondsToSpendMoving(int newValue)
    {
        millisecondsToSpendMoving = newValue;
    }

    void paintButton(juce::Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto b = getSwitchBounds();
        auto cornerSize = (isVertical ? b.getWidth() : b.getHeight()) * 0.5;
        g.setColour(juce::Colours::black.withAlpha(0.1f));
        g.drawRoundedRectangle(b, cornerSize, 2.0f);
        g.setColour(getSwitchState() ? juce::Colours::limegreen : juce::Colours::darkgrey);
        g.fillRoundedRectangle(b, cornerSize);

        juce::Path switchPath;
        switchPath.addRoundedRectangle(b, cornerSize, cornerSize);
        g.fillPath(switchPath);

        juce::Rectangle<float> switchCircleBounds;
        if (!isVertical)
            switchCircleBounds = { getSwitchState() ? 1.5f + b.getRight() - b.getHeight() : b.getX() - 1.5f, b.getY(), b.getHeight(), b.getHeight() };
        else
            switchCircleBounds = {
                b.getX(),
                getSwitchState() ? b.getBottom() - b.getWidth() - 1.5f : b.getY() + 1.5f,
                b.getWidth(),
                b.getWidth()
        };
        animator.animateComponent(&switchCircle, switchCircleBounds.reduced(1).toNearestInt(), 1.0, millisecondsToSpendMoving, false, 0.5, 0.5);
    }

    void resized() override
    {
        Button::resized();
        auto b = getSwitchBounds();
        juce::Rectangle<float> switchCircleBounds;
        if (!isVertical)
            switchCircleBounds = { getSwitchState() ? b.getRight() - b.getHeight() : b.getX(), b.getY(), b.getHeight(), b.getHeight() };
        else
            switchCircleBounds = {
                b.getX(),
                getSwitchState() ? b.getBottom() - b.getWidth() : b.getY(),
                b.getHeight(),
                b.getHeight()
        };
        switchCircle.setBounds(switchCircleBounds.reduced(1).toNearestInt());
    }

private:
    int millisecondsToSpendMoving{ 50 };

    bool getSwitchState() const
    {
        return isInverted ? !getToggleState() : getToggleState();
    }
    bool isInverted = false;
    bool isVertical = false;

    juce::Rectangle<float> getSwitchBounds()
    {
        auto b = getLocalBounds().toFloat().reduced(4, 4);
        return b;
    }

    juce::String onText, offText;
    class SwitchCircle : public Component
    {
        void paint(juce::Graphics& g) override
        {
            g.setColour(juce::Colours::white);
            g.fillEllipse(getLocalBounds().toFloat());
        }
    } switchCircle;
    juce::ComponentAnimator animator;
};

class RaffaEqualizerBox : public juce::Component {
public:
    RaffaEqualizerBox() {}

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::lightgrey);

        g.drawRect(logScaleBox);
        g.fillRectList(logScaleLines);
    }

    void initAtPosition(float xPadding, float yPadding, float width, float height) {
        clearLogScaleLines();
        for (float i = 50.f; i < 2000;) {
            float percent = juce::mapFromLog10(i, 20.f, 20000.f);
            auto line = juce::Rectangle<float>(xPadding + width * percent, yPadding, .5f, height);
            addScaleLine(line);
            //g.fillRect(padding + width * percent, 0.f, .1f, height);

            if (i < 100) i += 10.f;
            else if (i < 1000) i += 100.f;
            else if (i < 10000) i += 1000.f;
            else if (i < 20000) i += 10000.f;
        }
    }

    void clearLogScaleLines() { logScaleLines.clear(); };
    void addScaleLine(juce::Rectangle<float> line) { logScaleLines.addWithoutMerging(line); };

private:
    juce::Rectangle<float> logScaleBox;
    juce::RectangleList<float> logScaleLines;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RaffaEqualizerBox)
};

class SettingsToolbar : public juce::Component, public juce::Button::Listener
{
public:
    SettingsToolbar(TriumvirateBassAudioProcessor& p) : audioProcessor(p),
        cabinetToggleButton("Cabinet Emulation", false),
        highPassFreqSlider(*audioProcessor.apvts.getParameter("highPassFreq"), "High Band", "Hz"),
        lowPassFreqSlider(*audioProcessor.apvts.getParameter("lowPassFreq"), "Low Band", "Hz"),
        midHighPassFreqSlider(*audioProcessor.apvts.getParameter("midHighPassFreq"), "Mid HP", "Hz"),
        midLowPassFreqSlider(*audioProcessor.apvts.getParameter("midLowPassFreq"), "Mid LP", "Hz", true),
        cabinetToggleButtonAttachment(*audioProcessor.apvts.getParameter("cabinetEnabled"), cabinetToggleButton),
        highPassFreqSliderAttachment(audioProcessor.apvts, "highPassFreq", highPassFreqSlider),
        lowPassFreqSliderAttachment(audioProcessor.apvts, "lowPassFreq", lowPassFreqSlider),
        midHighPassFreqSliderAttachment(audioProcessor.apvts, "midHighPassFreq", midHighPassFreqSlider),
        midLowPassFreqSliderAttachment(audioProcessor.apvts, "midLowPassFreq", midLowPassFreqSlider),
        equalizerBox()
    {
        // Create and add the toolbar...
        addAndMakeVisible(toolbar);
        // And use our item factory to add a set of default icons to it...
        toolbar.addDefaultItems(factory);

        auto lowLabelFunction = [this]()
            {
                lowLabel.setText(juce::String(lowPassFreqSlider.getValue()) + " Hz LP",
                    juce::dontSendNotification);
            };

        auto highLabelFunction = [this]()
            {
                highLabel.setText(juce::String(highPassFreqSlider.getValue()) + " Hz HP",
                    juce::dontSendNotification);
            };

        auto midLabelFunction = [this]()
            {
                midLabel.setText(juce::String(midHighPassFreqSlider.getValue()) + " to " + juce::String(midLowPassFreqSlider.getValue()) + " Hz BP",
                    juce::dontSendNotification);
            };

        lowPassFreqSlider.onValueChange = lowLabelFunction;
        highPassFreqSlider.onValueChange = highLabelFunction;
        midHighPassFreqSlider.onValueChange = midLabelFunction;
        midLowPassFreqSlider.onValueChange = midLabelFunction;

        lowLabelFunction();
        midLabelFunction();
        highLabelFunction();

        cabinetPage.emplace_back(&cabinetToggleButton);
        cabinetPage.emplace_back(&cabinetLabel);
        cabinetPage.emplace_back(&cabinetDescription);

        for (auto* c : cabinetPage) {
            addAndMakeVisible(c);
        }

        crossoverPage.emplace_back(&equalizerBox);
        crossoverPage.emplace_back(&lowPassFreqSlider);
        crossoverPage.emplace_back(&lowLabel);
        crossoverPage.emplace_back(&midHighPassFreqSlider);
        crossoverPage.emplace_back(&midLowPassFreqSlider);
        crossoverPage.emplace_back(&midLabel);
        crossoverPage.emplace_back(&highPassFreqSlider);
        crossoverPage.emplace_back(&highLabel);
        crossoverPage.emplace_back(&crossoverDescription);

        for (auto* c : crossoverPage) {
            addChildComponent(c);
        }
    }

    void buttonClicked(juce::Button* b) override{
        if (auto* button = dynamic_cast<juce::ToolbarButton*>(b)) {
            if (button->getItemId() == SettingsToolbarItemFactory::SettingsToolbarItemIds::cabinet) {
                
                for (auto* c : cabinetPage) {
                    c->setVisible(true);
                    c->toFront(true);
                }
                
                for (auto* c : crossoverPage) {
                    c->setVisible(false);
                    c->toBack();
                }
            }
            else if (button->getItemId() == SettingsToolbarItemFactory::SettingsToolbarItemIds::crossover) {

                for (auto* c : crossoverPage) {
                    c->setVisible(true);
                    c->toFront(true);
                }

                for (auto* c : cabinetPage) {
                    c->setVisible(false);
                    c->toBack();
                }
            }
        }
    }
    
    void resized() override
    {
        toolbar.setBounds(getLocalBounds().removeFromTop(36).reduced(4,0));
        
        // Cabinet
        //================
        auto cabinetBounds = getLocalBounds().removeFromBottom(JUCE_LIVE_CONSTANT(70)).removeFromLeft(800).reduced(20, 5);
        auto cabinetButtonArea = cabinetBounds.removeFromLeft(30);
        cabinetToggleButton.setBounds(cabinetButtonArea
            .removeFromBottom(JUCE_LIVE_CONSTANT(40))
            .removeFromTop(JUCE_LIVE_CONSTANT(20)));
        cabinetLabel.setBounds(cabinetBounds.removeFromLeft(100));
        cabinetDescription.setBounds(cabinetBounds.reduced(40,0));

        // Crossover
        //================
        auto crossoverBounds = getLocalBounds().removeFromRight(380).removeFromBottom(74).reduced(20, 5);

        int width = crossoverBounds.getWidth()+40;
        int height = 60;
        float xPadding = 50.f;
        float yPadding = 10.f;

        int eqOffset = -280;
        int slidersOffset = -230;
        int descOffset = 40;
        int sliderHeight = 20;
        int labelWidth = 100;
        int labelHeight = 40;

        equalizerBox.setBounds(crossoverBounds.getX() + eqOffset, crossoverBounds.getY(), width, crossoverBounds.getHeight());
        equalizerBox.initAtPosition(xPadding, yPadding, width, height);

        lowPassFreqSlider.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset, crossoverBounds.getY()+10, width, sliderHeight));
        lowLabel.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset - 80, crossoverBounds.getY(), labelWidth, labelHeight));
        midHighPassFreqSlider.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset, crossoverBounds.getY()+30, width, sliderHeight));
        midLowPassFreqSlider.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset, crossoverBounds.getY()+30, width, sliderHeight));
        midLabel.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset-80, crossoverBounds.getY()+20, labelWidth, labelHeight));
        highPassFreqSlider.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset, crossoverBounds.getY()+50, width, sliderHeight));
        highLabel.setBounds(juce::Rectangle(crossoverBounds.getX() + slidersOffset-80, crossoverBounds.getY()+40, labelWidth, labelHeight));

        crossoverDescription.setBounds(crossoverBounds.getX() + descOffset, crossoverBounds.getY(), crossoverBounds.getWidth(), crossoverBounds.getHeight());
    }

    void paint(juce::Graphics& g) override{
        juce::Component::paint(g);
        g.fillAll(juce::Colour::fromRGBA(0, 0, 0, 200));
        
#ifdef DEBUG
        int childNum = 0;
        int toShow = JUCE_LIVE_CONSTANT(0);
        for (auto* child : getChildren())
        {
            ++childNum;
            if (childNum == toShow) {
                g.setColour(juce::Colours::lime);
                g.drawRect(child->getBounds());
                g.drawText(juce::String(childNum)+ " " + child->getName(),
                    child->getBounds(),
                    juce::Justification::centred);
            }
        }
#endif
    }

private:
    juce::Toolbar toolbar;

    TriumvirateBassAudioProcessor& audioProcessor;

    juce::Label cabinetLabel{ {"cabLabel"}, "Enable Cabinet"};
    juce::Label cabinetDescription{ {"cabDes"}, "This is the cabinet emulation. "
    "If you use this plugin on its own, the switch should be enabled.\n"
    "Deactivate the switch if you are running into an amp simulation, and want to use this effect just as a pedal."};
    
    SlidingToggleButton cabinetToggleButton;
    juce::ButtonParameterAttachment
        cabinetToggleButtonAttachment;
    
    juce::Label crossoverDescription{ {"crossoverDes"}, "This is the crossover section.\n"
        "Each band has individual, non linear filters.\n"
        "As such, it looks and sounds different from regular crossovers and produces unpredictable results."};

    RaffaLinearSlider highPassFreqSlider;
    RaffaLinearSlider lowPassFreqSlider;
    RaffaLinearSlider midHighPassFreqSlider;
    RaffaLinearSlider midLowPassFreqSlider;

    juce::Label lowLabel{ "lowLabel" };
    juce::Label midLabel{ "midLabel" };
    juce::Label highLabel{ "highLabel" };

    RaffaEqualizerBox equalizerBox;

    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    SliderAttachment 
        highPassFreqSliderAttachment,
        lowPassFreqSliderAttachment,
        midHighPassFreqSliderAttachment,
        midLowPassFreqSliderAttachment;

    std::vector<juce::Component*> cabinetPage;
    std::vector<juce::Component*> crossoverPage;

    //==============================================================================
    class SettingsToolbarItemFactory : public juce::ToolbarItemFactory
    {
    public:
        SettingsToolbarItemFactory(juce::Button::Listener* l) : listener(l) {}

        //==============================================================================
        // Each type of item a toolbar can contain must be given a unique ID.
        enum SettingsToolbarItemIds
        {
            cabinet = 1,
            crossover = 2,
        };

        void getAllToolbarItemIds(juce::Array<int>& ids) override
        {
            ids.add(cabinet);
            ids.add(crossover);

            ids.add(spacerId);
            //ids.add(separatorBarId);
            //ids.add(flexibleSpacerId);
        }

        void getDefaultItemSet(juce::Array<int>& ids) override
        {
            ids.add(cabinet);
            ids.add(spacerId);
            ids.add(crossover);
        }

        juce::ToolbarItemComponent* createItem(int itemId) override
        {
            switch (itemId)
            {
            case cabinet:           return createButtonFromId(itemId, "cabinet");
            case crossover:         return createButtonFromId(itemId, "crossover");
            default:                break;
            }

            return nullptr;
        }

    private:
        juce::ToolbarButton* createButtonFromId(const int itemId, const juce::String& text)
        {
            auto drawable = std::make_unique<juce::DrawableImage>();
            if (itemId == cabinet) {
                drawable->setImage(juce::ImageCache::getFromMemory(BinaryData::Speaker_EFEFEF_40x40_png, BinaryData::Speaker_EFEFEF_40x40_pngSize));
            }
            else {
                drawable->setImage(juce::ImageCache::getFromMemory(BinaryData::Crossover_EFEFEF_40x40_png, BinaryData::Crossover_EFEFEF_40x40_pngSize));
            }
            auto* c = new juce::ToolbarButton(itemId, text, std::move(drawable), {});
            c->addListener(listener);
            return c;
        }

        juce::Button::Listener* listener;
    };

    SettingsToolbarItemFactory factory{ this };
};

struct BypassButton : juce::ImageButton { 
    BypassButton(const juce::String& name = juce::String());
    bool hitTest(int x, int y) override; 
};

struct CustomLevelMeterLnF : foleys::LevelMeterLookAndFeel {
    CustomLevelMeterLnF();
    void setupDefaultMeterColours() override;
    void setupDefaultStereoFieldColours() override;
};

/**
*/
class TriumvirateBassAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TriumvirateBassAudioProcessorEditor (TriumvirateBassAudioProcessor&);
    ~TriumvirateBassAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void initialiseLevelMeters();
    void initialiseButtons();
    void paintBypassButton();
    void paintPreferencesButton();
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TriumvirateBassAudioProcessor& audioProcessor;

    gui::PresetPanel presetPanel;

    juce::Label versionLabel;

    SettingsToolbar preferencesPanel;

    CustomLevelMeterLnF levelMeterLnF;
    foleys::LevelMeter inputLevelMeter, outputLevelMeter;

    InOutGainSlider
        inputGainSlider,
        outputGainSlider,
        dryWetSlider;

    CustomGainSlider
        lowVolumeSlider,
        midVolumeSlider,
        highVolumeSlider,
        lowGainSlider,
        midGainSlider,
        highGainSlider;

    BypassButton bypassButton;
    juce::Image ledOff, ledOn;

    juce::ImageButton preferencesButton;
    juce::Image settingsImage;

    
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    SliderAttachment
        inputGainSliderAttachment,
        lowVolumeSliderAttachment,
        midVolumeSliderAttachment,
        highVolumeSliderAttachment,
        lowGainSliderAttachment,
        midGainSliderAttachment,
        highGainSliderAttachment,
        outputGainSliderAttachment,
        dryWetSliderAttachment;

    juce::ButtonParameterAttachment
        bypassButtonAttachment;

    juce::TooltipWindow tooltipWindow;

    std::vector<juce::Component*> getDefaultComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriumvirateBassAudioProcessorEditor)
};
