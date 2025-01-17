/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TriumvirateBassAudioProcessor::TriumvirateBassAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    apvts.addParameterListener("bypass", this);
    apvts.addParameterListener("inputGain", this);
    apvts.addParameterListener("outputGain", this);
    apvts.addParameterListener("highPassFreq", this);
    apvts.addParameterListener("highVolume", this);
    apvts.addParameterListener("highPreampGain", this);
    apvts.addParameterListener("lowPassFreq", this);
    apvts.addParameterListener("lowVolume", this);
    apvts.addParameterListener("lowPreampGain", this);
    apvts.addParameterListener("midHighPassFreq", this);
    apvts.addParameterListener("midLowPassFreq", this);
    apvts.addParameterListener("midVolume", this);
    apvts.addParameterListener("midPreampGain", this);

    apvts.state.setProperty(service::PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);

    presetManager = std::make_unique<service::PresetManager>(apvts);
}

TriumvirateBassAudioProcessor::~TriumvirateBassAudioProcessor()
{
}

//==============================================================================
const juce::String TriumvirateBassAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TriumvirateBassAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TriumvirateBassAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TriumvirateBassAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TriumvirateBassAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TriumvirateBassAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TriumvirateBassAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TriumvirateBassAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TriumvirateBassAudioProcessor::getProgramName (int index)
{
    return {};
}

void TriumvirateBassAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TriumvirateBassAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    // this prepares the meterSource to measure all output blocks and average over 100ms to allow smooth movements
    inputLevelMeterSource.resize(getTotalNumInputChannels(), sampleRate * 0.1 / samplesPerBlock);
    outputLevelMeterSource.resize(getTotalNumOutputChannels(), sampleRate * 0.1 / samplesPerBlock);

    previousInputGain = settings.input;

    lowLeftChain.prepare(spec);
    lowRightChain.prepare(spec);

    midLeftChain.prepare(spec);
    midRightChain.prepare(spec);

    highLeftChain.prepare(spec);
    highRightChain.prepare(spec);

    initialisePostLowBandLowPass();
    initialisePostMidBandLowPass();
    updateFilters();
    updateDistortions();
}

void TriumvirateBassAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TriumvirateBassAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void smoothGainTransition(juce::AudioBuffer<float>& buffer, float& newGain, float& previousGain) 
{
    if (newGain == previousGain)
    {
        buffer.applyGain(juce::Decibels::decibelsToGain(newGain));
    }
    else
    {
        buffer
            .applyGainRamp(
                0,
                buffer.getNumSamples(),
                juce::Decibels::decibelsToGain(previousGain),
                juce::Decibels::decibelsToGain(newGain));

        previousGain = newGain;
    }
}

void TriumvirateBassAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    smoothGainTransition(buffer, settings.input, previousInputGain);

    inputLevelMeterSource.measureBlock(buffer);
    
    if (!settings.bypass) 
    {
        // TODO variable cutoff
        //updateFilters();

        if (distortionChanged) 
        {
            updateDistortions();
            distortionChanged = false;
        }

        // Duplicate buffer for mid and high processing
        midBuffer.makeCopyOf(buffer, true);
        highBuffer.makeCopyOf(buffer, true);
    
        juce::dsp::AudioBlock<float> lowBlock(buffer);
        juce::dsp::AudioBlock<float> midBlock(midBuffer);
        juce::dsp::AudioBlock<float> highBlock(highBuffer);
 
        for (auto i = 0; i < totalNumOutputChannels; i++) 
        {
            // Low
            auto lowChannelBlock = lowBlock.getSingleChannelBlock(i);
            juce::dsp::ProcessContextReplacing<float> lowChannelContext(lowChannelBlock);
    
            if (settings.lowPostGain != TriumvirateBassAudioProcessor::MINUS_INFINITY_DB) 
            {
                if(i==0){
                    lowLeftChain.process(lowChannelContext);
                }
                else {
                    lowRightChain.process(lowChannelContext);
                }
            }
            else {
                lowChannelContext.getOutputBlock().clear();
            }
   
            // Mid
            auto midChannelBlock = midBlock.getSingleChannelBlock(i);
            juce::dsp::ProcessContextReplacing<float> midChannelContext(midChannelBlock);
    
            if (settings.midPostGain != TriumvirateBassAudioProcessor::MINUS_INFINITY_DB) 
            {
                if (i == 0) {
                    midLeftChain.process(midChannelContext);
                }
                else {
                    midRightChain.process(midChannelContext);
                }
            }
            else {
                midChannelContext.getOutputBlock().clear();
            }
        
            // High
            auto highChannelBlock = highBlock.getSingleChannelBlock(i);
            juce::dsp::ProcessContextReplacing<float> highChannelContext(highChannelBlock);

            if (settings.highPostGain != TriumvirateBassAudioProcessor::MINUS_INFINITY_DB) 
            {
                if (i == 0) {
                    highLeftChain.process(highChannelContext);
                }
                else {
                    highRightChain.process(highChannelContext);
                }
            }
            else {
                highChannelContext.getOutputBlock().clear();
            }

            // Sum
            lowChannelContext.getOutputBlock().add(midChannelBlock).add(highChannelBlock);
        }

    }

    smoothGainTransition(buffer, settings.output, previousOutputGain);

    outputLevelMeterSource.measureBlock(buffer);
}

