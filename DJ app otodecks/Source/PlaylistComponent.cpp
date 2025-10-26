/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 15 Aug 2023 4:59:38pm
    Author:  Liu Junhua

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "DeckGUI.h"

PlaylistComponent::PlaylistComponent(DJAudioPlayer* _player1,
    DJAudioPlayer* _player2,
    DeckGUI* _deckGUI1,
    DeckGUI* _deckGUI2,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse)
    : player1(_player1),
    player2(_player2),
    deckGUI1(_deckGUI1),
    deckGUI2(_deckGUI2),
    formatManager(formatManagerToUse),
    thumbCache(cacheToUse)
{

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    // Initialize the table with appropriate columns
    tableComponent.getHeader().addColumn("Track Title", 1, 200);
    tableComponent.getHeader().addColumn("Length", 2, 100);
    tableComponent.getHeader().addColumn("Deck 1", 3, 100);
    tableComponent.getHeader().addColumn("Deck 2", 4, 100);
    tableComponent.getHeader().addColumn("Queue 1", 5, 100);
    tableComponent.getHeader().addColumn("Queue 2", 6, 100);
    tableComponent.getHeader().addColumn("Delete", 7, 80);

    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);

    // load an existing playlist
    loadPlaylist();
}

PlaylistComponent::~PlaylistComponent()
{
    // Save playlist when component is destroyed
    savePlaylist();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(Colours::indigo);
    g.setColour(Colours::white);
    g.setFont(16.0f);
    g.drawText("Playlist", 10, 5, getWidth() - 20, 20, Justification::centredLeft);
}

void PlaylistComponent::resized()
{
    // Layout the components
    int margin = 10;

    // Position the table to use the full component height
    tableComponent.setBounds(margin,
        margin,
        getWidth() - (margin * 2),
        getHeight() - (margin * 2));
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(Colours::orange);
    }
    else {
        g.fillAll(Colours::darkgrey);
    }
}

void PlaylistComponent::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowNumber < trackTitles.size())
    {
        if (columnId == 1) // Track Title
        {
            g.drawText(trackTitles[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);
        }
        else if (columnId == 2 && rowNumber < trackLengths.size()) // Length
        {
            double lengthInSeconds = trackLengths[rowNumber];
            int minutes = static_cast<int>(lengthInSeconds) / 60;
            int seconds = static_cast<int>(lengthInSeconds) % 60;

            String timeStr = String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
            g.drawText(timeStr, 2, 0, width - 4, height, Justification::centred, true);
        }
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{
    // Create buttons for the deck loading and delete columns
    if (columnId == 3) // Deck 1
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Load 1" };
            String id = "deck1_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 4) // Deck 2
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Load 2" };
            String id = "deck2_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 5) // Queue 1
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Queue 1" };
            String id = "queue1_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 6) // Queue 2
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Queue 2" };
            String id = "queue2_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 7) // Delete
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "X" };
            String id = "delete_" + String(rowNumber);
            btn->setComponentID(id);
            btn->addListener(this);
            btn->setColour(TextButton::buttonColourId, Colours::red);
            existingComponentToUpdate = btn;
        }
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    String id = button->getComponentID();

    if (id.startsWith("deck1_"))
    {
        int rowNumber = id.substring(6).getIntValue();
        if (rowNumber < trackURLs.size())
        {
            loadToDeck(1, trackURLs[rowNumber]);
        }
    }
    else if (id.startsWith("deck2_"))
    {
        int rowNumber = id.substring(6).getIntValue();
        if (rowNumber < trackURLs.size())
        {
            loadToDeck(2, trackURLs[rowNumber]);
        }
    }
    else if (id.startsWith("queue1_"))
    {
        int rowNumber = id.substring(7).getIntValue();
        if (rowNumber < trackURLs.size() && deckGUI1 != nullptr)
        {
            // Add to queue of deck 1
            URL url = trackURLs[rowNumber];
            File file = url.getLocalFile();
            deckGUI1->addToQueue(url, file.getFileNameWithoutExtension());
        }
    }
    else if (id.startsWith("queue2_"))
    {
        int rowNumber = id.substring(7).getIntValue();
        if (rowNumber < trackURLs.size() && deckGUI2 != nullptr)
        {
            // Add to queue of deck 2
            URL url = trackURLs[rowNumber];
            File file = url.getLocalFile();
            deckGUI2->addToQueue(url, file.getFileNameWithoutExtension());
        }
    }
    else if (id.startsWith("delete_"))
    {
        int rowNumber = id.substring(7).getIntValue();
        if (rowNumber < trackTitles.size())
        {
            trackTitles.erase(trackTitles.begin() + rowNumber);
            trackURLs.erase(trackURLs.begin() + rowNumber);
            if (rowNumber < trackLengths.size())
            {
                trackLengths.erase(trackLengths.begin() + rowNumber);
            }
            tableComponent.updateContent();
        }
    }
}

bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    // Accept files with audio extensions
    for (const String& file : files)
    {
        if (file.endsWithIgnoreCase(".wav") || file.endsWithIgnoreCase(".mp3") ||
            file.endsWithIgnoreCase(".aif") || file.endsWithIgnoreCase(".aiff"))
        {
            return true;
        }
    }
    return false;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    // Add dropped files to the playlist
    for (const String& filename : files)
    {
        File file(filename);
        addToPlaylist(file);
    }
}

void PlaylistComponent::loadToDeck(int deckNumber, URL audioURL)
{
    File file = audioURL.getLocalFile();

    if (deckNumber == 1 && player1 != nullptr)
    {
        player1->loadURL(audioURL);
        // Update waveform and track name in DeckGUI1
        if (deckGUI1 != nullptr)
        {
            deckGUI1->updateWaveformDisplay(audioURL);
            deckGUI1->updateTrackName(file.getFileNameWithoutExtension());
        }
    }
    else if (deckNumber == 2 && player2 != nullptr)
    {
        player2->loadURL(audioURL);
        // Update waveform and track name in DeckGUI2
        if (deckGUI2 != nullptr)
        {
            deckGUI2->updateWaveformDisplay(audioURL);
            deckGUI2->updateTrackName(file.getFileNameWithoutExtension());
        }
    }
}

void PlaylistComponent::savePlaylist()
{
    // Create an XML document to store the playlist
    std::unique_ptr<XmlElement> playlist = std::make_unique<XmlElement>("PLAYLIST");

    for (int i = 0; i < trackTitles.size(); ++i)
    {
        if (i < trackURLs.size())
        {
            auto track = std::make_unique<XmlElement>("TRACK");
            track->setAttribute("title", trackTitles[i]);
            track->setAttribute("url", trackURLs[i].toString(false));

            if (i < trackLengths.size())
            {
                track->setAttribute("length", String(trackLengths[i]));
            }

            playlist->addChildElement(track.release());
        }
    }

    // Save the XML to a file
    File playlistFile(File::getSpecialLocation(File::userDocumentsDirectory)
        .getChildFile("DJPlaylist.xml"));
    playlist->writeToFile(playlistFile, "");
}

void PlaylistComponent::loadPlaylist()
{
    // Clear existing tracks
    trackTitles.clear();
    trackURLs.clear();
    trackLengths.clear();

    // Load from XML file
    File playlistFile(File::getSpecialLocation(File::userDocumentsDirectory)
        .getChildFile("DJPlaylist.xml"));

    if (playlistFile.existsAsFile())
    {
        std::unique_ptr<XmlElement> playlistElement = XmlDocument::parse(playlistFile);

        if (playlistElement != nullptr)
        {
            if (playlistElement->hasTagName("PLAYLIST"))
            {
                forEachXmlChildElement(*playlistElement, trackElement)
                {
                    if (trackElement->hasTagName("TRACK"))
                    {
                        String title = trackElement->getStringAttribute("title");
                        String urlString = trackElement->getStringAttribute("url");
                        URL url(urlString);

                        if (url.isLocalFile())
                        {
                            double length = trackElement->getDoubleAttribute("length", 0.0);

                            trackTitles.push_back(title.toStdString());
                            trackURLs.push_back(url);
                            trackLengths.push_back(length);
                        }
                    }
                }
            }
        }
    }

    tableComponent.updateContent();
}

void PlaylistComponent::addToPlaylist(File file)
{
    if (file.existsAsFile())
    {
        URL audioURL = URL(file);

        // Get the track title
        String title = file.getFileNameWithoutExtension();

        // Check if the track is already in the playlist
        bool isDuplicate = false;
        for (const URL& url : trackURLs)
        {
            if (url.toString(false) == audioURL.toString(false))
            {
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate)
        {
            // Add the track to the playlist
            trackTitles.push_back(title.toStdString());
            trackURLs.push_back(audioURL);

            // Calculate track length
            std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
            if (reader != nullptr)
            {
                double lengthInSeconds = reader->lengthInSamples / reader->sampleRate;
                trackLengths.push_back(lengthInSeconds);
            }
            else
            {
                trackLengths.push_back(0.0);
            }

            tableComponent.updateContent();
        }
    }
}

void PlaylistComponent::removeFromPlaylist()
{
    int selectedRow = tableComponent.getSelectedRow();
    if (selectedRow >= 0 && selectedRow < trackTitles.size())
    {
        trackTitles.erase(trackTitles.begin() + selectedRow);
        trackURLs.erase(trackURLs.begin() + selectedRow);

        if (selectedRow < trackLengths.size())
        {
            trackLengths.erase(trackLengths.begin() + selectedRow);
        }

        tableComponent.updateContent();
    }
}

String PlaylistComponent::getTrackLength(URL audioURL)
{
    // Get the length of the audio file
    File file = audioURL.getLocalFile();
    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        double lengthInSeconds = reader->lengthInSamples / reader->sampleRate;
        int minutes = static_cast<int>(lengthInSeconds) / 60;
        int seconds = static_cast<int>(lengthInSeconds) % 60;

        return String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
    }

    return "0:00";
}