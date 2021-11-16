/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Chorus_FlangerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Chorus_FlangerAudioProcessorEditor (Chorus_FlangerAudioProcessor&);
    ~Chorus_FlangerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Chorus_FlangerAudioProcessor& audioProcessor;
    
//    juce::AudioParameterFloat* mDryWetParameter;
//    juce::AudioParameterFloat* mFeedbackParameter;
//    juce::AudioParameterFloat* mDepthParameter;
//    juce::AudioParameterFloat* mRateParameter;
//    juce::AudioParameterFloat* mPhaseOffsetParameter;
//
//    juce::AudioParameterInt* mTypeParameter;
    
    juce::Slider mDryWetSlider;
    juce::Label mDryWetLabel;
    juce::Slider mDepthSlider;
    juce::Label mDepthLabel;
    juce::Slider mRateSlider;
    juce::Label mRateLabel;
    juce::Slider mPhaseOffsetSlider;
    juce::Label mPhaseOffsetLabel;
    juce::Slider mFeedbackSlider;
    juce::Label mFeedbackLabel;
    
    juce::ComboBox mType;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Chorus_FlangerAudioProcessorEditor)
};
