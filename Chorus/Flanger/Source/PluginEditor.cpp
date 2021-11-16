/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Chorus_FlangerAudioProcessorEditor::Chorus_FlangerAudioProcessorEditor (Chorus_FlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    auto& params = processor.getParameters();
    
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    
    mDryWetSlider.setBounds(0, 125, 100, 100);
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
    
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue();};
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* depthParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
    
    mDepthSlider.setBounds(100, 125, 100, 100);
    mDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 25);
    mDepthSlider.setColour(mDepthSlider.textBoxOutlineColourId, juce::Colour());
    mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end, 0.01);
    mDepthSlider.setValue(*depthParameter);
    addAndMakeVisible(mDepthSlider);
    
    mDepthLabel.setText("Depth", juce::dontSendNotification);
    mDepthLabel.attachToComponent(&mDepthSlider, false);
    mDepthLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mDepthLabel);
    
    mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter = mDepthSlider.getValue();};
    mDepthSlider.onDragStart = [depthParameter] { depthParameter->beginChangeGesture(); };
    mDepthSlider.onDragEnd = [depthParameter] { depthParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* rateParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);
    
    mRateSlider.setBounds(200, 125, 100, 100);
    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 25);
    mRateSlider.setColour(mRateSlider.textBoxOutlineColourId, juce::Colour());
    mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end, 0.01);
    mRateSlider.setValue(*rateParameter);
    addAndMakeVisible(mRateSlider);
    
    mRateLabel.setText("Rate", juce::dontSendNotification);
    mRateLabel.attachToComponent(&mRateSlider, false);
    mRateLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mRateLabel);
    
    mRateSlider.onValueChange = [this, rateParameter] { *rateParameter = mRateSlider.getValue();};
    mRateSlider.onDragStart = [rateParameter] { rateParameter->beginChangeGesture(); };
    mRateSlider.onDragEnd = [rateParameter] { rateParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* phaseParameter = (juce::AudioParameterFloat*)params.getUnchecked(3);
    
    mPhaseOffsetSlider.setBounds(300, 0, 100, 100);
    mPhaseOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    mPhaseOffsetSlider.setRange(phaseParameter->range.start, phaseParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseParameter);
    //addAndMakeVisible(mPhaseOffsetSlider);
    
    mPhaseOffsetSlider.onValueChange = [this, phaseParameter] { *phaseParameter = mPhaseOffsetSlider.getValue();};
    mPhaseOffsetSlider.onDragStart = [phaseParameter] { phaseParameter->beginChangeGesture(); };
    mPhaseOffsetSlider.onDragEnd = [phaseParameter] { phaseParameter->endChangeGesture(); };
    
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(4);
    
    mFeedbackSlider.setBounds(300, 125, 100, 100);
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
    
    juce::AudioParameterInt* typeParameter = (juce::AudioParameterInt*)params.getUnchecked(5);
    
    mType.setBounds(150, 50, 100, 30);
    mType.addItem("Chorus", 1);
    mType.addItem("Flanger", 2);
    addAndMakeVisible(mType);
    
    mType.onChange = [this, typeParameter] {
        typeParameter->beginChangeGesture();
        *typeParameter = mType.getSelectedItemIndex();
        typeParameter->endChangeGesture();
    };
    
    mType.setSelectedItemIndex(*typeParameter);
    
}

Chorus_FlangerAudioProcessorEditor::~Chorus_FlangerAudioProcessorEditor()
{
}

//==============================================================================
void Chorus_FlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font("Arial", 25.0f, juce::Font::bold));
    g.drawFittedText ("Chorus/Flanger", getLocalBounds(), juce::Justification::centredTop, 1);
}

void Chorus_FlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
