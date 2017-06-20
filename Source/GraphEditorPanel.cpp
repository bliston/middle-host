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

#include "../JuceLibraryCode/JuceHeader.h"
#include "GraphEditorPanel.h"
#include "InternalFilters.h"
#include "MainHostWindow.h"
#include "FilterIOConfiguration.h"

//==============================================================================
class PluginWindow;
static Array <PluginWindow*> activePluginWindows;

PluginWindow::PluginWindow (Component* const pluginEditor,
                            AudioProcessorGraph::Node* const o,
                            WindowFormatType t,
                            AudioProcessorGraph& audioGraph)
    : DocumentWindow (pluginEditor->getName(), Colours::lightblue,
                      DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      graph (audioGraph),
      owner (o),
      type (t)
{
    setSize (400, 300);

    setContentOwned (pluginEditor, true);

    setTopLeftPosition (owner->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));

    owner->properties.set (getOpenProp (type), true);

    setVisible (true);

    activePluginWindows.add (this);
}

void PluginWindow::closeCurrentlyOpenWindowsFor (const uint32 nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner->nodeId == nodeId)
            delete activePluginWindows.getUnchecked (i);
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    if (activePluginWindows.size() > 0)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            delete activePluginWindows.getUnchecked (i);

        Component dummyModalComp;
        dummyModalComp.enterModalState();
        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}

//==============================================================================
class PluginEditor;
static Array <PluginEditor*> activePluginEditors;

PluginEditor::PluginEditor (Component* const pluginEditor,
                            AudioProcessorGraph::Node* const o,
                            WindowFormatType t,
                            AudioProcessorGraph& audioGraph)
:
graph (audioGraph),
owner (o),
type (t)
{
    
    setSize (getHeight(), getWidth());
    
    addAndMakeVisible(pluginEditor);
    
    setVisible (true);
    
    activePluginEditors.add (this);
}

void PluginEditor::closePluginEditorsFor (const uint32 nodeId)
{
    for (int i = activePluginEditors.size(); --i >= 0;)
        if (activePluginEditors.getUnchecked(i)->owner->nodeId == nodeId)
        {
            auto processor = activePluginEditors.getUnchecked (i)->owner->getProcessor();
            closeProcessor(processor);
        }
}

void PluginEditor::closeAllPluginEditors()
{
    if (activePluginEditors.size() > 0)
    {
        for (int i = activePluginEditors.size(); --i >= 0;)
        {
            auto processor = activePluginEditors.getUnchecked (i)->owner->getProcessor();
            closeProcessor(processor);
        }
        
        Component dummyModalComp;
        dummyModalComp.enterModalState();
        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}

void PluginEditor::clearContentComponent()
{
    removeAllChildren();
    deleteAllChildren();

}

void* PluginEditor::deleteComponent (void* userData)
{
    Component* comp = static_cast<Component*>(userData);
    //Component* top = comp->getTopLevelComponent();
    //delete top;
    delete comp;
    return 0;
}
void PluginEditor::closeProcessor (AudioProcessor* processor)
{
    if (processor)
    {
        AudioProcessorEditor* editor = processor->getActiveEditor ();
        
        if (editor)
        {
            MessageManager::getInstance()->callFunctionOnMessageThread (
                                                                        deleteComponent, editor);
        }
        
        processor->releaseResources ();
    }
}

void PluginEditor::resized()
{
    setBounds(getLocalBounds());
}


//==============================================================================
PluginEditor* PluginEditor::getPluginEditorFor (AudioProcessorGraph::Node* const node,
                                          WindowFormatType type,
                                          AudioProcessorGraph& audioGraph)
{
    jassert (node != nullptr);
    
    for (int i = activePluginEditors.size(); --i >= 0;)
        if (activePluginEditors.getUnchecked(i)->owner == node
            && activePluginEditors.getUnchecked(i)->type == type)
            return activePluginEditors.getUnchecked(i);
    
    AudioProcessor* processor = node->getProcessor();
    AudioProcessorEditor* ui = nullptr;
    
    if (type == Normal)
    {
        ui = processor->createEditorIfNeeded();
        
        if (ui == nullptr)
            type = Generic;
    }
    
    if (ui == nullptr)
    {
        if (type == Generic || type == Parameters)
            ui = new GenericAudioProcessorEditor (processor);
        //else if (type == Programs)
            //ui = new ProgramAudioProcessorEditor (processor);
        else if (type == AudioIO)
            ui = new FilterIOConfigurationWindow (processor);
    }
    
    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast<AudioPluginInstance*> (processor))
            ui->setName (plugin->getName());
        
        return new PluginEditor (ui, node, type, audioGraph);
    }
    
    return nullptr;
}

PluginEditor::~PluginEditor()
{
    activePluginEditors.removeFirstMatchingValue (this);
    //closeAllPluginEditors();
    
    
}




//==============================================================================
class ProcessorProgramPropertyComp : public PropertyComponent,
                                     private AudioProcessorListener
{
public:
    ProcessorProgramPropertyComp (const String& name, AudioProcessor& p, int index_)
        : PropertyComponent (name),
          owner (p),
          index (index_)
    {
        owner.addListener (this);
    }

    ~ProcessorProgramPropertyComp()
    {
        owner.removeListener (this);
    }

    void refresh() { }
    void audioProcessorChanged (AudioProcessor*) { }
    void audioProcessorParameterChanged (AudioProcessor*, int, float) { }

private:
    AudioProcessor& owner;
    const int index;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorProgramPropertyComp)
};

class ProgramAudioProcessorEditor : public AudioProcessorEditor
{
public:
    ProgramAudioProcessorEditor (AudioProcessor* const p)
        : AudioProcessorEditor (p)
    {
        jassert (p != nullptr);
        setOpaque (true);

        addAndMakeVisible (panel);

        Array<PropertyComponent*> programs;

        const int numPrograms = p->getNumPrograms();
        int totalHeight = 0;

        for (int i = 0; i < numPrograms; ++i)
        {
            String name (p->getProgramName (i).trim());

            if (name.isEmpty())
                name = "Unnamed";

            ProcessorProgramPropertyComp* const pc = new ProcessorProgramPropertyComp (name, *p, i);
            programs.add (pc);
            totalHeight += pc->getPreferredHeight();
        }

        panel.addProperties (programs);

        setSize (400, jlimit (25, 400, totalHeight));
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::grey);
    }

    void resized() override
    {
        panel.setBounds (getLocalBounds());
    }

private:
    PropertyPanel panel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramAudioProcessorEditor)
};

