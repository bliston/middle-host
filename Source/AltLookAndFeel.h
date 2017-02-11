
#ifndef __ALTLOOKANDFEEL_JUCEHEADER__
#define __ALTLOOKANDFEEL_JUCEHEADER__

#include "../JuceLibraryCode/JuceHeader.h"

namespace LookAndFeelHelpers
{
    static Colour createBaseColour (Colour buttonColour,
                                    bool hasKeyboardFocus,
                                    bool isMouseOverButton,
                                    bool isButtonDown) noexcept
    {
        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
        const Colour baseColour (buttonColour.withMultipliedSaturation (sat));
        
        if (isButtonDown)      return baseColour.contrasting (0.2f);
            if (isMouseOverButton) return baseColour.contrasting (0.1f);
                
                return baseColour;
    }
}
    
class AltLookAndFeel : public LookAndFeel_V3
{
public:
    AltLookAndFeel()
    {
        
        accentColour = Colour (0xff9c27b0).withAlpha(1.0f);
        LookAndFeel::setDefaultSansSerifTypefaceName("Quicksand");
        setColour(TextButton::textColourOnId, accentColour);
        setColour(TextButton::buttonColourId, Colours::white);
        setColour (AlertWindow::backgroundColourId, Colour (0xffffffff));
        setColour (AlertWindow::textColourId, Colour (0xff000000));
        setColour (AlertWindow::outlineColourId, Colour (0xffffffff));
        setColour(TextEditor::focusedOutlineColourId, accentColour);
        setColour(TextEditor::highlightedTextColourId, accentColour);
        setColour(TextEditor::textColourId, accentColour);
        setColour(TabbedButtonBar::tabOutlineColourId, accentColour);
        setColour(TabbedButtonBar::frontTextColourId, Colours::white);
        setColour(TabbedButtonBar::tabTextColourId, Colours::lightgrey);
        setColour(TabbedComponent::backgroundColourId, accentColour);
        setColour(TabbedComponent::outlineColourId, Colours::white);
        setColour(DocumentWindow::textColourId, Colours::white);
        

    }
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        const float radius = jmin (width / 2, height / 2) - 4.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // fill
        g.setColour (Colours::orange);
        g.fillEllipse (rx, ry, rw, rw);
        
