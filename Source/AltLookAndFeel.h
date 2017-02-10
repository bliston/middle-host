

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
        LookAndFeel::setDefaultSansSerifTypefaceName("Quicksand");
        setColour(TextButton::textColourOnId, Colour (0xffffffff));
        setColour(TextButton::buttonColourId, Colour (0xff4FC3F7));
        setColour (AlertWindow::backgroundColourId, Colour (0xffffffff));
        setColour (AlertWindow::textColourId, Colour (0xff000000));
        setColour (AlertWindow::outlineColourId, Colour (0xffffffff));
        setColour(TextEditor::focusedOutlineColourId, Colour (0xff4FC3F7));
        setColour(TextEditor::highlightedTextColourId, Colour (0xff4FC3F7));
        setColour(TextEditor::textColourId, Colour (0xff4FC3F7));
        setColour(TabbedButtonBar::tabOutlineColourId, Colours::whitesmoke);
        setColour(TabbedComponent::backgroundColourId, Colours::white);
        setColour(TabbedComponent::outlineColourId, Colours::white);

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
        //textColour = *new Colour(Colour (0xff000000));
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
                
                g.setColour (isEnabled ? Colours::blue.withAlpha (isButtonDown ? 0.3f : 0.1f)
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
                g.setColour (Colours::white);
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
                    g.setColour(Colours::whitesmoke);
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
                createTabTextLayout (button, length, depth, col, textLayout);
                
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
                
                
                };