//==============================================================================
PluginWindow* PluginWindow::getWindowFor (AudioProcessorGraph::Node* const node,
                                          WindowFormatType type,
                                          AudioProcessorGraph& audioGraph)
{
    jassert (node != nullptr);

    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node
             && activePluginWindows.getUnchecked(i)->type == type)
            return activePluginWindows.getUnchecked(i);

    AudioProcessor* processor = node->getProcessor();
    AudioProcessorEditor* ui = nullptr;

    if (type == Normal)
    {
        ui = processor->createEditorIfNeeded();

        if (ui == nullptr)
            type = Generic;
    }

    if (ui == nullptr)
    {
        if (type == Generic || type == Parameters)
            ui = new GenericAudioProcessorEditor (processor);
        else if (type == Programs)
            ui = new ProgramAudioProcessorEditor (processor);
        else if (type == AudioIO)
            ui = new FilterIOConfigurationWindow (processor);
    }

    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast<AudioPluginInstance*> (processor))
            ui->setName (plugin->getName());
		if (ui->getName() != "Recorder")
        return new PluginWindow (ui, node, type, audioGraph);
    }

    return nullptr;
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeFirstMatchingValue (this);
    clearContentComponent();
}

void PluginWindow::moved()
{
    owner->properties.set (getLastXProp (type), getX());
    owner->properties.set (getLastYProp (type), getY());
}

void PluginWindow::closeButtonPressed()
{
    owner->properties.set (getOpenProp (type), false);
    delete this;
}

//==============================================================================
class PinComponent   : public Component,
                       public SettableTooltipClient
{
public:
    PinComponent (FilterGraph& graph_,
                  const uint32 filterID_, const int index_, const bool isInput_)
        : filterID (filterID_),
          index (index_),
          isInput (isInput_),
          busIdx (0),
          graph (graph_)
    {
        if (const AudioProcessorGraph::Node::Ptr node = graph.getNodeForId (filterID_))
        {
            String tip;

            if (index == FilterGraph::midiChannelNumber)
            {
                tip = isInput ? "MIDI Input"
                              : "MIDI Output";
            }
            else
            {
                const AudioProcessor& processor = *node->getProcessor();

                int channel;
                channel = processor.getOffsetInBusBufferForAbsoluteChannelIndex (isInput, index, busIdx);

                if (const AudioProcessor::Bus* bus = processor.getBus (isInput, busIdx))
                    tip = bus->getName() + String (": ")
                          + AudioChannelSet::getAbbreviatedChannelTypeName (bus->getCurrentLayout().getTypeOfChannel (channel));
                else
                    tip = (isInput ? "Main Input: "
                           : "Main Output: ") + String (index + 1);

            }

            setTooltip (tip);
        }

        setSize (16, 16);
    }

    void paint (Graphics& g) override
    {
        const float w = (float) getWidth();
        const float h = (float) getHeight();

        Path p;
        p.addEllipse (w * 0.25f, h * 0.25f, w * 0.5f, h * 0.5f);

        p.addRectangle (w * 0.4f, isInput ? (0.5f * h) : 0.0f, w * 0.2f, h * 0.5f);

        Colour colour = (index == FilterGraph::midiChannelNumber ? findColour (mainAccentColourId) : findColour (mainBackgroundColourId).contrasting());

        g.setColour (colour.withRotatedHue (static_cast<float> (busIdx) / 5.0f));
        g.fillPath (p);
    }

    void mouseDown (const MouseEvent& e) override
    {
        getGraphPanel()->beginConnectorDrag (isInput ? 0 : filterID,
                                             index,
                                             isInput ? filterID : 0,
                                             index,
                                             e);
    }

    void mouseDrag (const MouseEvent& e) override
    {
        getGraphPanel()->dragConnector (e);
    }

    void mouseUp (const MouseEvent& e) override
    {
        getGraphPanel()->endDraggingConnector (e);
    }

    const uint32 filterID;
    const int index;
    const bool isInput;
    int busIdx;

private:
    FilterGraph& graph;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PinComponent)
};

//==============================================================================
class FilterComponent    : public Component
{
public:
    FilterComponent (FilterGraph& graph_,
                     const uint32 filterID_)
        : graph (graph_),
          filterID (filterID_),
          numInputs (0),
          numOutputs (0),
          pinSize (16),
          font (13.0f),
          numIns (0),
          numOuts (0)
    {
        shadow.setShadowProperties (DropShadow (Colours::lightgrey, 3, Point<int> (0, 1)));
        //setComponentEffect (&shadow);

        setSize (150, 60);
    }

    ~FilterComponent()
    {
        deleteAllChildren();
    }