//==============================================================================
bool TriumvirateBassAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TriumvirateBassAudioProcessor::createEditor()
{
    return new TriumvirateBassAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TriumvirateBassAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void TriumvirateBassAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

void TriumvirateBassAudioProcessor::initialisePostLowBandLowPass() 
{
    // Fixed Low Pass on bass freq
    auto postAmpLowPassCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        5500.f,
        getSampleRate(),
        1);

    auto& leftLowPass = lowLeftChain.get<LowChainPositions::LowPostFilter>();
    auto& rightLowPass = lowRightChain.get<LowChainPositions::LowPostFilter>();

    *leftLowPass.coefficients = *postAmpLowPassCoefficients[0];
    *rightLowPass.coefficients = *postAmpLowPassCoefficients[0];
}

void TriumvirateBassAudioProcessor::initialisePostMidBandLowPass() 
{
    // Fixed Low Pass on mid freq
    auto postAmpMidLowPassCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        13000.f,
        getSampleRate(),
        1);

    auto& midLeftLowPass = midLeftChain.get<MidChainPositions::MidPostFilter>();
    auto& midRightLowPass = midRightChain.get<MidChainPositions::MidPostFilter>();

    *midLeftLowPass.coefficients = *postAmpMidLowPassCoefficients[0];
    *midRightLowPass.coefficients = *postAmpMidLowPassCoefficients[0];
}

void TriumvirateBassAudioProcessor::updateHighPassFilters(const TriumvirateBassSettings& chainSettings)
{
    auto highPassCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.highPassFreq,
        getSampleRate(),
        (chainSettings.highPassSlope + 1) * 2);

    auto& leftHighPass = highLeftChain.get<HighChainPositions::HighPassFilter>();
    auto& rightHighPass = highRightChain.get<HighChainPositions::HighPassFilter>();
    
    updateCutFilter(leftHighPass, highPassCoefficients, chainSettings.highPassSlope);
    updateCutFilter(rightHighPass, highPassCoefficients, chainSettings.highPassSlope);
}

void TriumvirateBassAudioProcessor::updateBandPassFilters(const TriumvirateBassSettings& chainSettings)
{
    auto highPassCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.midHighPassFreq,
        getSampleRate(),
        (chainSettings.midHighPassSlope + 1) * 2);

    auto& leftHighPass = midLeftChain.get<MidChainPositions::MidHighPassFilter>();
    auto& rightHighPass = midRightChain.get<MidChainPositions::MidHighPassFilter>();

    updateCutFilter(leftHighPass, highPassCoefficients, chainSettings.midHighPassSlope);
    updateCutFilter(rightHighPass, highPassCoefficients, chainSettings.midHighPassSlope);

    auto lowPassCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.midLowPassFreq,
        getSampleRate(),
        (chainSettings.midLowPassSlope + 1) * 2);

    auto& leftLowPass = midLeftChain.get<MidChainPositions::MidLowPassFilter>();
    auto& rightLowPass = midRightChain.get<MidChainPositions::MidLowPassFilter>();

    updateCutFilter(leftLowPass, lowPassCoefficients, chainSettings.midLowPassSlope);
    updateCutFilter(rightLowPass, lowPassCoefficients, chainSettings.midLowPassSlope);
}

void TriumvirateBassAudioProcessor::updateLowPassFilters(const TriumvirateBassSettings& chainSettings)
{
    auto lowPassCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.lowPassFreq,
        getSampleRate(),
        (chainSettings.lowPassSlope + 1) * 2);

    auto& leftLowPass = lowLeftChain.get<LowChainPositions::LowPassFilter>();
    auto& rightLowPass = lowRightChain.get<LowChainPositions::LowPassFilter>();

    updateCutFilter(leftLowPass, lowPassCoefficients, chainSettings.lowPassSlope);
    updateCutFilter(rightLowPass, lowPassCoefficients, chainSettings.lowPassSlope);
}

void TriumvirateBassAudioProcessor::updateFilters()
{
    updateHighPassFilters(settings);
    updateBandPassFilters(settings);
    updateLowPassFilters(settings);
}

float distortionAlgorithm(float threshold, float hardness, float x) 
{
    float t = std::abs(x);
    if (t > threshold)
    {
        float sign = (x >= 0) ? 1.f : -1.f;
        float diff = t - threshold;
        x = sign * (threshold + diff / (hardness + diff));
    }
    return x;
}

