/*
  ==============================================================================

    Queue.cpp
    Created: 8 Mar 2025 8:23:38am
    Author:  kavya

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Queue.h"
#include "DeckGUI.h"

//==============================================================================
Queue::Queue(DJAudioPlayer* _player, Component* _parentDeckGUI)
    : player(_player),
    parentDeckGUI(_parentDeckGUI)
{
    // Setup table
    addAndMakeVisible(queueTable);
    queueTable.setModel(this);
    queueTable.getHeader().addColumn("Queue Track", 1, 220);
    queueTable.getHeader().addColumn("Play", 2, 85);
    queueTable.getHeader().addColumn("Remove", 3, 85);
    queueTable.setColour(ListBox::backgroundColourId, Colours::navy);

    // Add buttons
    addAndMakeVisible(playNextButton);
    addAndMakeVisible(clearQueueButton);

    playNextButton.addListener(this);
    clearQueueButton.addListener(this);

    // Styling
    playNextButton.setColour(TextButton::buttonColourId, Colours::skyblue);
    clearQueueButton.setColour(TextButton::buttonColourId, Colours::orchid);

    playNextButton.setColour(TextButton::textColourOnId, Colours::white);
    clearQueueButton.setColour(TextButton::textColourOnId, Colours::white);
}

Queue::~Queue()
{
    playNextButton.removeListener(this);
    clearQueueButton.removeListener(this);
}

void Queue::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    g.setColour(Colours::white);
    g.setFont(14.0f);
    g.drawText("Track Queue", 10, 5, getWidth() - 20, 20, Justification::centredLeft);
}

void Queue::resized()
{
    // Layout components
    int margin = 5;
    int buttonHeight = 25;
    int buttonWidth = (getWidth() - margin * 3) / 2; // Half width minus margins

    // Buttons at the top
    playNextButton.setBounds(margin, margin, buttonWidth, buttonHeight);
    clearQueueButton.setBounds(margin * 2 + buttonWidth, margin, buttonWidth, buttonHeight);

    // Table for rest of the space
    queueTable.setBounds(margin,
        margin * 2 + buttonHeight,
        getWidth() - margin * 2,
        getHeight() - (margin * 3 + buttonHeight));
}

int Queue::getNumRows()
{
    return queuedTrackNames.size();
}

void Queue::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::orange);
    }
    else
    {
        g.fillAll(rowNumber % 2 == 0 ? Colours::darkgrey : Colours::darkgrey.darker(0.3f));
    }
}

void Queue::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < queuedTrackNames.size() && columnId == 1) // Track name column
    {
        g.setColour(Colours::white);
        g.setFont(14.0f);
        g.drawText(queuedTrackNames[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}

Component* Queue::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (columnId == 2) // Play column
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "O" };
            String id = "play_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            btn->setColour(TextButton::buttonColourId, Colours::green.darker(0.6f));
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 3) // Remove column
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "X" };
            String id = "remove_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            btn->setColour(TextButton::buttonColourId, Colours::red.darker(0.6f));
            existingComponentToUpdate = btn;
        }
    }

    return existingComponentToUpdate;
}

void Queue::buttonClicked(Button* button)
{
    if (button == &playNextButton)
    {
        playNextInQueue();
    }
    else if (button == &clearQueueButton)
    {
        clearQueue();
    }
    else
    {
        String id = button->getComponentID();

        if (id.startsWith("play_"))
        {
            int rowNumber = id.substring(5).getIntValue();
            if (rowNumber < queuedURLs.size())
            {
                // Play this specific track and remove it from queue
                URL url = queuedURLs[rowNumber];

                // Load and play the track
                player->loadURL(url);
                player->start();

                // Update DeckGUI
                if (DeckGUI* deckGUI = dynamic_cast<DeckGUI*>(parentDeckGUI))
                {
                    deckGUI->updateWaveformDisplay(url);
                    File file = url.getLocalFile();
                    deckGUI->updateTrackName(file.getFileNameWithoutExtension());
                }

                // Remove from queue
                queuedURLs.erase(queuedURLs.begin() + rowNumber);
                queuedTrackNames.erase(queuedTrackNames.begin() + rowNumber);
                queueTable.updateContent();
            }
        }
        else if (id.startsWith("remove_"))
        {
            int rowNumber = id.substring(7).getIntValue();
            if (rowNumber < queuedURLs.size())
            {
                // Remove this specific track from queue
                queuedURLs.erase(queuedURLs.begin() + rowNumber);
                queuedTrackNames.erase(queuedTrackNames.begin() + rowNumber);
                queueTable.updateContent();
            }
        }
    }
}

void Queue::addToQueue(const URL& url, const String& trackName)
{
    // Add to queue
    queuedURLs.push_back(url);
    queuedTrackNames.push_back(trackName);

    // Update the table
    queueTable.updateContent();
}

void Queue::clearQueue()
{
    queuedURLs.clear();
    queuedTrackNames.clear();
    queueTable.updateContent();
}

void Queue::playNextInQueue()
{
    if (!queuedURLs.empty())
    {
        // Get the next track
        URL url = queuedURLs[0];

        // Load and play the track
        player->loadURL(url);
        player->start();

        // Update DeckGUI
        if (DeckGUI* deckGUI = dynamic_cast<DeckGUI*>(parentDeckGUI))
        {
            deckGUI->updateWaveformDisplay(url);
            File file = url.getLocalFile();
            deckGUI->updateTrackName(file.getFileNameWithoutExtension());
        }

        // Remove from queue
        queuedURLs.erase(queuedURLs.begin());
        queuedTrackNames.erase(queuedTrackNames.begin());
        queueTable.updateContent();
    }
}