    void mouseDown (const MouseEvent& e) override
    {
        originalPos = localPointToGlobal (Point<int>());

        toFront (true);

        if (e.mods.isPopupMenu())
        {
            PopupMenu m;
            m.addItem (1, "Delete this filter");
            m.addItem (2, "Disconnect all pins");
            m.addSeparator();
            m.addItem (3, "Show plugin UI");
            m.addItem (4, "Show all programs");
            m.addItem (5, "Show all parameters");
            m.addSeparator();
            m.addItem (6, "Configure Audio I/O");
            m.addItem (7, "Test state save/load");

            const int r = m.show();

            if (r == 1)
            {
                graph.removeFilter (filterID);
                return;
            }
            else if (r == 2)
            {
                graph.disconnectFilter (filterID);
            }
            else
            {
                if (AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID))
                {
                    AudioProcessor* const processor = f->getProcessor();
                    jassert (processor != nullptr);

                    if (r == 7)
                    {
                        MemoryBlock state;
                        processor->getStateInformation (state);
                        processor->setStateInformation (state.getData(), (int) state.getSize());
                    }
                    else
                    {
                        PluginWindow::WindowFormatType type = processor->hasEditor() ? PluginWindow::Normal
                                                                                     : PluginWindow::Generic;

                        switch (r)
                        {
                            case 4: type = PluginWindow::Programs; break;
                            case 5: type = PluginWindow::Parameters; break;
                            case 6: type = PluginWindow::AudioIO; break;

                            default: break;
                        };

                        if (PluginWindow* const w = PluginWindow::getWindowFor (f, type, graph.getGraph()))
                            w->toFront (true);
                    }
                }
            }
        }
    }

    void mouseDrag (const MouseEvent& e) override
    {
        if (! e.mods.isPopupMenu())
        {
            Point<int> pos (originalPos + Point<int> (e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY()));

            if (getParentComponent() != nullptr)
                pos = getParentComponent()->getLocalPoint (nullptr, pos);

            graph.setNodePosition (filterID,
                                   (pos.getX() + getWidth() / 2) / (double) getParentWidth(),
                                   (pos.getY() + getHeight() / 2) / (double) getParentHeight());

            getGraphPanel()->updateComponents();
        }
    }

    void mouseUp (const MouseEvent& e) override
    {
        if (e.mouseWasDraggedSinceMouseDown())
        {
            graph.setChangedFlag (true);
        }
        else if (e.getNumberOfClicks() == 2)
        {
            if (const AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID))
                if (PluginWindow* const w = PluginWindow::getWindowFor (f, PluginWindow::Normal, graph.getGraph()))
                    w->toFront (true);
        }
    }

    bool hitTest (int x, int y) override
    {
        for (int i = getNumChildComponents(); --i >= 0;)
            if (getChildComponent(i)->getBounds().contains (x, y))
                return true;

        return x >= 3 && x < getWidth() - 6 && y >= pinSize && y < getHeight() - pinSize;
    }

    void paint (Graphics& g) override
    {
        g.setColour (findColour (mainBackgroundColourId));

        const int x = 4;
        const int y = pinSize;
        const int w = getWidth() - x * 2;
        const int h = getHeight() - pinSize * 2;

        g.fillRoundedRectangle (x, y, w, h, 10);

        g.setColour (findColour (mainBackgroundColourId).contrasting());
        g.setFont (font);
        g.drawFittedText (getName(), getLocalBounds().reduced (4, 2), Justification::centred, 2);

        g.setColour (findColour (mainAccentColourId));
        g.drawRoundedRectangle (x, y, w, h, 10, 1.35f);
    }

    void resized() override
    {
        if (AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID))
        {
            if (AudioProcessor* const processor = f->getProcessor())
            {
                for (int i = 0; i < getNumChildComponents(); ++i)
                {
                    if (PinComponent* const pc = dynamic_cast<PinComponent*> (getChildComponent(i)))
                    {
                        const bool isInput = pc->isInput;
                        int busIdx, channelIdx;

                        channelIdx =
                            processor->getOffsetInBusBufferForAbsoluteChannelIndex (isInput, pc->index, busIdx);

                        const int total = isInput ? numIns : numOuts;
                        const int index = pc->index == FilterGraph::midiChannelNumber ? (total - 1) : pc->index;

                        const float totalSpaces = static_cast<float> (total) + (static_cast<float> (jmax (0, processor->getBusCount (isInput) - 1)) * 0.5f);
                        const float indexPos = static_cast<float> (index) + (static_cast<float> (busIdx) * 0.5f);

                        pc->setBounds (proportionOfWidth ((1.0f + indexPos) / (totalSpaces + 1.0f)) - pinSize / 2,
                                       pc->isInput ? 0 : (getHeight() - pinSize),
                                       pinSize, pinSize);
                    }
                }
            }
        }
    }

    void getPinPos (const int index, const bool isInput, float& x, float& y)
    {
        for (int i = 0; i < getNumChildComponents(); ++i)
        {
            if (PinComponent* const pc = dynamic_cast<PinComponent*> (getChildComponent(i)))
            {
                if (pc->index == index && isInput == pc->isInput)
                {
                    x = getX() + pc->getX() + pc->getWidth() * 0.5f;
                    y = getY() + pc->getY() + pc->getHeight() * 0.5f;
                    break;
                }
            }
        }
    }

    void update()
    {
        const AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (filterID));

        if (f == nullptr)
        {
            delete this;
            return;
        }

        numIns = f->getProcessor()->getTotalNumInputChannels();
        if (f->getProcessor()->acceptsMidi())
            ++numIns;

        numOuts = f->getProcessor()->getTotalNumOutputChannels();
        if (f->getProcessor()->producesMidi())
            ++numOuts;

        int w = 100;
        int h = 60;

        w = jmax (w, (jmax (numIns, numOuts) + 1) * 20);

        const int textWidth = font.getStringWidth (f->getProcessor()->getName());
        w = jmax (w, 16 + jmin (textWidth, 300));
        if (textWidth > 300)
            h = 100;

        setSize (w, h);

        setName (f->getProcessor()->getName());

        {
            Point<double> p = graph.getNodePosition (filterID);
            setCentreRelative ((float) p.x, (float) p.y);
        }

        if (numIns != numInputs || numOuts != numOutputs)
        {
            numInputs = numIns;
            numOutputs = numOuts;

            deleteAllChildren();

            int i;
            for (i = 0; i < f->getProcessor()->getTotalNumInputChannels(); ++i)
                addAndMakeVisible (new PinComponent (graph, filterID, i, true));

            if (f->getProcessor()->acceptsMidi())
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, true));

            for (i = 0; i < f->getProcessor()->getTotalNumOutputChannels(); ++i)
                addAndMakeVisible (new PinComponent (graph, filterID, i, false));

            if (f->getProcessor()->producesMidi())
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, false));

            resized();
        }
    }

    FilterGraph& graph;
    const uint32 filterID;
    int numInputs, numOutputs;

private:
    int pinSize;
    Point<int> originalPos;
    Font font;
    int numIns, numOuts;
    DropShadowEffect shadow;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }

    FilterComponent (const FilterComponent&);
    FilterComponent& operator= (const FilterComponent&);
};