void TriumvirateBassAudioProcessor::updateDistortions() 
{
    float(*d)(float, float, float) = distortionAlgorithm;

    // Values calculated from experimental measurements
    lowLeftChain.get<LowChainPositions::LowPreampGain>()
        .setGainDecibels(pow(2, settings.lowPreampGain / 18.1818f) + (settings.lowPreampGain * 5.56f) / 100 );
    lowRightChain.get<LowChainPositions::LowPreampGain>()
        .setGainDecibels(pow(2, settings.lowPreampGain / 18.1818f) + (settings.lowPreampGain * 5.56f) / 100 );

    lowLeftChain.get<LowChainPositions::LowPostGain>()
        .setGainDecibels(settings.lowPostGain +2.54f);
    lowRightChain.get<LowChainPositions::LowPostGain>()
        .setGainDecibels(settings.lowPostGain +2.54f);

    auto& lowLeftWaveshaper = lowLeftChain.get<LowChainPositions::LowDistortion>();
    auto& lowRightWaveshaper = lowRightChain.get<LowChainPositions::LowDistortion>();

    float threshold = std::pow(2, -(settings.lowPreampGain / 33.3336f));
    float hardness = 32;

    std::function<float (float)> lowDistortion = [d, threshold, hardness](float x)
    {
        return d(threshold, hardness, x);
    };

    lowLeftWaveshaper.functionToUse = lowDistortion;
    lowRightWaveshaper.functionToUse = lowDistortion;

    /// ///

    midLeftChain.get<MidChainPositions::MidPreampGain>()
        .setGainDecibels(pow(2, settings.midPreampGain / 27.2727f) + (settings.midPreampGain * 32.0f) / 100 +8.f);
    midRightChain.get<MidChainPositions::MidPreampGain>()
        .setGainDecibels(pow(2, settings.midPreampGain / 27.2727f) + (settings.midPreampGain * 32.0f) / 100 +8.f);

    midLeftChain.get<MidChainPositions::MidPostGain>()
        .setGainDecibels(settings.midPostGain + pow(2, settings.midPreampGain / 27.2727f) - 15.f);
    midRightChain.get<MidChainPositions::MidPostGain>()
        .setGainDecibels(settings.midPostGain + pow(2, settings.midPreampGain / 27.2727f) - 15.f);

    auto& midLeftWaveshaper = midLeftChain.get<MidChainPositions::MidDistortion>();
    auto& midRightWaveshaper = midRightChain.get<MidChainPositions::MidDistortion>();
    
    threshold = std::pow(2, -(settings.midPreampGain / 33.3336f));
    hardness = 64;

    std::function<float(float)> midDistortion = [d, threshold, hardness](float x)
    {
        return d(threshold, hardness, x);
    };

    midLeftWaveshaper.functionToUse = midDistortion;
    midRightWaveshaper.functionToUse = midDistortion;

    /// ///

    highLeftChain.get<HighChainPositions::HighPreampGain>()
        .setGainDecibels(pow(2, settings.highPreampGain / 27.2727f) + (settings.highPreampGain * 20.0f) / 100 + 8.0f);
    highRightChain.get<HighChainPositions::HighPreampGain>()
        .setGainDecibels(pow(2, settings.highPreampGain / 27.2727f) + (settings.highPreampGain * 20.0f) / 100 + 8.0f);

    highLeftChain.get<HighChainPositions::HighPostGain>()
        .setGainDecibels(settings.highPostGain + (pow(2, settings.highPreampGain / 27.2727f)) - 9.0f);
    highRightChain.get<HighChainPositions::HighPostGain>()
        .setGainDecibels(settings.highPostGain + (pow(2, settings.highPreampGain / 27.2727f)) - 9.0f);

    auto& highLeftWaveshaper = highLeftChain.get<HighChainPositions::HighDistortion>();
    auto& highRightWaveshaper = highRightChain.get<HighChainPositions::HighDistortion>();

    threshold = std::pow(2, -(settings.highPreampGain / 22.2222f));
    hardness = 64;

    std::function<float(float)> highDistortion = [d, threshold, hardness](float x)
    {
        return d(threshold, hardness, x);
    };

    highLeftWaveshaper.functionToUse = highDistortion;
    highRightWaveshaper.functionToUse = highDistortion;
}

