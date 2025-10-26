/*
  ==============================================================================

    Queue.h
    Created: 8 Mar 2025 8:23:38am
    Author:  kavya

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include <vector>
#include <string>

//==============================================================================

class Queue : public Component,
    public TableListBoxModel,
    public Button::Listener
{
public:
    Queue(DJAudioPlayer* player, Component* parentDeckGUI);
    ~Queue() override;

    void paint(Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void buttonClicked(Button* button) override;

    // Queue management
    void addToQueue(const URL& url, const String& trackName);
    void clearQueue();
    void playNextInQueue();
    bool hasTracksInQueue() const { return !queuedURLs.empty(); }

private:
    TableListBox queueTable;
    std::vector<URL> queuedURLs;
    std::vector<String> queuedTrackNames;

    DJAudioPlayer* player;
    Component* parentDeckGUI;

    TextButton playNextButton{ "Play Next" };
    TextButton clearQueueButton{ "Clear Queue" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Queue)
};