//==============================================================================
class ConnectorComponent   : public Component,
                             public SettableTooltipClient
{
public:
    ConnectorComponent (FilterGraph& graph_)
        : sourceFilterID (0),
          destFilterID (0),
          sourceFilterChannel (0),
          destFilterChannel (0),
          graph (graph_),
          lastInputX (0),
          lastInputY (0),
          lastOutputX (0),
          lastOutputY (0)
    {
        setAlwaysOnTop (true);
    }

    void setInput (const uint32 sourceFilterID_, const int sourceFilterChannel_)
    {
        if (sourceFilterID != sourceFilterID_ || sourceFilterChannel != sourceFilterChannel_)
        {
            sourceFilterID = sourceFilterID_;
            sourceFilterChannel = sourceFilterChannel_;
            update();
        }
    }

    void setOutput (const uint32 destFilterID_, const int destFilterChannel_)
    {
        if (destFilterID != destFilterID_ || destFilterChannel != destFilterChannel_)
        {
            destFilterID = destFilterID_;
            destFilterChannel = destFilterChannel_;
            update();
        }
    }

    void dragStart (int x, int y)
    {
        lastInputX = (float) x;
        lastInputY = (float) y;
        resizeToFit();
    }

    void dragEnd (int x, int y)
    {
        lastOutputX = (float) x;
        lastOutputY = (float) y;
        resizeToFit();
    }

    void update()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        if (lastInputX != x1
             || lastInputY != y1
             || lastOutputX != x2
             || lastOutputY != y2)
        {
            resizeToFit();
        }
    }

    void resizeToFit()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        const Rectangle<int> newBounds ((int) jmin (x1, x2) - 4,
                                        (int) jmin (y1, y2) - 4,
                                        (int) std::abs (x1 - x2) + 8,
                                        (int) std::abs (y1 - y2) + 8);

        if (newBounds != getBounds())
            setBounds (newBounds);
        else
            resized();

        repaint();
    }

    void getPoints (float& x1, float& y1, float& x2, float& y2) const
    {
        x1 = lastInputX;
        y1 = lastInputY;
        x2 = lastOutputX;
        y2 = lastOutputY;

        if (GraphEditorPanel* const hostPanel = getGraphPanel())
        {
            if (FilterComponent* srcFilterComp = hostPanel->getComponentForFilter (sourceFilterID))
                srcFilterComp->getPinPos (sourceFilterChannel, false, x1, y1);

            if (FilterComponent* dstFilterComp = hostPanel->getComponentForFilter (destFilterID))
                dstFilterComp->getPinPos (destFilterChannel, true, x2, y2);
        }
    }

    void paint (Graphics& g) override
    {
        if (sourceFilterChannel == FilterGraph::midiChannelNumber
             || destFilterChannel == FilterGraph::midiChannelNumber)
        {
            g.setColour (findColour (mainAccentColourId).withAlpha(0.5f));
        }
        else
        {
            g.setColour (findColour (mainBackgroundColourId).contrasting().withAlpha(0.5f));
        }

        g.fillPath (linePath);
    }

    bool hitTest (int x, int y) override
    {
        if (hitPath.contains ((float) x, (float) y))
        {
            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (x, y, distanceFromStart, distanceFromEnd);

            // avoid clicking the connector when over a pin
            return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
        }

        return false;
    }

    void mouseDown (const MouseEvent&) override
    {
        dragging = false;
    }

    void mouseDrag (const MouseEvent& e) override
    {
        if (dragging)
        {
            getGraphPanel()->dragConnector (e);
        }
        else if (e.mouseWasDraggedSinceMouseDown())
        {
            dragging = true;

            graph.removeConnection (sourceFilterID, sourceFilterChannel, destFilterID, destFilterChannel);

            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (e.x, e.y, distanceFromStart, distanceFromEnd);
            const bool isNearerSource = (distanceFromStart < distanceFromEnd);

            getGraphPanel()->beginConnectorDrag (isNearerSource ? 0 : sourceFilterID,
                                                 sourceFilterChannel,
                                                 isNearerSource ? destFilterID : 0,
                                                 destFilterChannel,
                                                 e);
        }
    }

    void mouseUp (const MouseEvent& e) override
    {
        if (dragging)
            getGraphPanel()->endDraggingConnector (e);
    }

    void resized() override
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        lastInputX = x1;
        lastInputY = y1;
        lastOutputX = x2;
        lastOutputY = y2;

        x1 -= getX();
        y1 -= getY();
        x2 -= getX();
        y2 -= getY();

        linePath.clear();
        linePath.startNewSubPath (x1, y1);
        linePath.cubicTo (x1, y1 + (y2 - y1) * 0.33f,
                          x2, y1 + (y2 - y1) * 0.66f,
                          x2, y2);

        PathStrokeType wideStroke (8.0f);
        wideStroke.createStrokedPath (hitPath, linePath);

        PathStrokeType stroke (2.5f);
        stroke.createStrokedPath (linePath, linePath);

        const float arrowW = 5.0f;
        const float arrowL = 4.0f;

        Path arrow;
        arrow.addTriangle (-arrowL, arrowW,
                           -arrowL, -arrowW,
                           arrowL, 0.0f);

        arrow.applyTransform (AffineTransform()
                                .rotated (float_Pi * 0.5f - (float) atan2 (x2 - x1, y2 - y1))
                                .translated ((x1 + x2) * 0.5f,
                                             (y1 + y2) * 0.5f));

        linePath.addPath (arrow);
        linePath.setUsingNonZeroWinding (true);
    }

    uint32 sourceFilterID, destFilterID;
    int sourceFilterChannel, destFilterChannel;

private:
    FilterGraph& graph;
    float lastInputX, lastInputY, lastOutputX, lastOutputY;
    Path linePath, hitPath;
    bool dragging;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }

    void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        distanceFromStart = juce_hypot (x - (x1 - getX()), y - (y1 - getY()));
        distanceFromEnd = juce_hypot (x - (x2 - getX()), y - (y2 - getY()));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
};


//==============================================================================
GraphEditorPanel::GraphEditorPanel (FilterGraph& graph_)
    : graph (graph_)
{
    graph.addChangeListener (this);
    setOpaque (true);
}

GraphEditorPanel::~GraphEditorPanel()
{
    graph.removeChangeListener (this);
    draggingConnector = nullptr;
    deleteAllChildren();
}

void GraphEditorPanel::paint (Graphics& g)
{
    g.fillAll (findColour (mainBackgroundColourId));
}

void GraphEditorPanel::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;

        if (MainHostWindow* const mainWindow = findParentComponentOfClass<MainHostWindow>())
        {
            mainWindow->addPluginsToMenu (m);

            const int r = m.show();

            createNewPlugin (mainWindow->getChosenType (r), e.x, e.y);
        }
    }
}

void GraphEditorPanel::createNewPlugin (const PluginDescription* desc, int x, int y)
{
    graph.addFilter (desc, x / (double) getWidth(), y / (double) getHeight());
}

FilterComponent* GraphEditorPanel::getComponentForFilter (const uint32 filterID) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* const fc = dynamic_cast<FilterComponent*> (getChildComponent (i)))
            if (fc->filterID == filterID)
                return fc;
    }

    return nullptr;
}

ConnectorComponent* GraphEditorPanel::getComponentForConnection (const AudioProcessorGraph::Connection& conn) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (ConnectorComponent* const c = dynamic_cast<ConnectorComponent*> (getChildComponent (i)))
            if (c->sourceFilterID == conn.sourceNodeId
                 && c->destFilterID == conn.destNodeId
                 && c->sourceFilterChannel == conn.sourceChannelIndex
                 && c->destFilterChannel == conn.destChannelIndex)
                return c;
    }

    return nullptr;
}

