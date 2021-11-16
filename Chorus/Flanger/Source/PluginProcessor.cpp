/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Chorus_FlangerAudioProcessor::Chorus_FlangerAudioProcessor()
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
    addParameter(mDryWetParameter = new juce::AudioParameterFloat("drywet",
                                                                  "Dry/Wet",
                                                                  0.0,
                                                                  1.0,
                                                                  0.5));
    addParameter(mDepthParameter = new juce::AudioParameterFloat("depth",
                                                                 "Depth",
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.5f));
    addParameter(mRateParameter = new juce::AudioParameterFloat("rate",
                                                                 "Rate",
                                                                 0.1f,
                                                                 20.0f,
                                                                 10.0f));
    addParameter(mPhaseOffsetParameter = new juce::AudioParameterFloat("phaseoffset",
                                                                 "Phase Offset",
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.0f));
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat("feedback",
                                                                    "Feedback",
                                                                    0.0,
                                                                    0.98,
                                                                    0.5));
    addParameter(mTypeParameter = new juce::AudioParameterInt("type",
                                                              "Type",
                                                              0,
                                                              1,
                                                              0));
    
    mDelayTimeSmoothed = 0;
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
    
   
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    
    mLFOPhase = 0;
}

Chorus_FlangerAudioProcessor::~Chorus_FlangerAudioProcessor()
{
}

//==============================================================================
const juce::String Chorus_FlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Chorus_FlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Chorus_FlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Chorus_FlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Chorus_FlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Chorus_FlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Chorus_FlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Chorus_FlangerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Chorus_FlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void Chorus_FlangerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Chorus_FlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mLFOPhase = 0;
    
    mCircularBufferLength = sampleRate*MAX_DELAY_TIME;
    
    mCircularBufferLeft.reset(new float[mCircularBufferLength]);
    juce::zeromem(mCircularBufferLeft.get(), mCircularBufferLength * sizeof(float));
    mCircularBufferRight.reset(new float[mCircularBufferLength]);
    juce::zeromem(mCircularBufferRight.get(), mCircularBufferLength * sizeof(float));
    
    mCircularBufferWriteHead = 0;
    
    mDelayTimeSmoothed = 1;
}

void Chorus_FlangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Chorus_FlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Chorus_FlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    for (int i = 0; i < buffer.getNumSamples(); i++){
        
        float lfoOut = sin(2*M_PI * mLFOPhase);
        
        mLFOPhase += *mRateParameter / getSampleRate();
        
        if (mLFOPhase > 1){
            mLFOPhase -= 1;
        }
        
        lfoOut *= *mDepthParameter;
        
        float lfoOutMapped = 0;
        
        //If we've selected chorus or flanger
        if (*mTypeParameter == 0){
            lfoOutMapped = juce::jmap(lfoOut, -1.0f, 1.0f, 0.005f, 0.03f); //Chorus
        }else{
            lfoOutMapped = juce::jmap(lfoOut, -1.0f, 1.0f, 0.001f, 0.005f); //Flanger
        }
        
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.001 * (mDelayTimeSmoothed - lfoOutMapped);
        mDelayTimeInSamples = getSampleRate() * mDelayTimeSmoothed;
        
        mCircularBufferLeft.get()[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight.get()[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
        
        mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
        
        if (mDelayReadHead < 0){
            mDelayReadHead = mCircularBufferLength + mDelayReadHead;
        }
        
        int readHeadInt_x = (int)mDelayReadHead;
        int readHeadInt_x1 = readHeadInt_x + 1;
        float readHeadRemainderFloat = mDelayReadHead - readHeadInt_x;
        if (readHeadInt_x >= mCircularBufferLength){
            readHeadInt_x -= mCircularBufferLength; //Wrapping around circular buffer if we are over the length
        }
        
        float delay_sample_left = linear_interp(mCircularBufferLeft.get()[readHeadInt_x], mCircularBufferLeft.get()[readHeadInt_x1], readHeadRemainderFloat);
        float delay_sample_right =  linear_interp(mCircularBufferRight.get()[readHeadInt_x], mCircularBufferRight.get()[readHeadInt_x1], readHeadRemainderFloat);
        
        mFeedbackLeft = delay_sample_left * (*mFeedbackParameter);
        mFeedbackRight = delay_sample_right * (*mFeedbackParameter);
        
        buffer.setSample(0, i, buffer.getSample(0, i) * (1-(*mDryWetParameter)) + delay_sample_left * (*mDryWetParameter));
        buffer.setSample(1, i, buffer.getSample(1, i) * (1-(*mDryWetParameter)) + delay_sample_right * (*mDryWetParameter));
        
        mCircularBufferWriteHead++;
        if (mCircularBufferWriteHead >= mCircularBufferLength){
            mCircularBufferWriteHead = 0;
        }
        
    }
}

//==============================================================================
bool Chorus_FlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Chorus_FlangerAudioProcessor::createEditor()
{
    return new Chorus_FlangerAudioProcessorEditor (*this);
}

//==============================================================================
void Chorus_FlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Chorus_FlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Chorus_FlangerAudioProcessor();
}
