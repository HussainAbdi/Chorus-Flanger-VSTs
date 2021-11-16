/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    auto& params = processor.getParameters();
    
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    
    mDryWetSlider.setBounds(0, 75, 100, 100);
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 25);
    mDryWetSlider.setColour(mDryWetSlider.textBoxOutlineColourId, juce::Colour());
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end, 0.01);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    
    mDryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    mDryWetLabel.attachToComponent(&mDryWetSlider, false);
    mDryWetLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mDryWetLabel);
    
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
    
    mFeedbackSlider.setBounds(150, 75, 100, 100);
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 25);
    mFeedbackSlider.setColour(mFeedbackSlider.textBoxOutlineColourId, juce::Colour());
    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end, 0.01);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    
    mFeedbackLabel.setText("Feedback", juce::dontSendNotification);
    mFeedbackLabel.attachToComponent(&mFeedbackSlider, false);
    mFeedbackLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mFeedbackLabel);
    
    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* delayTimeParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);
    
    mDelayTimeSlider.setBounds(300, 75, 100, 100);
    mDelayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDelayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 25);
    mDelayTimeSlider.setColour(mDelayTimeSlider.textBoxOutlineColourId, juce::Colour());
    mDelayTimeSlider.setRange(delayTimeParameter->range.start, delayTimeParameter->range.end, 0.01);
    mDelayTimeSlider.setValue(*delayTimeParameter);
    addAndMakeVisible(mDelayTimeSlider);
    
    mDelayTimeSlider.onValueChange = [this, delayTimeParameter] { *delayTimeParameter = mDelayTimeSlider.getValue(); };
    mDelayTimeSlider.onDragStart = [delayTimeParameter] { delayTimeParameter->beginChangeGesture(); };
    mDelayTimeSlider.onDragEnd = [delayTimeParameter] { delayTimeParameter->endChangeGesture(); };
    
    mDelayTimeLabel.setText("Delay Time", juce::dontSendNotification);
    mDelayTimeLabel.attachToComponent(&mDelayTimeSlider, false);
    mDelayTimeLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mDelayTimeLabel);
    
    
    
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font("Arial", 25.0f, juce::Font::bold));
    g.drawFittedText ("Delay", getLocalBounds(), juce::Justification::centredTop, 1);
}

void DelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