PinComponent* GraphEditorPanel::findPinAt (const int x, const int y) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* fc = dynamic_cast<FilterComponent*> (getChildComponent (i)))
        {
            if (PinComponent* pin = dynamic_cast<PinComponent*> (fc->getComponentAt (x - fc->getX(),
                                                                                     y - fc->getY())))
                return pin;
        }
    }

    return nullptr;
}

void GraphEditorPanel::resized()
{
    updateComponents();
}

void GraphEditorPanel::changeListenerCallback (ChangeBroadcaster*)
{
    updateComponents();
}

void GraphEditorPanel::updateComponents()
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* const fc = dynamic_cast<FilterComponent*> (getChildComponent (i)))
            fc->update();
    }

    for (int i = getNumChildComponents(); --i >= 0;)
    {
        ConnectorComponent* const cc = dynamic_cast<ConnectorComponent*> (getChildComponent (i));

        if (cc != nullptr && cc != draggingConnector)
        {
            if (graph.getConnectionBetween (cc->sourceFilterID, cc->sourceFilterChannel,
                                            cc->destFilterID, cc->destFilterChannel) == nullptr)
            {
                delete cc;
            }
            else
            {
                cc->update();
            }
        }
    }

    for (int i = graph.getNumFilters(); --i >= 0;)
    {
        const AudioProcessorGraph::Node::Ptr f (graph.getNode (i));

        if (getComponentForFilter (f->nodeId) == 0)
        {
            FilterComponent* const comp = new FilterComponent (graph, f->nodeId);
            addAndMakeVisible (comp);
            comp->update();
        }
    }

    for (int i = graph.getNumConnections(); --i >= 0;)
    {
        const AudioProcessorGraph::Connection* const c = graph.getConnection (i);

        if (getComponentForConnection (*c) == 0)
        {
            ConnectorComponent* const comp = new ConnectorComponent (graph);
            addAndMakeVisible (comp);

            comp->setInput (c->sourceNodeId, c->sourceChannelIndex);
            comp->setOutput (c->destNodeId, c->destChannelIndex);
        }
    }
}

void GraphEditorPanel::beginConnectorDrag (const uint32 sourceFilterID, const int sourceFilterChannel,
                                           const uint32 destFilterID, const int destFilterChannel,
                                           const MouseEvent& e)
{
    draggingConnector = dynamic_cast<ConnectorComponent*> (e.originalComponent);

    if (draggingConnector == nullptr)
        draggingConnector = new ConnectorComponent (graph);

    draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
    draggingConnector->setOutput (destFilterID, destFilterChannel);

    addAndMakeVisible (draggingConnector);
    draggingConnector->toFront (false);

    dragConnector (e);
}

void GraphEditorPanel::dragConnector (const MouseEvent& e)
{
    const MouseEvent e2 (e.getEventRelativeTo (this));

    if (draggingConnector != nullptr)
    {
        draggingConnector->setTooltip (String());

        int x = e2.x;
        int y = e2.y;

        if (PinComponent* const pin = findPinAt (x, y))
        {
            uint32 srcFilter = draggingConnector->sourceFilterID;
            int srcChannel   = draggingConnector->sourceFilterChannel;
            uint32 dstFilter = draggingConnector->destFilterID;
            int dstChannel   = draggingConnector->destFilterChannel;

            if (srcFilter == 0 && ! pin->isInput)
            {
                srcFilter = pin->filterID;
                srcChannel = pin->index;
            }
            else if (dstFilter == 0 && pin->isInput)
            {
                dstFilter = pin->filterID;
                dstChannel = pin->index;
            }

            if (graph.canConnect (srcFilter, srcChannel, dstFilter, dstChannel))
            {
                x = pin->getParentComponent()->getX() + pin->getX() + pin->getWidth() / 2;
                y = pin->getParentComponent()->getY() + pin->getY() + pin->getHeight() / 2;

                draggingConnector->setTooltip (pin->getTooltip());
            }
        }

        if (draggingConnector->sourceFilterID == 0)
            draggingConnector->dragStart (x, y);
        else
            draggingConnector->dragEnd (x, y);
    }
}

void GraphEditorPanel::endDraggingConnector (const MouseEvent& e)
{
    if (draggingConnector == nullptr)
        return;

    draggingConnector->setTooltip (String());

    const MouseEvent e2 (e.getEventRelativeTo (this));

    uint32 srcFilter = draggingConnector->sourceFilterID;
    int srcChannel   = draggingConnector->sourceFilterChannel;
    uint32 dstFilter = draggingConnector->destFilterID;
    int dstChannel   = draggingConnector->destFilterChannel;

    draggingConnector = nullptr;

    if (PinComponent* const pin = findPinAt (e2.x, e2.y))
    {
        if (srcFilter == 0)
        {
            if (pin->isInput)
                return;

            srcFilter = pin->filterID;
            srcChannel = pin->index;
        }
        else
        {
            if (! pin->isInput)
                return;

            dstFilter = pin->filterID;
            dstChannel = pin->index;
        }

        graph.addConnection (srcFilter, srcChannel, dstFilter, dstChannel);
    }
}


//==============================================================================
class TooltipBar   : public Component,
                     private Timer
{
public:
    TooltipBar()
    {
        startTimer (100);
    }

    void paint (Graphics& g) override
    {
        g.setFont (Font (getHeight() * 0.7f));
        g.setColour (findColour (mainBackgroundColourId).contrasting().withAlpha(0.7f));
        g.drawFittedText (tip, 0, 0, getWidth(), getHeight(), Justification::centred, 1);
    }

    void timerCallback() override
    {
        Component* const underMouse = Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();
        TooltipClient* const ttc = dynamic_cast<TooltipClient*> (underMouse);

        String newTip;

        if (ttc != nullptr && ! (underMouse->isMouseButtonDown() || underMouse->isCurrentlyBlockedByAnotherModalComponent()))
            newTip = ttc->getTooltip();

        if (newTip != tip)
        {
            tip = newTip;
            repaint();
        }
    }

private:
    String tip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipBar)
};


        
/** Simple list box that just displays a StringArray. */
class FileListBoxModel   : public ListBoxModel
{
public:
    FileListBoxModel (const Array<String>& list, FilterGraph& graph_)
    : midiMessageList (list), graph(graph_)
    {
    }
    
    int getNumRows() override    { return midiMessageList.size(); }
    
