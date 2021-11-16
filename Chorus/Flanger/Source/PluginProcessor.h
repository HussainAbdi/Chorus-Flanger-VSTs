/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define MAX_DELAY_TIME 2

//==============================================================================
/**
*/
class Chorus_FlangerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Chorus_FlangerAudioProcessor();
    ~Chorus_FlangerAudioProcessor() override;

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
    
    float linear_interp(float sample_x, float sample_x1, float inPhase){
        return (1 - inPhase) * sample_x + inPhase * sample_x1;
    }


private:
    
    float mLFOPhase;
    
    float mDelayTimeSmoothed;
    
    juce::AudioParameterFloat* mDryWetParameter;
    juce::AudioParameterFloat* mDepthParameter;
    juce::AudioParameterFloat* mRateParameter;
    juce::AudioParameterFloat* mPhaseOffsetParameter;
    juce::AudioParameterFloat* mFeedbackParameter;
    
    
    juce::AudioParameterInt* mTypeParameter;
    
    float mDryWet;
    
    float mFeedbackLeft;
    float mFeedbackRight;
    
    float mDelayTimeInSamples;
    float mDelayReadHead;
    
    int mCircularBufferLength;
    int mCircularBufferWriteHead;
    
    
    std::unique_ptr<float> mCircularBufferLeft;
    std::unique_ptr<float> mCircularBufferRight;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Chorus_FlangerAudioProcessor)
};
