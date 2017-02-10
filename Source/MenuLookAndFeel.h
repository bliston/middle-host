//
//  MenuLookAndFeel.hpp
//  Plugin Host
//
//  Created by Andrew Liston on 2017-01-15.
//
//



#include "../JuceLibraryCode/JuceHeader.h"

class MenuLookAndFeel : public LookAndFeel_V3
{
public:
    MenuLookAndFeel()
    {
        setColour(TextButton::textColourOnId, Colour (0xff1abc9c));
        setColour(TextButton::buttonColourId, Colour (0xffffffff));
        setCornerSize(0.0f);
        setOutlineSize(0.0f);
        setGradientSize(0.0f);
        setOutlineColour(findColour(TextButton::buttonColourId));
    }
    
    void drawButtonShape (Graphics& g, const Path& outline, Colour baseColour, float height)
    {
        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();
        
        g.setGradientFill (ColourGradient (baseColour.brighter (gradientSize), 0.0f, 0.0f,
                                               baseColour.darker (gradientSize), 0.0f, height, false));
        g.fillPath (outline);
        
        g.setColour (Colours::white.withAlpha (0.4f * mainAlpha * mainBrightness * mainBrightness));
        //g.strokePath (outline, PathStrokeType (1.0f), AffineTransform::translation (0.0f, 1.0f)
                      //.scaled (1.0f, (height - 1.6f) / height));
        //sets outline colour
        //g.setColour (Colours::black.withAlpha (0.4f * mainAlpha));
        g.setColour (outlineColour);
        g.strokePath (outline, PathStrokeType (outlineSize));
    }
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                               bool isMouseOverButton, bool isButtonDown) override
    {
        
        //shadow.setShadowProperties (DropShadow (Colours::black.withAlpha (0.4f), 3, Point<int> (0, 1)));
        //button.setComponentEffect (&shadow);
        
        Colour baseColour (backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                           .withMultipliedAlpha (button.isEnabled() ? 0.9f : 0.5f));
        
        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting (isButtonDown ? 0.1f : 0.05f);
        
        const bool flatOnLeft   = button.isConnectedOnLeft();
        const bool flatOnRight  = button.isConnectedOnRight();
        const bool flatOnTop    = button.isConnectedOnTop();
        const bool flatOnBottom = button.isConnectedOnBottom();
        
        const float width  = button.getWidth() - 1.0f;
        const float height = button.getHeight() - 1.0f;
        
        if (width > 0 && height > 0)
        {
            
            Path outline;
            outline.addRoundedRectangle (0.5f, 0.5f, width, height, cornerSize, cornerSize,
                                         ! (flatOnLeft  || flatOnTop),
                                         ! (flatOnRight || flatOnTop),
                                         ! (flatOnLeft  || flatOnBottom),
                                         ! (flatOnRight || flatOnBottom));
            
            drawButtonShape (g, outline, baseColour, height);
        }
    }
    
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        Colour textColour;
        textColour = button.findColour(TextButton::textColourOnId);
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
                
            void setButtonColour(Colour colour) {
                    setColour(TextButton::buttonColourId, colour);
            }
            
            void setCornerSize (float newCornerSize)
            {
                cornerSize = newCornerSize;
            }
            
            void setGradientSize (float newGradientSize)
            {
                gradientSize = newGradientSize;
            }
            
            void setOutlineColour(Colour newColour) {
                outlineColour = newColour;
            }
            
            void setOutlineSize (float newOutlineSize)
            {
                outlineSize = newOutlineSize;
            }
                
private:
    float cornerSize;
    float gradientSize;
    Colour outlineColour;
    float outlineSize;
    DropShadowEffect shadow;
};