    void paintListBoxItem (int row, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (isPositiveAndBelow (row, midiMessageList.size()))
        {
            const String& message = midiMessageList.getReference (row);
            
            if (rowIsSelected) {
                //if (row == midiMessageList.indexOf(graph.getLastDocumentOpened().getFileNameWithoutExtension()))
                g.fillAll (Colours::skyblue.withAlpha (0.3f));
            }

            g.setColour (Colours::black);
            
            g.drawText (message,
                        Rectangle<int> (width, height).reduced (4, 0),
                        Justification::centredLeft, true);
        }
    }
    
    void listBoxItemClicked (int row, const MouseEvent &) override
    {
        if (isPositiveAndBelow (row, midiMessageList.size()))
        {
            String documentsFolder = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
            String folder = documentsFolder + "/Middle/Projects/";
            const String& message = midiMessageList.getReference (row);
            graph.loadFrom(File(folder + message + ".middlet"), true);
        }
        
    }
    
    
    
private:
    const Array<String>& midiMessageList;
    FilterGraph& graph;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileListBoxModel)
};
        
        

//==============================================================================
class FileListBoxComponent  : public Component,
private AsyncUpdater
{
public:
    FileListBoxComponent (FilterGraph& graph_)
    : midiLogListBoxModel (midiMessageList, graph_), graph(graph_)
    {
        setOpaque (false);
        addAndMakeVisible(titleLabel);
        titleLabel.setText("Template Projects", dontSendNotification);
        titleLabel.setColour(Label::textColourId, findColour(mainAccentColourId));
        titleLabel.setJustificationType(Justification::centred);
        addAndMakeVisible (messageListBox);
        messageListBox.setModel (&midiLogListBoxModel);
        messageListBox.setMultipleSelectionEnabled(false);
        messageListBox.setRowHeight(25);
        messageListBox.setColour(ListBox::backgroundColourId, findColour(lightAccentColourId));
        fillList();
        
    }
    
    ~FileListBoxComponent()
    {

    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::transparentWhite);
        
        ScopedPointer<Drawable> logoBackground;
        // svg for thumbnail background highlight
        ScopedPointer<XmlElement> backSvg (XmlDocument::parse (BinaryData::middle_therapeutic_logo_svg));
        jassert (backSvg != nullptr);
        
//        logoBackground = Drawable::createFromSVG (*backSvg);
//        logoBackground->replaceColour(Colours::white, findColour(mainAccentColourId));
//        logoBackground->drawWithin (g, getLocalBounds().removeFromBottom (250).reduced (80).toFloat() , RectanglePlacement::centred, 1.0f);
    }
    
    void resized() override
    {
        Rectangle<int> area (getLocalBounds());
        titleLabel.setBounds(area.removeFromTop(20));
        messageListBox.setBounds (area.reduced (8));
    }
    
    void fillList ()
    {
        clearMessagesFromList();
        String documentsFolder = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
        String folder = documentsFolder + "/Middle/Projects";
        DirectoryIterator iter (File (folder), false, "*.middlet");
        
        while (iter.next())
        {
            File theFileItFound (iter.getFile());
			if (!theFileItFound.hasWriteAccess())
            addMessageToList (theFileItFound.getFileNameWithoutExtension());
        }
        
        selectOpenedListItem();
    }
    
    void selectOpenedListItem() {
        int indexOfDocumentTitle;
        indexOfDocumentTitle = midiMessageList.indexOf(graph.getLastDocumentOpened().getFileNameWithoutExtension());
        messageListBox.selectRow(indexOfDocumentTitle);
		messageListBox.repaint();
    }
    
    void addMessageToList (const String& message)
    {
        midiMessageList.add (message);
        triggerAsyncUpdate();
    }
    
    void clearMessagesFromList ()
    {
        midiMessageList.clear();
        triggerAsyncUpdate();
    }
    
    void handleAsyncUpdate() override
    {
        //fillList();
        messageListBox.updateContent();
        messageListBox.repaint();
    }
    

private:
    ListBox messageListBox;
    Array<String> midiMessageList;
    FileListBoxModel midiLogListBoxModel;
    FilterGraph& graph;
    int iterNumFiles;
    Label titleLabel;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileListBoxComponent)
};



//==============================================================================
class PresetBrowser   : public Component,
public DragAndDropContainer,
private Button::Listener,
private FileBrowserListener
{
public:
    PresetBrowser(FilterGraph* graph_)
    : moviesWildcardFilter ("*.middlet", "*.middlet", "Movies File Filter"),
    directoryThread ("Movie File Scanner Thread"),
    movieList (&moviesWildcardFilter, directoryThread),
    fileTree (movieList),
    resizerBar (&stretchableManager, 1, false),
    graph(graph_)
    {
        setOpaque (false);
        String documentsFolder = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
        String folder = documentsFolder + "/Middle/Projects";
        movieList.setDirectory (File(folder), true, true);
        directoryThread.startThread (1);
        
        fileTree.addListener (this);
        fileTree.setColour (FileTreeComponent::backgroundColourId, Colours::whitesmoke.withAlpha (0.2f));
        fileTree.setColour (FileTreeComponent::highlightColourId, Colours::lightskyblue);
        addAndMakeVisible (fileTree);
        
        addAndMakeVisible (resizerBar);
        
        // we have to set up our StretchableLayoutManager so it know the limits and preferred sizes of it's contents
        stretchableManager.setItemLayout (0,            // for the fileTree
                                          -0.1, -0.9,   // must be between 50 pixels and 90% of the available space
                                          -1.0);        // and its preferred size is 30% of the total available space
//        
//        stretchableManager.setItemLayout (1,            // for the resize bar
//                                          5, 5, 5);     // hard limit to 5 pixels
//        
//        stretchableManager.setItemLayout (2,            // for the movie components
//                                          -0.1, -0.9,   // size must be between 50 pixels and 90% of the available space
//                                          -0.7);        // and its preferred size is 70% of the total available space
    }
    
    ~PresetBrowser()
    {
        fileTree.removeListener (this);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::transparentWhite);
    }
    
    void resized() override
    {
        // make a list of two of our child components that we want to reposition
        Component* comps[] = { &fileTree, &resizerBar, nullptr };
        
        // this will position the 3 components, one above the other, to fit
        // vertically into the rectangle provided.
        stretchableManager.layOutComponents (comps, 3,
                                             0, 0, getWidth(), getHeight(),
                                             true, true);
        
        // now position out two video components in the space that's left
        //Rectangle<int> area (getLocalBounds().removeFromBottom (getHeight() - resizerBar.getBottom()));
        
        {

        }

    }
    