juce::AudioProcessorValueTreeState::ParameterLayout
TriumvirateBassAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    auto dBGainParamAttributes = juce::AudioParameterFloatAttributes().withStringFromValueFunction([](auto value, auto) 
        { 
            if (value == TriumvirateBassAudioProcessor::MINUS_INFINITY_DB) 
            {
                return juce::String("-inf");
            }
            else 
            {
                return juce::String(value, 2);
            }
        })
        .withLabel("dB");

    layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain", "Input",
        juce::NormalisableRange<float>(-12.0f, 12.f, 0.01f, 2.f, true), 0.f, dBGainParamAttributes));

    //TODO: variable crossover
    //layout.add(std::make_unique<juce::AudioParameterFloat>("lowPassFreq", "lowPassFreq",
    //    juce::NormalisableRange<float>(70.f, 210.f, 1.f, 0.25f, false), 140.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowPreampGain", "Low Preamp Gain",
        juce::NormalisableRange<float>(0.0f, 100.f, 0.05f, 2.5f, false), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowVolume", "Low Volume",
        juce::NormalisableRange<float>(-128.f, 0.f, 0.01f, 5.f, false), 0.f, dBGainParamAttributes));

    //TODO: variable crossover
    //layout.add(std::make_unique<juce::AudioParameterFloat>("highPassFreq", "highPassFreq",
    //    juce::NormalisableRange<float>(400.f, 800.f, 1.f, 0.25f, false), 600.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highPreampGain", "High Preamp Gain",
        juce::NormalisableRange<float>(0.0f, 100.f, 0.05f, 2.5f, false), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highVolume", "High Volume",
        juce::NormalisableRange<float>(-128.f, 0.f, 0.01f, 5.f, false), 0.f, dBGainParamAttributes));

    //TODO: variable crossover
    //layout.add(std::make_unique<juce::AudioParameterFloat>("midHighPassFreq", "midHighPassFreq",
    //    juce::NormalisableRange<float>(80.f, 200.f, 1.f, 0.25f, false), 80.f));
    //layout.add(std::make_unique<juce::AudioParameterFloat>("midLowPassFreq", "midLowPassFreq",
    //    juce::NormalisableRange<float>(600.f, 1000.f, 1.f, 0.25f, false), 800.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midPreampGain", "Mid Preamp Gain",
        juce::NormalisableRange<float>(0.0f, 100.f, 0.05f, 2.5f, false), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midVolume", "Mid Volume",
        juce::NormalisableRange<float>(-128.f, 0.f, 0.01f, 5.f, false), 0.f, dBGainParamAttributes));

    layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain", "Output",
        juce::NormalisableRange<float>(-12.0f, 12.f, 0.01f, 2.f, true), 0.f, dBGainParamAttributes));

    //juce::StringArray stringArray;
    //for (int i = 0; i < 4; i++)
    //{
    //    juce::String str;
    //    str << (12 + i * 12);
    //    str << " dB/Oct";
    //    stringArray.add(str);
    //}

    //layout.add(std::make_unique<juce::AudioParameterChoice>("lowPassSlope", "lowPassSlope", stringArray, 0));
    //layout.add(std::make_unique<juce::AudioParameterChoice>("highPassSlope", "highPassSlope", stringArray, 0));
    //layout.add(std::make_unique<juce::AudioParameterChoice>("midHighPassSlope", "midHighPassSlope", stringArray, 0));
    //layout.add(std::make_unique<juce::AudioParameterChoice>("midLowPassSlope", "midLowPassSlope", stringArray, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false, "Bypass"));

    return layout;
}

foleys::LevelMeterSource& TriumvirateBassAudioProcessor::getInputLevelMeterSource()
{
    return inputLevelMeterSource;
}

foleys::LevelMeterSource& TriumvirateBassAudioProcessor::getOutputLevelMeterSource()
{
    return outputLevelMeterSource;
}

void TriumvirateBassAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    
    if (parameterID == "bypass") {
        settings.bypass = newValue;
        return;
    }

    if (parameterID == "inputGain") {
        settings.input = newValue;
        return;
    }

    if (parameterID == "outputGain") {
        settings.output = newValue;
        return;
    }

    distortionChanged = true;
    
    if (parameterID == "highPassFreq")
        settings.highPassFreq = newValue;
    
    if (parameterID == "highVolume")
        settings.highPostGain = newValue;
    
    if (parameterID == "highPreampGain")
        settings.highPreampGain = newValue;
        
    if (parameterID == "lowPassFreq")
        settings.lowPassFreq = newValue;
    
    if (parameterID == "lowVolume")
        settings.lowPostGain = newValue;
    
    if (parameterID == "lowPreampGain")
        settings.lowPreampGain = newValue;
    
    if (parameterID == "midHighPassFreq")
        settings.midHighPassFreq = newValue;
    
    if (parameterID == "midLowPassFreq")
        settings.midLowPassFreq = newValue;
    
    if (parameterID == "midVolume")
        settings.midPostGain = newValue;
    
    if (parameterID == "midPreampGain")
        settings.midPreampGain = newValue;
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TriumvirateBassAudioProcessor();
}
