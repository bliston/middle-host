//
//  MenuButton.hpp
//  Plugin Host
//
//  Created by Andrew Liston on 2017-01-15.
//
//

#include "../JuceLibraryCode/JuceHeader.h"
#include "MenuLookAndFeel.h"

class MenuButton: public TextButton

{
public:
    MenuButton();
    void setBackgroundColour (Colour newBackgroundColour);
    void setOutlineColour (Colour newOutlineColour);
    void setOutlineSize(float newOutlineSize);
    void setCornerSize (float newCornerSize);
    void setGradientSize (float newGradientSize);
    
private:
    MenuLookAndFeel menuLookAndFeel;
};

MenuButton::MenuButton()
{
    this->setLookAndFeel (&menuLookAndFeel);
}

void MenuButton::setBackgroundColour (Colour newBackgroundColour)
{
    menuLookAndFeel.setButtonColour (newBackgroundColour);
}

void MenuButton::setOutlineColour (Colour newOutlineColour)
{
    menuLookAndFeel.setOutlineColour (newOutlineColour);
}

void MenuButton::setOutlineSize (float newOutlineSize)
{
    menuLookAndFeel.setOutlineSize (newOutlineSize);
}

void MenuButton::setCornerSize (float newCornerSize)
{
    menuLookAndFeel.setCornerSize (newCornerSize);
}

void MenuButton::setGradientSize (float newGradientSize)
{
    menuLookAndFeel.setGradientSize (newGradientSize);
}