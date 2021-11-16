/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
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
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat("feedback",
                                                                    "Feedback",
                                                                    0.0,
                                                                    0.98,
                                                                    0.5));
    addParameter(mDelayTimeParameter = new juce::AudioParameterFloat("delaytime",
                                                                     "Delay Time",
                                                                     0.01,
                                                                     MAX_DELAY_TIME,
                                                                     0.5));
    
    mDelayTimeSmoothed = 0;
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
    
   
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mDelayTimeInSamples = sampleRate * (*mDelayTimeParameter); //The 0.5 can be changed with a parameter to improve upon this!
    
    
    
    mCircularBufferLength = sampleRate*MAX_DELAY_TIME;
    
    mCircularBufferLeft.reset(new float[mCircularBufferLength]);
    juce::zeromem(mCircularBufferLeft.get(), mCircularBufferLength * sizeof(float));
    mCircularBufferRight.reset(new float[mCircularBufferLength]);
    juce::zeromem(mCircularBufferRight.get(), mCircularBufferLength * sizeof(float));
    
    mCircularBufferWriteHead = 0;
    
    mDelayTimeSmoothed = *mDelayTimeParameter;
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.001 * (mDelayTimeSmoothed - *mDelayTimeParameter);
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
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
