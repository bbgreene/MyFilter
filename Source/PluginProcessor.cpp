/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyFilterAudioProcessor::MyFilterAudioProcessor()
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
}

MyFilterAudioProcessor::~MyFilterAudioProcessor()
{
}

//==============================================================================
const juce::String MyFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    auto chainSettings = getChainSettings(apvts);
    
    auto passCoefficents = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.highPassFreq,
                                                                                                       sampleRate,
                                                                                                       2 * (chainSettings.highPassSlope + 1));
    auto& leftHighPass = leftChain.get<ChainPositions::HighPass>();
    
    leftHighPass.setBypassed<0>(true);
    leftHighPass.setBypassed<1>(true);
    leftHighPass.setBypassed<2>(true);
    leftHighPass.setBypassed<3>(true);
    
    switch (chainSettings.highPassSlope)
    {
            
        case Slope_12:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            break;
        }
        
        case Slope_24:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            break;
        }
        
        case Slope_36:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            *leftHighPass.get<2>().coefficients = *passCoefficents[2];
            leftHighPass.setBypassed<2>(false);
            break;
        }
        case Slope_48:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            *leftHighPass.get<2>().coefficients = *passCoefficents[2];
            leftHighPass.setBypassed<2>(false);
            *leftHighPass.get<3>().coefficients = *passCoefficents[3];
            leftHighPass.setBypassed<3>(false);
            break;
        }
    }
    
    auto& rightHighPass = rightChain.get<ChainPositions::HighPass>();
    
    rightHighPass.setBypassed<0>(true);
    rightHighPass.setBypassed<1>(true);
    rightHighPass.setBypassed<2>(true);
    rightHighPass.setBypassed<3>(true);
    
    switch (chainSettings.highPassSlope)
    {
            
        case Slope_12:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            break;
        }
        
        case Slope_24:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            break;
        }
        
        case Slope_36:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            *rightHighPass.get<2>().coefficients = *passCoefficents[2];
            rightHighPass.setBypassed<2>(false);
            break;
        }
        case Slope_48:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            *rightHighPass.get<2>().coefficients = *passCoefficents[2];
            rightHighPass.setBypassed<2>(false);
            *rightHighPass.get<3>().coefficients = *passCoefficents[3];
            rightHighPass.setBypassed<3>(false);
            break;
        }
    }
}

void MyFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void MyFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto chainSettings = getChainSettings(apvts);
    
    auto passCoefficents = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.highPassFreq,
                                                                                                       getSampleRate(),
                                                                                                       2 * (chainSettings.highPassSlope + 1));
    auto& leftHighPass = leftChain.get<ChainPositions::HighPass>();
    
    leftHighPass.setBypassed<0>(true);
    leftHighPass.setBypassed<1>(true);
    leftHighPass.setBypassed<2>(true);
    leftHighPass.setBypassed<3>(true);
    
    switch (chainSettings.highPassSlope)
    {
            
        case Slope_12:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            break;
        }
        
        case Slope_24:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            break;
        }
        
        case Slope_36:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            *leftHighPass.get<2>().coefficients = *passCoefficents[2];
            leftHighPass.setBypassed<2>(false);
            break;
        }
        case Slope_48:
        {
            *leftHighPass.get<0>().coefficients = *passCoefficents[0];
            leftHighPass.setBypassed<0>(false);
            *leftHighPass.get<1>().coefficients = *passCoefficents[1];
            leftHighPass.setBypassed<1>(false);
            *leftHighPass.get<2>().coefficients = *passCoefficents[2];
            leftHighPass.setBypassed<2>(false);
            *leftHighPass.get<3>().coefficients = *passCoefficents[3];
            leftHighPass.setBypassed<3>(false);
            break;
        }
    }
    
    auto& rightHighPass = rightChain.get<ChainPositions::HighPass>();
    
    rightHighPass.setBypassed<0>(true);
    rightHighPass.setBypassed<1>(true);
    rightHighPass.setBypassed<2>(true);
    rightHighPass.setBypassed<3>(true);
    
    switch (chainSettings.highPassSlope)
    {
            
        case Slope_12:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            break;
        }
        
        case Slope_24:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            break;
        }
        
        case Slope_36:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            *rightHighPass.get<2>().coefficients = *passCoefficents[2];
            rightHighPass.setBypassed<2>(false);
            break;
        }
        case Slope_48:
        {
            *rightHighPass.get<0>().coefficients = *passCoefficents[0];
            rightHighPass.setBypassed<0>(false);
            *rightHighPass.get<1>().coefficients = *passCoefficents[1];
            rightHighPass.setBypassed<1>(false);
            *rightHighPass.get<2>().coefficients = *passCoefficents[2];
            rightHighPass.setBypassed<2>(false);
            *rightHighPass.get<3>().coefficients = *passCoefficents[3];
            rightHighPass.setBypassed<3>(false);
            break;
        }
    }
    
    
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool MyFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyFilterAudioProcessor::createEditor()
{
//    return new MyFilterAudioProcessorEditor (*this);
    
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MyFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MyFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
    settings.highPassFreq = apvts.getRawParameterValue("HighPass Freq")->load();
    settings.lowPassFreq = apvts.getRawParameterValue("LowPass Freq")->load();
    settings.highPassSlope = static_cast<Slope>(apvts.getRawParameterValue("HighPass Slope")->load());
    settings.lowPassSlope = static_cast<Slope>(apvts.getRawParameterValue("LowPass Slope")->load());
    
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    MyFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPass Freq",
                                                           "HighPass Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.33f),
                                                           20.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPass Freq",
                                                           "LowPass Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.33f),
                                                           20000.f));
    
    juce::StringArray stringArray;
    for( int i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i*12);
        str << " db/Oct";
        stringArray.add(str);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighPass Slope", "HighPass Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowPass Slope", "LowPass Slope", stringArray, 0));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFilterAudioProcessor();
}