private:
    WildcardFileFilter moviesWildcardFilter;
    TimeSliceThread directoryThread;
    DirectoryContentsList movieList;
    FileTreeComponent fileTree;
    StretchableLayoutManager stretchableManager;
    StretchableLayoutResizerBar resizerBar;
    FilterGraph* graph;
    
    void refresh() {
        movieList.refresh();
        fileTree.refresh();
    }
    
    void buttonClicked (Button* button) override
    {

    }
    
    void selectionChanged() override
    {
        
        if (graph != nullptr && graph->saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk) {
            graph->setLastDocumentOpened(fileTree.getSelectedFile().getFullPathName());
            graph->loadFrom(fileTree.getSelectedFile().getFullPathName(), false);
        }
        
    }
    
    
    void fileClicked (const File&, const MouseEvent&) override {}
    void fileDoubleClicked (const File&) override {}
    void browserRootChanged (const File&) override {}
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowser)
};



//==============================================================================
/**
 Template option tile button.
 The drawable button object class for the tile icons and buttons in the TemplateTileBrowser
 */
class TemplateOptionButton   : public DrawableButton
{
public:
    TemplateOptionButton (const String& buttonName, ButtonStyle buttonStyle, const char* thumbSvg)
    : DrawableButton (buttonName, buttonStyle)
    {
        // svg for thumbnail icon
        ScopedPointer<XmlElement> svg (XmlDocument::parse (thumbSvg));
        jassert (svg != nullptr);
        
        thumb = Drawable::createFromSVG (*svg);
        thumb->replaceColour(Colour (0xff448AFF), findColour(mainAccentColourId));
        
        // svg for thumbnail background highlight
        ScopedPointer<XmlElement> backSvg (XmlDocument::parse (BinaryData::middle_highlight_svg));
        jassert (backSvg != nullptr);
        
        hoverBackground = Drawable::createFromSVG (*backSvg);
        hoverBackground->replaceColour(Colour (0xffDDDDDD), findColour(mainAccentColourId).withAlpha(0.3f));
        
        name = buttonName;
        
        description = "<insert description>";
    }
    
    void paintButton (Graphics& g, bool isMouseOverButton, bool /*isButtonDown*/) override
    {
        const Rectangle<float> r (getLocalBounds().toFloat());

        if (isMouseOverButton)
        {
            if (getStyle() == ImageFitted)
            {
                hoverBackground->drawWithin (g, r , RectanglePlacement::centred, 1.0);
                thumb->drawWithin (g, r , RectanglePlacement::centred, 1.0);
            }
            else if (getStyle() == ImageAboveTextLabel)
            {
                g.setColour (findColour (mainAccentColourId).withAlpha (0.3f));
                if (isConnectedOnLeft() || isConnectedOnRight()) {
                    g.fillRoundedRectangle (r, 10.0f);
                    g.setColour (findColour (mainAccentColourId));
                    g.drawRoundedRectangle (r, 10.0f, 1.35f);
                }
                else {
                    g.fillRect (r);
                    g.setColour (findColour (mainAccentColourId));
                    g.drawRect (r);
                }
                //hoverBackground->drawWithin (g, r , RectanglePlacement::centred, 1.0);
                thumb->drawWithin (g, getLocalBounds().toFloat() , RectanglePlacement::centred, 1.0f);
            }
            else
            {
                g.setColour (findColour (mainAccentColourId).withAlpha (0.3f));
                if (isConnectedOnLeft() || isConnectedOnRight()) {
                    g.setColour (findColour (mainAccentColourId));
                    g.drawRoundedRectangle (r, 10.0f, 1.35f);
                }
                else {
                    g.setColour (findColour (mainAccentColourId));
                    g.drawRect (r);
                }
            }
        }
        else
        {
            if (getStyle() == ImageFitted)
            {
                thumb->drawWithin (g, r, RectanglePlacement::centred, 1.0);
            }
            else if (getStyle() == ImageAboveTextLabel)
            {
                thumb->drawWithin (g, getLocalBounds().toFloat() , RectanglePlacement::centred, 1.0f);
                //thumb->drawWithin (g, getLocalBounds().removeFromTop (100).reduced (10).toFloat() , RectanglePlacement::centred, 1.0f);
                g.setColour (findColour (mainAccentColourId));
                if (isConnectedOnLeft() || isConnectedOnRight()) {
                    g.drawRoundedRectangle (r, 10.0f, 1.35f);
                }
                else {
                    g.drawRect (r);
                }
            }
            else
            {
                g.setColour (findColour (mainAccentColourId));
                if (isConnectedOnLeft() || isConnectedOnRight()) {
                    g.drawRoundedRectangle (r, 10.0f, 1.35f);
                }
                else {
                    g.drawRect (r);
                }
            }
        }
        
        Rectangle<float> textTarget;
        g.setColour (findColour (mainBackgroundColourId).contrasting());
        
        // center the text for the text buttons or position the text in the image buttons
        if (getStyle() != ImageFitted)
        {
            textTarget = getLocalBounds().toFloat();
        }
        else if (getStyle() == ImageAboveTextLabel)
        {
            textTarget = RectanglePlacement (RectanglePlacement::centred).appliedTo (thumb->getDrawableBounds(), r);
            textTarget = textTarget.removeFromBottom (textTarget.getHeight() * 0.3f);
        }
        else
        {
            textTarget = RectanglePlacement (RectanglePlacement::centred).appliedTo (thumb->getDrawableBounds(), r);
            textTarget = textTarget.removeFromBottom (textTarget.getHeight() * 0.3f);
        }
        
        const int textH = (getStyle() == ImageAboveTextLabel)
        ? jmin (16, getLocalBounds().proportionOfHeight (0.25f))
        : 0;
        
        if (textH > 0)
        {
            g.setFont ((float) textH);
            
            g.setColour (findColour (getToggleState() ? DrawableButton::textColourOnId
                                     : DrawableButton::textColourId)
                         .withMultipliedAlpha (isEnabled() ? 1.0f : 0.4f));
            
            g.drawFittedText (name,
                              2, getLocalBounds().getHeight() - textH - getLocalBounds().proportionOfHeight (0.25f),
                              getLocalBounds().getWidth() - 4, textH,
                              Justification::centred, 1);
        }
        else
        {
            g.drawText (name, textTarget, Justification::centred, true);
        }
        
        
        
        
    }
    
    void resized() override
    {
        thumb->setBoundsToFit (0, 0, getWidth(), getHeight(), Justification::centred, false);
    }
    
    void setDescription (String descript) noexcept
    {
        description = descript;
    }
    
