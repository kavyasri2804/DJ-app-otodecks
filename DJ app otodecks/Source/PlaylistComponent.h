/*
  ==============================================================================

    PlaylistComponent.h
    Created: 15 Aug 2023 4:59:38pm
    Author:  Liu Junhua

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "DeckGUI.h" 


//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component, public TableListBoxModel, public Button::Listener, public FileDragAndDropTarget
{
public:
    PlaylistComponent(DJAudioPlayer* _player1,
        DJAudioPlayer* _player2,
        DeckGUI* _deckGUI1,
        DeckGUI* _deckGUI2,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    int getNumRows() override;
    void paintRowBackground(Graphics & g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;
    
    void buttonClicked(Button * button) override;

    // File drag and drop methods
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    // Load to specific deck
    void loadToDeck(int deckNumber, URL audioURL);

    // Playlist management
    void savePlaylist();
    void loadPlaylist();
    void addToPlaylist(File file);
    void removeFromPlaylist();
    
private:
    TableListBox tableComponent;
    std::vector<std::string> trackTitles;

    std::vector<URL> trackURLs;
    std::vector<double> trackLengths;

    DJAudioPlayer* player1;
    DJAudioPlayer* player2;
    DeckGUI* deckGUI1; 
    DeckGUI* deckGUI2;  
    AudioFormatManager& formatManager;
    AudioThumbnailCache& thumbCache;

    String getTrackLength(URL audioURL);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
