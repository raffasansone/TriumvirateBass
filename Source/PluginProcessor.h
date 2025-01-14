/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetService/PresetManager.h"

constexpr auto BUILD_VERSION = JucePlugin_VersionString;

enum Slope
{
    Slope_12, // Decays to 0
    Slope_24, // Decays to 1, etc...
    Slope_36,
    Slope_48
};

struct TriumvirateBassSettings
{
    class DebugFloat
    {
    public:
        explicit DebugFloat(const float f) {
            m_float = f;
        }

        DebugFloat& operator=(const float f) {
            m_float = f;
            return *this;
        }

        operator float() const{
            return m_float;
        }

        operator float&() {
            return m_float;
        }

    private:
        float m_float;
    };

    DebugFloat input{ 0.0f }, output{ 0.0f };
    float highPassFreq{ 600.f }, lowPassFreq{ 140.f }, midHighPassFreq{ 100.f }, midLowPassFreq{ 800.f };
    float lowPreampGain{ 0.0f }, midPreampGain{ 0.0f }, highPreampGain{ 0.0f };
    float lowPostGain{ 1.0f }, midPostGain{ 1.0f }, highPostGain{ 1.0f };
    Slope lowPassSlope{ Slope::Slope_12 }, highPassSlope{ Slope::Slope_12 }, midLowPassSlope{ Slope::Slope_12 }, midHighPassSlope{Slope::Slope_12};
    bool bypass{ false };

    float& operator=(const float f) {

    }
};


//==============================================================================
/**
*/
class TriumvirateBassAudioProcessor  : public juce::AudioProcessor,
                                       public juce::AudioProcessorValueTreeState::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TriumvirateBassAudioProcessor();
    ~TriumvirateBassAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "PARAMETERS", createParameterLayout()};

    foleys::LevelMeterSource& getInputLevelMeterSource();
    foleys::LevelMeterSource& getOutputLevelMeterSource();
    
    service::PresetManager& getPresetManager() { return *presetManager; }

    static constexpr float MINUS_INFINITY_DB { -128.f };

private:
    foleys::LevelMeterSource inputLevelMeterSource, outputLevelMeterSource;

    TriumvirateBassSettings settings;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    bool distortionChanged {false};

    std::unique_ptr<service::PresetManager> presetManager;

    juce::AudioBuffer<float> midBuffer, highBuffer;
    float previousInputGain, previousOutputGain;
    using Waveshaper = juce::dsp::WaveShaper<float, std::function<float (float)>>;
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; // Filter up to 4th order
    using Gain = juce::dsp::Gain<float>;

    using LowChain = juce::dsp::ProcessorChain<CutFilter, Gain, Waveshaper, Filter, Gain>;
    LowChain lowLeftChain, lowRightChain;

    using MidChain = juce::dsp::ProcessorChain<CutFilter, CutFilter, Gain, Waveshaper, Filter, Gain>;
    MidChain midLeftChain, midRightChain;

    using HighChain = juce::dsp::ProcessorChain<CutFilter, Gain, Waveshaper, Gain>;
    HighChain highLeftChain, highRightChain;

    enum LowChainPositions
    {
        LowPassFilter,
        LowPreampGain,
        LowDistortion,
        LowPostFilter,
        LowPostGain
    };

    enum MidChainPositions
    {
        MidLowPassFilter,
        MidHighPassFilter,
        MidPreampGain,
        MidDistortion,
        MidPostFilter,
        MidPostGain
    };

    enum HighChainPositions
    {
        HighPassFilter,
        HighPreampGain,
        HighDistortion,
        HighPostGain
    };

    template<int Index, typename ChainType, typename CoefficientType>
    void updateCutFilterCoefficients(ChainType& chain, const CoefficientType& coefficients)
    {
        *(chain.template get<Index>().coefficients) = *(coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, 
        const CoefficientType& coefficients,
        const Slope& slope)
    {
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);

        switch (slope)
        {
        case Slope_48:
        {
            updateCutFilterCoefficients<3>(chain, coefficients);
        }

        case Slope_36:
        {
            updateCutFilterCoefficients<2>(chain, coefficients);
        }
        case Slope_24:
        {
            updateCutFilterCoefficients<1>(chain, coefficients);
        }
        case Slope_12:
        {
            updateCutFilterCoefficients<0>(chain, coefficients);
        }
        }
    }

    void initialisePostLowBandLowPass();
    void initialisePostMidBandLowPass();
    void updateHighPassFilters(const TriumvirateBassSettings& chainSettings);
    void updateBandPassFilters(const TriumvirateBassSettings& chainSettings);
    void updateLowPassFilters(const TriumvirateBassSettings& chainSettings);
    void updateFilters();
    void updateDistortions();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriumvirateBassAudioProcessor)
};
