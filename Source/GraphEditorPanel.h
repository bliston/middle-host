/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef __GRAPHEDITORPANEL_JUCEHEADER__
#define __GRAPHEDITORPANEL_JUCEHEADER__

#include "FilterGraph.h"
#include "SlidingPanelComponent.h"
#include "BinaryData.h"
#include "AppColours.h"
#include "CustomMidiKeyboardComponent.h"
//#include "MiddleLookAndFeel.h"

class FilterComponent;
class ConnectorComponent;
class PinComponent;


//==============================================================================
/**
    A panel that displays and edits a FilterGraph.
*/
class GraphEditorPanel   : public Component,
                           public ChangeListener
{
public:
    GraphEditorPanel (FilterGraph& graph);
    ~GraphEditorPanel();

    void paint (Graphics& g);
    void mouseDown (const MouseEvent& e);

    void createNewPlugin (const PluginDescription* desc, int x, int y);

    FilterComponent* getComponentForFilter (uint32 filterID) const;
    ConnectorComponent* getComponentForConnection (const AudioProcessorGraph::Connection& conn) const;
    PinComponent* findPinAt (int x, int y) const;

    void resized();
    void changeListenerCallback (ChangeBroadcaster*);
    void updateComponents();

    //==============================================================================
    void beginConnectorDrag (uint32 sourceFilterID, int sourceFilterChannel,
                             uint32 destFilterID, int destFilterChannel,
                             const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endDraggingConnector (const MouseEvent& e);

    //==============================================================================
private:
    FilterGraph& graph;
    ScopedPointer<ConnectorComponent> draggingConnector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorPanel)
};

//==============================================================================
class KeyboardFocusTransferer : public MouseListener
{
public:
    KeyboardFocusTransferer()
    {
        
    }
    
    ~KeyboardFocusTransferer()
    {
        transferer->removeMouseListener(this);
        transferer = nullptr;
        receiver = nullptr;
    }
    
    
    void mouseDown(const MouseEvent &event) override
    {
        if (transferer->hasKeyboardFocus(true) && event.mods.isLeftButtonDown()) {
            receiver->grabKeyboardFocus();
        }
    }
    
    void setCallback(Component* _transferer, Component* _receiver) {
        transferer = _transferer;
        receiver = _receiver;
        transferer->addMouseListener(this, true);
    }
    
    
private:
    Component* transferer;
    Component* receiver;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyboardFocusTransferer)
};

//==============================================================================
/**
    A panel that embeds a GraphEditorPanel with a midi keyboard at the bottom.

    It also manages the graph itself, and plays it.
*/
class GraphDocumentComponent  : public Component
{
public:
    //==============================================================================
    GraphDocumentComponent (AudioPluginFormatManager& formatManager,
                            AudioDeviceManager* deviceManager);
    ~GraphDocumentComponent();

    //==============================================================================
    void createNewPlugin (const PluginDescription* desc, int x, int y);
    inline void setDoublePrecision (bool doublePrecision) { graphPlayer.setDoublePrecisionProcessing (doublePrecision); }
    //PluginEditor*  getPluginEditor (const String& name);

    //==============================================================================
    ScopedPointer<FilterGraph> graph;

    //==============================================================================
    void resized() override;

    //==============================================================================
    void unfocusKeyboardComponent();

    //==============================================================================
    void releaseGraph();
    
    //==============================================================================
    void paint (Graphics& g) override;
    
    void mouseDown(const MouseEvent &event) override;
    

private:
    //==============================================================================
    AudioDeviceManager* deviceManager;
    AudioProcessorPlayer graphPlayer;
    MidiKeyboardState keyState;

public:
    GraphEditorPanel* graphPanel;
    SlidingPanelComponent* panel;
    

private:
    Component* keyboardComp;
    Component* statusBar;
    KeyboardFocusTransferer* keyboardFocusTransferer;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphDocumentComponent)
};


//==============================================================================
/** A desktop window containing a plugin's UI. */
class PluginWindow  : public DocumentWindow
{
public:
    enum WindowFormatType
    {
        Normal = 0,
        Generic,
        Programs,
        Parameters,
        AudioIO,
        NumTypes
    };

    PluginWindow (Component* pluginEditor, AudioProcessorGraph::Node*, WindowFormatType, AudioProcessorGraph&);
    ~PluginWindow();

    static PluginWindow* getWindowFor (AudioProcessorGraph::Node*, WindowFormatType, AudioProcessorGraph&);

    static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
    static void closeAllCurrentlyOpenWindows();

    void moved() override;
    void closeButtonPressed() override;

private:
    AudioProcessorGraph& graph;
    AudioProcessorGraph::Node* owner;
    WindowFormatType type;

    float getDesktopScaleFactor() const override     { return 1.0f; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

//==============================================================================
/** A desktop window containing a plugin's UI. */
class PluginEditor  : public Component
{
public:
    enum WindowFormatType
    {
        Normal = 0,
        Generic,
        Programs,
        Parameters,
        AudioIO,
        NumTypes
    };
    
    PluginEditor (Component* pluginEditor, AudioProcessorGraph::Node*, WindowFormatType, AudioProcessorGraph&);
    ~PluginEditor();
    
    static PluginEditor* getPluginEditorFor (AudioProcessorGraph::Node*, WindowFormatType, AudioProcessorGraph&);
    
    static void closePluginEditorsFor (const uint32 nodeId);
    static void closeAllPluginEditors();
    void clearContentComponent();
    static void* deleteComponent (void* userData);
    static void closeProcessor (AudioProcessor* processor);
    void resized() override;
    
private:
    AudioProcessorGraph& graph;
    AudioProcessorGraph::Node* owner;
    WindowFormatType type;
    
    float getDesktopScaleFactor() const override     { return 1.0f; }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

inline String toString (PluginWindow::WindowFormatType type)
{
    switch (type)
    {
        case PluginWindow::Normal:     return "Normal";
        case PluginWindow::Generic:    return "Generic";
        case PluginWindow::Programs:   return "Programs";
        case PluginWindow::Parameters: return "Parameters";
        default:                       return String();
    }
}

inline String toString (PluginEditor::WindowFormatType type)
{
    switch (type)
    {
        case PluginEditor::Normal:     return "Normal";
        case PluginEditor::Generic:    return "Generic";
        case PluginEditor::Programs:   return "Programs";
        case PluginEditor::Parameters: return "Parameters";
        default:                       return String();
    }
}

inline String getLastXProp (PluginWindow::WindowFormatType type)    { return "uiLastX_" + toString (type); }
inline String getLastYProp (PluginWindow::WindowFormatType type)    { return "uiLastY_" + toString (type); }
inline String getOpenProp  (PluginWindow::WindowFormatType type)    { return "uiopen_"  + toString (type); }

#endif   // __GRAPHEDITORPANEL_JUCEHEADER__
