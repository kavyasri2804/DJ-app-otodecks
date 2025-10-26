/*
  ==============================================================================

    DeckGUI.cpp
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse
           ) : player(_player), 
               waveformDisplay(formatManagerToUse, cacheToUse),
    queueComponent(_player, this) // Initialize the queue component
{

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
       
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    addAndMakeVisible(waveformDisplay);

    addAndMakeVisible(queueComponent); // Make queue visible

    addAndMakeVisible(trackNameLabel); // Make label visible

    // Add and configure labels
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedLabel);

    volLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);

    volLabel.setJustificationType(Justification::centred);
    speedLabel.setJustificationType(Justification::centred);

    volLabel.setColour(Label::textColourId, Colours::white);
    speedLabel.setColour(Label::textColourId, Colours::white);

    // Configure the label
    trackNameLabel.setText("No Track Loaded", dontSendNotification);
    trackNameLabel.setJustificationType(Justification::centred);
    trackNameLabel.setColour(Label::textColourId, Colours::white);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    volSlider.setRange(0.0, 1.0);
    volSlider.setSliderStyle(juce::Slider::Rotary);
    volSlider.setNumDecimalPlacesToDisplay(2);
    volSlider.setColour(Slider::thumbColourId, Colours::white);

    speedSlider.setRange(0.0, 3.0);  // Speed range from 0x to 3x
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setNumDecimalPlacesToDisplay(2);
    speedSlider.setColour(Slider::thumbColourId, Colours::white);

    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);  // Hide the value display
    posSlider.setColour(Slider::thumbColourId, Colours::white);

    playButton.setColour(TextButton::buttonColourId, Colours::navy);
    stopButton.setColour(TextButton::buttonColourId, Colours::indigo);
    loadButton.setColour(TextButton::buttonColourId, Colours::darkslateblue);

    playButton.setColour(TextButton::textColourOnId, Colours::white);
    stopButton.setColour(TextButton::textColourOnId, Colours::white);
    loadButton.setColour(TextButton::textColourOnId, Colours::white);

    startTimer(500);


}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{

    g.fillAll(Colours::slategrey);    //background color

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   //outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 10;

    // Play/Stop buttons 
    playButton.setBounds(0, 0, getWidth() / 2, rowH);
    stopButton.setBounds(getWidth() / 2, 0, getWidth() / 2, rowH);

    // Labels above sliders
    volLabel.setBounds(0, rowH, getWidth() / 2, rowH / 2);
    speedLabel.setBounds(getWidth() / 2, rowH, getWidth() / 2, rowH / 2);

    // Sliders below labels
    volSlider.setBounds(0, rowH + rowH / 2, getWidth() / 2, rowH * 2);
    speedSlider.setBounds(getWidth() / 2, rowH + rowH / 2, getWidth() / 2, rowH * 2);

    //position slider
    posSlider.setBounds(0, rowH * 3.5, getWidth(), rowH / 2);

    // Waveform
    waveformDisplay.setBounds(0, rowH * 4, getWidth(), rowH * 1.5);

    // Track name and load button
    trackNameLabel.setBounds(0, rowH * 5.5, getWidth(), rowH / 2);
    loadButton.setBounds(0, rowH * 6, getWidth(), rowH);

    // Queue
    queueComponent.setBounds(0, rowH * 7, getWidth(), rowH * 3);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->start();
    }
     if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        player->stop();

    }
     if (button == &loadButton)
     {
         auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
         fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
             {
                 File chosenFile = chooser.getResult(); // choose the selected file
                 if (chosenFile.existsAsFile())
                 {
                     player->loadURL(URL{ chosenFile });  // load track into player
                     waveformDisplay.loadURL(URL{ chosenFile }); // load into waveform display

                     // update track name label
                     trackNameLabel.setText(chosenFile.getFileNameWithoutExtension(), dontSendNotification);
                 }
             });
     }
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
    
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
  std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
  return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  std::cout << "DeckGUI::filesDropped" << std::endl;
  if (files.size() == 1)
  {
    player->loadURL(URL{File{files[0]}});
  }
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(
            player->getPositionRelative());
}

void DeckGUI::updateWaveformDisplay(URL audioURL)
{
    waveformDisplay.loadURL(audioURL);
}

void DeckGUI::updateTrackName(const String& trackName)
{
    trackNameLabel.setText(trackName, dontSendNotification);
}

// add a track to the queue
void DeckGUI::addToQueue(const URL& url, const String& trackName)
{
    queueComponent.addToQueue(url, trackName);
}


    