    String getDescription() const noexcept
    {
        return description;
    }
    
private:
    ScopedPointer<Drawable> thumb, hoverBackground;
    String name, description;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemplateOptionButton)
};

        
        

//==============================================================================
/**
 Project Template Component for front page.
 Features multiple icon buttons to select the type of project template
 */
class TemplateTileBrowser   : public Component,
private ChangeListener,
private ButtonListener

{
public:
    TemplateTileBrowser (FilterGraph& graph_)
    : graph(graph_)
    {
        presetBrowser = new FileListBoxComponent (graph);
        addAndMakeVisible(presetBrowser);
        graph.addChangeListener(this);

    }
    
    ~TemplateTileBrowser()
    {
        graph.removeChangeListener (this);
        pluginEditor = nullptr;
        deleteAllChildren();
    }
    
    void paint (Graphics& g) override
    {
        g.setColour (findColour (mainAccentColourId));
        g.setFont (20.0f);
		if (projectHasPlugin("Recorder")) {
			g.drawText("Loading...", 0, 0, getWidth(), 200, Justification::centred, true);
		}
		else {
			g.drawText("No Recorder", 0, 0, getWidth(), 200, Justification::centred, true);
		}
        
        
    }

	bool projectHasPlugin(const String& pluginName) {
		bool hasPlugin = false;
		for (int i = graph.getNumFilters(); --i >= 0;)
		{
			const AudioProcessorGraph::Node::Ptr f(graph.getNode(i));
			if (f->getProcessor()->getName() == pluginName) {
					hasPlugin = true;
			}

		}
		return hasPlugin;
	}
    
    void resized() override
    {
        Rectangle<int> allOpts = getLocalBounds();
        allOpts.removeFromTop (200);
        presetBrowser->setBounds(allOpts);
        
    }
    
    void showPlugin (const String& name)
    {
        for (int i = graph.getNumFilters(); --i >= 0;)
        {
            const AudioProcessorGraph::Node::Ptr f(graph.getNode(i));
            if (f->getProcessor()->getName() == name) {
                if (PluginWindow* const w = PluginWindow::getWindowFor(f, PluginWindow::Normal, graph.getGraph()))
                    w->toFront(true);
            }
            
        }
    }
    
    PluginEditor* getPluginEditor (const String& name)
    {
        for (int i = graph.getNumFilters(); --i >= 0;)
        {
            const AudioProcessorGraph::Node::Ptr f(graph.getNode(i));
            if (f->getProcessor()->getName() == name)
                if (PluginEditor* const w = PluginEditor::getPluginEditorFor(f, PluginEditor::Normal, graph.getGraph()))
                    return w;
        }
        return nullptr;
    }
    
    
    
private:
    OwnedArray<TemplateOptionButton> optionButtons;
    //ScopedPointer<TemplateOptionButton> audioSettingsButton, openProjectButton, saveProjectButton;
    ApplicationCommandManager commandManager;
    ScopedPointer<MainHostWindow> mainWindow;
    FilterGraph& graph;
    ScopedPointer<PluginEditor> pluginEditor;
    FileListBoxComponent* presetBrowser;
    
    void changeListenerCallback	(ChangeBroadcaster* source) override
    {
		repaint();
        pluginEditor = getPluginEditor("Recorder");
        addAndMakeVisible(pluginEditor);
        
    }
    
    void buttonClicked (Button* button) override
    {

        
    }

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemplateTileBrowser)
};
        

     
//==============================================================================
GraphDocumentComponent::GraphDocumentComponent (AudioPluginFormatManager& formatManager,
                                                AudioDeviceManager* deviceManager_)
    : graph (new FilterGraph (formatManager)), deviceManager (deviceManager_),
      graphPlayer (getAppProperties().getUserSettings()->getBoolValue ("doublePrecisionProcessing", false))
{
    graphPanel = new GraphEditorPanel (*graph);
    addAndMakeVisible(panel = new SlidingPanelComponent());
    TemplateTileBrowser* menu;
    panel->addTab ("Menu", menu = new TemplateTileBrowser (*graph), true);
    panel->addTab ("Graph", graphPanel, true);
    
    deviceManager->addChangeListener (graphPanel);

    graphPlayer.setProcessor (&graph->getGraph());

    keyState.addListener (&graphPlayer.getMidiMessageCollector());

    addAndMakeVisible (keyboardComp = new CustomMidiKeyboardComponent (keyState, CustomMidiKeyboardComponent::Orientation::horizontalKeyboard));
    
    addAndMakeVisible (statusBar = new TooltipBar());

    deviceManager->addAudioCallback (&graphPlayer);
    deviceManager->addMidiInputCallback (String(), &graphPlayer.getMidiMessageCollector());

    graphPanel->updateComponents();
    
	keyboardFocusTransferer = new KeyboardFocusTransferer();
	keyboardFocusTransferer->setCallback(this, keyboardComp);
    
    addMouseListener(this, true);
}

GraphDocumentComponent::~GraphDocumentComponent()
{
    releaseGraph();
    keyState.removeListener (&graphPlayer.getMidiMessageCollector());
    keyboardFocusTransferer->~KeyboardFocusTransferer();

}

void GraphDocumentComponent::resized()
{
    const int keysHeight = 60;
    const int statusHeight = 20;

    panel->setBounds (0, 0, getWidth(), getHeight() - keysHeight);
    statusBar->setBounds (0, getHeight() - keysHeight - statusHeight, getWidth(), statusHeight);
    keyboardComp->setBounds (0, getHeight() - keysHeight, getWidth(), keysHeight);
    //transComp->setBounds (0, getHeight() - keysHeight, getWidth(), keysHeight);
}

void GraphDocumentComponent::createNewPlugin (const PluginDescription* desc, int x, int y)
{
    graphPanel->createNewPlugin (desc, x, y);
}

void GraphDocumentComponent::unfocusKeyboardComponent()
{
    keyboardComp->unfocusAllComponents();
}

void GraphDocumentComponent::releaseGraph()
{
    deviceManager->removeAudioCallback (&graphPlayer);
    deviceManager->removeMidiInputCallback (String(), &graphPlayer.getMidiMessageCollector());
    deviceManager->removeChangeListener (graphPanel);

    deleteAllChildren();

    graphPlayer.setProcessor (nullptr);
    graph = nullptr;
}

void GraphDocumentComponent::paint (Graphics& g)
{
    g.fillAll(findColour (mainBackgroundColourId));
}
        
void GraphDocumentComponent::mouseDown(const MouseEvent &event)
{
    DBG(event.originalComponent->getName());
}
        
