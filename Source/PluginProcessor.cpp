/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioFilePlayerVSTAudioProcessor::AudioFilePlayerVSTAudioProcessor()
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
    formatManager.registerBasicFormats();
    resamplingTarget = 0;
}

AudioFilePlayerVSTAudioProcessor::~AudioFilePlayerVSTAudioProcessor()
{
}

//==============================================================================
const juce::String AudioFilePlayerVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioFilePlayerVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioFilePlayerVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioFilePlayerVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioFilePlayerVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioFilePlayerVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioFilePlayerVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioFilePlayerVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AudioFilePlayerVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void AudioFilePlayerVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudioFilePlayerVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    resamplingTarget = sampleRate;
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AudioFilePlayerVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioFilePlayerVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioFilePlayerVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));
}

//==============================================================================
bool AudioFilePlayerVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioFilePlayerVSTAudioProcessor::createEditor()
{
    return new AudioFilePlayerVSTAudioProcessorEditor (*this);
}

//==============================================================================
void AudioFilePlayerVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AudioFilePlayerVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void AudioFilePlayerVSTAudioProcessor::setFileForPlayback(const juce::File& file)
{
    fileForPlayback = file;
}

void AudioFilePlayerVSTAudioProcessor::continuePlayback()
{
    transportSource.start();
}

void AudioFilePlayerVSTAudioProcessor::startPlayback()
{
    juce::AudioFormatReader* reader = formatManager.createReaderFor(fileForPlayback);
    transportSource.prepareToPlay(0, resamplingTarget);

    if (reader != nullptr) 
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
    transportSource.start();
}

void AudioFilePlayerVSTAudioProcessor::pausePlayback()
{
    transportSource.stop();
}

void AudioFilePlayerVSTAudioProcessor::stopPlayback()
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.release();
    transportSource.releaseResources();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioFilePlayerVSTAudioProcessor();
}
