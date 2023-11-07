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
class AudioFilePlayerVSTAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AudioFilePlayerVSTAudioProcessorEditor (AudioFilePlayerVSTAudioProcessor&);
    ~AudioFilePlayerVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    enum TransportState {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    TransportState state;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    void changeState(TransportState newState);
    void fileChosenCallBack(const juce::File&);
    void playButtonClicked();
    void pauseButtonClicked();
    void browseButtonClicked();
    AudioFilePlayerVSTAudioProcessor& audioProcessor;
    juce::TextButton stopButton;
    juce::TextButton playButton;
    juce::TextButton pauseButton;
    juce::TextButton browseButton;
    juce::Label fileNameLabel;
    juce::File selectedFile;
    std::unique_ptr<juce::FileChooser> fileChooser = std::make_unique<juce::FileChooser>("Select an Audio File", juce::File(), "*.wav");;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlayerVSTAudioProcessorEditor)
};
