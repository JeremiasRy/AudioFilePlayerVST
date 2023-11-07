/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFilePlayerVSTAudioProcessorEditor::AudioFilePlayerVSTAudioProcessorEditor (AudioFilePlayerVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);

    browseButton.setButtonText("Browse");
    browseButton.setBounds(10, 10, 80, 30);
    addAndMakeVisible(browseButton);

    playButton.setButtonText("Play");
    playButton.setBounds(10, 50, 80, 30);
    playButton.setEnabled(false);
    addAndMakeVisible(playButton);

    pauseButton.setButtonText("Pause");
    pauseButton.setBounds(10, 90, 80, 30);
    pauseButton.setEnabled(false);
    addAndMakeVisible(pauseButton);

    stopButton.setButtonText("Stop");
    stopButton.setBounds(10, 140, 80, 30);
    stopButton.setEnabled(false);
    addAndMakeVisible(stopButton);

    fileNameLabel.setText("No file selected", juce::NotificationType::dontSendNotification);
    fileNameLabel.setBounds(100, 10, 400, 30);
    addAndMakeVisible(fileNameLabel);

    stopButton.onClick = [this]() { stopButtonClicked(); };
    playButton.onClick = [this]() { playButtonClicked(); };
    pauseButton.onClick = [this]() { pauseButtonClicked(); };
    browseButton.onClick = [this]() { browseButtonClicked(); };
}

AudioFilePlayerVSTAudioProcessorEditor::~AudioFilePlayerVSTAudioProcessorEditor()
{
}

//==============================================================================
void AudioFilePlayerVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void AudioFilePlayerVSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void AudioFilePlayerVSTAudioProcessorEditor::playButtonClicked() 
{
    changeState(TransportState::Starting);
}

void AudioFilePlayerVSTAudioProcessorEditor::pauseButtonClicked()
{
    audioProcessor.pausePlayback();
}

void AudioFilePlayerVSTAudioProcessorEditor::browseButtonClicked()
{
    auto chooseFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    fileChooser->launchAsync(chooseFlags, [this](const juce::FileChooser& chooser)
        {
            juce::File selectedFile = chooser.getResult();
            if (selectedFile.exists()) {
                fileChosenCallBack(selectedFile);
            }
        });
}

void AudioFilePlayerVSTAudioProcessorEditor::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Stopped:                           
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            break;

        case Starting:                          
            playButton.setEnabled(false);
            audioProcessor.startPlayback();
            changeState(Playing);
            break;

        case Playing:                           
            stopButton.setEnabled(true);
            break;

        case Stopping:     
            audioProcessor.stopPlayback();
            changeState(Stopped);
            break;
        }
    }
}

void AudioFilePlayerVSTAudioProcessorEditor::fileChosenCallBack(const juce::File& file)
{
    audioProcessor.setFileForPlayback(file);
    fileNameLabel.setText(file.getFileName(), juce::NotificationType::dontSendNotification);
    browseButton.setButtonText("Change File...");
    playButton.setEnabled(true);
}

void AudioFilePlayerVSTAudioProcessorEditor::stopButtonClicked()
{
    changeState(Stopping);
}