        // outline
        g.setColour (Colours::red);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);
        
        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer
        g.setColour (Colours::yellow);
        g.fillPath (p);
    }
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override
    {
        Rectangle<int> buttonArea = button.getLocalBounds();
        
        const float mouseOver = isMouseOverButton ? 0.8f: 1.0f;
        
        g.setColour (backgroundColour.withAlpha(mouseOver));
        g.fillRect (buttonArea);
    }
    
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        const float mouseOver = isMouseOverButton ? 0.5f: 1.0f;
        Colour textColour;
        textColour = button.findColour(TextButton::textColourOnId).withAlpha(mouseOver);
        String name;
        name = button.getButtonText();
        AttributedString a;
        a.setJustification (Justification::centred);
        
        String category;
        
        if (name.containsChar (':'))
        {
            category = name.upToFirstOccurrenceOf (":", false, false);
            name = name.fromFirstOccurrenceOf (":", false, false).trim();
            
            
            if (button.getHeight() > 20)
                category << "\n";
                else
                category << " ";
                }
                
                if (category.isNotEmpty()) {
                    Font font1;
                    font1.setTypefaceName("font-middle");
                    font1.setHeight(30.0f);
                    a.append (category, font1, textColour);
                }
                Font font1;
                font1.setTypefaceName("Quicksand");
                font1.setHeight(14.0f);
                a.append (name, font1, textColour);
                
                
                
                

        Font font (getTextButtonFont (button, button.getHeight()));
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                        : TextButton::textColourOffId)
                     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        
        const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;
        
        const int fontHeight = roundToInt (font.getHeight() * 0.6f);
        const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;
        
        const int edge = 4;
        const int offset = isButtonDown ? edge / 2 : 0;
        
        if (textWidth > 0)
            //g.drawFittedText (button.getButtonText(),
                              //leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
                              //Justification::centred, 2);
            a.draw (g, Rectangle<int> (button.getWidth(), button.getHeight()).toFloat());
            }
                
    void drawTickBox (Graphics& g, Component& /*component*/,
                                                  float x, float y, float w, float h,
                                                  const bool ticked,
                                                  const bool isEnabled,
                                                  const bool /*isMouseOverButton*/,
                                                  const bool isButtonDown) override
            {
                Path box;
                box.addRoundedRectangle (0.0f, 2.0f, 6.0f, 6.0f, 0.0f);
                
                g.setColour (isEnabled ? accentColour.withAlpha (isButtonDown ? 0.3f : 0.1f)
                             : Colours::lightgrey.withAlpha (0.1f));
                
                AffineTransform trans (AffineTransform::scale (w / 9.0f, h / 9.0f).translated (x, y));
                
                g.fillPath (box, trans);
                
                g.setColour (Colours::black.withAlpha (0.6f));
                g.strokePath (box, PathStrokeType (0.9f), trans);
                
                if (ticked)
                {
                    Path tick;
                    tick.startNewSubPath (1.5f, 3.0f);
                    tick.lineTo (3.0f, 6.0f);
                    tick.lineTo (6.0f, 0.0f);
                    
                    g.setColour (isEnabled ? Colours::black : Colours::grey);
                    g.strokePath (tick, PathStrokeType (2.5f), trans);
                }
            }
 
    void drawDocumentWindowTitleBar (DocumentWindow& window, Graphics& g,
                                                        int w, int h, int titleSpaceX, int titleSpaceW,
                                                        const Image* icon, bool drawTitleTextOnLeft) override
            {
                if (w * h == 0)
                    return;
                
                const bool isActive = window.isActiveWindow();
                
                //g.setGradientFill (ColourGradient (window.getBackgroundColour(),
                                                   //0.0f, 0.0f,
                                                   //window.getBackgroundColour().contrasting (isActive ? 0.15f : 0.05f),
                                                   //0.0f, (float) h, false));
                g.setColour (accentColour);
                g.fillAll();
                
                Font font (h * 0.65f, Font::plain);
                g.setFont (font);
                
                int textW = font.getStringWidth (window.getName());
                int iconW = 0;
                int iconH = 0;
                
                if (icon != nullptr)
                {
                    iconH = (int) font.getHeight();
                    iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
                }
                
                textW = jmin (titleSpaceW, textW + iconW);
                int textX = drawTitleTextOnLeft ? titleSpaceX
                : jmax (titleSpaceX, (w - textW) / 2);
                
                if (textX + textW > titleSpaceX + titleSpaceW)
                    textX = titleSpaceX + titleSpaceW - textW;
                
                if (icon != nullptr)
                {
                    g.setOpacity (isActive ? 1.0f : 0.6f);
                    g.drawImageWithin (*icon, textX, (h - iconH) / 2, iconW, iconH,
                                       RectanglePlacement::centred, false);
                    textX += iconW;
                    textW -= iconW;
                }
                
                if (window.isColourSpecified (DocumentWindow::textColourId) || isColourSpecified (DocumentWindow::textColourId))
                    g.setColour (window.findColour (DocumentWindow::textColourId));
                else
                    g.setColour (window.getBackgroundColour().contrasting (isActive ? 0.7f : 0.4f));
                
                g.drawText (window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
            }
                
    Button* createDocumentWindowButton (int buttonType) override
            {
                Path shape;
                
                if (buttonType == DocumentWindow::closeButton)
                {
                    shape.addLineSegment (Line<float> (0.0f, 0.0f, 1.0f, 1.0f), 0.1f);
                    shape.addLineSegment (Line<float> (1.0f, 0.0f, 0.0f, 1.0f), 0.1f);
                    
                    ShapeButton* const b = new ShapeButton ("close",
                                                            Colour (0xffffffff),
                                                            Colour (0xffffffff),
                                                            Colour (0xffffffff));
                    
                    b->setShape (shape, true, true, true);
                    return b;
                }
                else if (buttonType == DocumentWindow::minimiseButton)
                {
                    shape.addLineSegment (Line<float> (0.0f, 0.5f, 1.0f, 0.5f), 0.1f);
                    
                    DrawableButton* b = new DrawableButton ("minimise", DrawableButton::ImageFitted);
                    DrawablePath dp;
                    dp.setPath (shape);
                    dp.setFill (Colours::white);
                    b->setImages (&dp);
                    return b;
                }
                else if (buttonType == DocumentWindow::maximiseButton)
                {
                    shape.addLineSegment (Line<float> (0.5f, 0.0f, 0.5f, 1.0f), 0.1f);
                    shape.addLineSegment (Line<float> (0.0f, 0.5f, 1.0f, 0.5f), 0.1f);
                    
                    DrawableButton* b = new DrawableButton ("maximise", DrawableButton::ImageFitted);
                    DrawablePath dp;
                    dp.setPath (shape);
                    dp.setFill (Colours::white);
                    b->setImages (&dp);
                    return b;
                }
                
                jassertfalse;
                return nullptr;
            }
    void drawAlertBox (Graphics& g, AlertWindow& alert,
                                                   const Rectangle<int>& textArea, TextLayout& textLayout) override
            {
                g.fillAll (alert.findColour (AlertWindow::backgroundColourId));
                
                int iconSpaceUsed = 0;
                
                const int iconWidth = 80;
                int iconSize = jmin (iconWidth + 50, alert.getHeight() + 20);
                
                if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
                    iconSize = jmin (iconSize, textArea.getHeight() + 50);
                
                const Rectangle<int> iconRect (iconSize / -10, iconSize / -10,
                                               iconSize, iconSize);
                
                if (alert.getAlertType() != AlertWindow::NoIcon)
                {
                    Path icon;
                    uint32 colour;
                    char character;
                    
                    if (alert.getAlertType() == AlertWindow::WarningIcon)
                    {
                        colour = 0x55ff5555;
                        character = '!';
                        
                        icon.addTriangle (iconRect.getX() + iconRect.getWidth() * 0.5f, (float) iconRect.getY(),
                                          (float) iconRect.getRight(), (float) iconRect.getBottom(),
                                          (float) iconRect.getX(), (float) iconRect.getBottom());
                        
                        icon = icon.createPathWithRoundedCorners (5.0f);
                    }
                    else
                    {
                        colour    = alert.getAlertType() == AlertWindow::InfoIcon ? (uint32) 0x605555ff : (uint32) 0x40b69900;
                        character = alert.getAlertType() == AlertWindow::InfoIcon ? 'i' : '?';
                        
                        icon.addEllipse (iconRect.toFloat());
                    }
                    
                    GlyphArrangement ga;
                    ga.addFittedText (Font (iconRect.getHeight() * 0.9f, Font::bold),
                                      String::charToString ((juce_wchar) (uint8) character),
                                      (float) iconRect.getX(), (float) iconRect.getY(),
                                      (float) iconRect.getWidth(), (float) iconRect.getHeight(),
                                      Justification::centred, false);
                    ga.createPath (icon);
                    
                    icon.setUsingNonZeroWinding (false);
                    g.setColour (accentColour);
                    //g.fillPath (icon);
                    
                    iconSpaceUsed = 0;
                }
                
                g.setColour (alert.findColour (AlertWindow::textColourId));
                
                textLayout.draw (g, Rectangle<int> (textArea.getX() + iconSpaceUsed,
                                                    textArea.getY(),
                                                    textArea.getWidth() - iconSpaceUsed,
                                                    textArea.getHeight()).toFloat());
                
                g.setColour (alert.findColour (AlertWindow::outlineColourId));
                g.drawRect (0, 0, alert.getWidth(), alert.getHeight());
            }

    void drawResizableFrame (Graphics& g, int w, int h, const BorderSize<int>& border) override
            {
                if (! border.isEmpty())
                {
                    const Rectangle<int> fullSize (0, 0, w, h);
                    //const Rectangle<int> centreArea (border.subtractedFrom (fullSize));
                    
                    g.saveState();
                    
                    //g.excludeClipRegion (centreArea);
                    
                    g.setColour (Colour (0xffffffff));
                    g.drawRect (fullSize);
                    
                    g.setColour (Colour (0xffffffff));
                    //g.drawRect (centreArea.expanded (1, 1));
                    
                    g.restoreState();
                }
            }


    void createCustomTabTextLayout (const TabBarButton& button, float length, float depth,
                                                          Colour colour, TextLayout& textLayout)
            {
                const float mouseOver = button.isOver() ? 0.5f: 1.0f;
                Colour textColour;
                textColour = colour.withAlpha(mouseOver);
                String name;
                name = button.getButtonText();
                AttributedString a;
                a.setJustification (Justification::centred);
                
                String category;
                
                if (name.containsChar (':'))
                {
                    category = name.upToFirstOccurrenceOf (":", false, false);
                    name = name.fromFirstOccurrenceOf (":", false, false).trim();
                    
                    
                    if (button.getHeight() > 100)
                        category << "\n";
                    else
                        category << " ";
                }
                
                if (category.isNotEmpty()) {
                    Font font1 (depth * 0.5f);
                    font1.setUnderline (button.hasKeyboardFocus (false));
                    font1.setTypefaceName("font-middle");
                    font1.setHeight(30.0f);
                    a.append (category, font1, textColour);
                }
                Font font1 (depth * 0.5f);
                font1.setUnderline (button.hasKeyboardFocus (false));
                font1.setTypefaceName("Quicksand");
                font1.setHeight(16.0f);
                a.append (name, font1, textColour);
                
                textLayout.createLayout (a, length);
            }
                
    void drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override
            {
                const Rectangle<int> activeArea (button.getActiveArea());
                
                
                const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();
                
                const Colour bkg (button.getTabBackgroundColour());
                
                if (button.getToggleState())
                {
                    g.setColour (bkg);
                }
                else
                {
                    Point<int> p1, p2;
                    
                    switch (o)
                    {
                        case TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
                        case TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
                        case TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
                        case TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
                        default:                              jassertfalse; break;
                    }
                    
                    //g.setGradientFill (ColourGradient (bkg.brighter (0.2f), (float) p1.x, (float) p1.y,
                                                       //bkg.darker (0.1f),   (float) p2.x, (float) p2.y, false));
                    g.setColour(accentColour);
                }
                
                g.fillRect (activeArea);
                
                g.setColour (button.findColour (TabbedButtonBar::tabOutlineColourId));
                
                Rectangle<int> r (activeArea);
                
                if (o != TabbedButtonBar::TabsAtBottom)   g.fillRect (r.removeFromTop (1));
                if (o != TabbedButtonBar::TabsAtTop)      g.fillRect (r.removeFromBottom (1));
                if (o != TabbedButtonBar::TabsAtRight)    g.fillRect (r.removeFromLeft (1));
                if (o != TabbedButtonBar::TabsAtLeft)     g.fillRect (r.removeFromRight (1));
                
                const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
                
                Colour col (bkg.contrasting().withMultipliedAlpha (alpha));
                if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
                {
                    TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId
                    : TabbedButtonBar::tabTextColourId;
                    
                    if (bar->isColourSpecified (colID))
                        col = bar->findColour (colID);
                    else if (isColourSpecified (colID))
                        col = findColour (colID);
                }
                const Rectangle<float> area (button.getTextArea().toFloat());
                
                float length = area.getWidth();
                float depth  = area.getHeight();
                
                if (button.getTabbedButtonBar().isVertical())
                    std::swap (length, depth);
                
                TextLayout textLayout;
                createCustomTabTextLayout (button, length, depth, col, textLayout);
                
                AffineTransform t;
                
                switch (o)
                {
                    case TabbedButtonBar::TabsAtLeft:   t = t.rotated (float_Pi * -0.5f).translated (area.getX(), area.getBottom()); break;
                    case TabbedButtonBar::TabsAtRight:  t = t.rotated (float_Pi *  0.5f).translated (area.getRight(), area.getY()); break;
                    case TabbedButtonBar::TabsAtTop:
                    case TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
                    default:                            jassertfalse; break;
                }
                
                g.addTransform (t);
                textLayout.draw (g, Rectangle<float> (length, depth));
            }
                
                
    void drawTabAreaBehindFrontButton (TabbedButtonBar& bar, Graphics& g, const int w, const int h) override
            {
                const float shadowSize = 0.0f;
                
                Rectangle<int> shadowRect, line;
                ColourGradient gradient (Colours::black.withAlpha (bar.isEnabled() ? 0.08f : 0.04f), 0, 0,
                                         Colours::transparentBlack, 0, 0, false);
                
                switch (bar.getOrientation())
                {
                    case TabbedButtonBar::TabsAtLeft:
                        gradient.point1.x = (float) w;
                        gradient.point2.x = w * (1.0f - shadowSize);
                        shadowRect.setBounds ((int) gradient.point2.x, 0, w - (int) gradient.point2.x, h);
                        line.setBounds (w - 1, 0, 1, h);
                        break;
                        
                    case TabbedButtonBar::TabsAtRight:
                        gradient.point2.x = w * shadowSize;
                        shadowRect.setBounds (0, 0, (int) gradient.point2.x, h);
                        line.setBounds (0, 0, 1, h);
                        break;
                        
                    case TabbedButtonBar::TabsAtTop:
                        gradient.point1.y = (float) h;
                        gradient.point2.y = h * (1.0f - shadowSize);
                        shadowRect.setBounds (0, (int) gradient.point2.y, w, h - (int) gradient.point2.y);
                        line.setBounds (0, h - 1, w, 1);
                        break;
                        
                    case TabbedButtonBar::TabsAtBottom:
                        gradient.point2.y = h * shadowSize;
                        shadowRect.setBounds (0, 0, w, (int) gradient.point2.y);
                        line.setBounds (0, 0, w, 1);
                        break;
                        
                    default: break;
                }
                
                g.setGradientFill (gradient);
                g.fillRect (shadowRect.expanded (2, 2));
                
                g.setColour (bar.findColour (TabbedButtonBar::tabOutlineColourId));
                g.fillRect (line);
            }
    
    Colour getAccentColour() {
        return accentColour;
    }
    
private:
Colour accentColour;
};


            
#endif   // __ALTLOOKANDFEEL_JUCEHEADER__
