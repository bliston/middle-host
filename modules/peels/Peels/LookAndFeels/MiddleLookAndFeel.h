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

#ifndef JUCE_MIDDLELOOKANDFEEL_H_INCLUDED
#define JUCE_MIDDLELOOKANDFEEL_H_INCLUDED

#include "../PeelsCommon.h"
#include "../ColorIds/AppColours.h"
#include "../BinaryData/BinaryData.h"


namespace LookAndFeelHelpers
{
	static Colour createBaseColour(Colour buttonColour,
		bool hasKeyboardFocus,
		bool isMouseOverButton,
		bool isButtonDown) noexcept
	{
		const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
		const Colour baseColour(buttonColour.withMultipliedSaturation(sat));

		if (isButtonDown)      return baseColour.contrasting(0.2f);
		if (isMouseOverButton) return baseColour.contrasting(0.1f);

		return baseColour;
	}
}
namespace Peels
{
class MiddleLookAndFeel : public LookAndFeel_V3
{
public:
	MiddleLookAndFeel()
	{
		typeface = Typeface::createSystemTypefaceFor(BinaryData::quicksand_regular_ttf, BinaryData::quicksand_regular_ttf_Size);
		typefaceIcons = Typeface::createSystemTypefaceFor(BinaryData::material_icons_regular_ttf, BinaryData::material_icons_regular_ttf_Size);

		const Colour accentColour(Colour(0xff0EADFF));

		setColour(mainAccentColourId, accentColour);
		setColour(lightAccentColourId, Colour(0xffFAFAFA));
		setColour(mainBackgroundColourId, Colour(0xffFAFAFA));
		setColour(TextButton::textColourOffId, Colours::white);
		setColour(TextButton::buttonColourId, accentColour);
		setColour(TextButton::textColourOnId, Colours::white);
		setColour(ToggleButton::tickColourId, accentColour);
		setColour(ToggleButton::tickDisabledColourId, Colours::lightgrey);
		setColour(Label::outlineColourId, Colours::lightgrey);

		setColour(AlertWindow::backgroundColourId, findColour(mainBackgroundColourId));
		setColour(AlertWindow::textColourId, findColour(mainBackgroundColourId).contrasting());
		setColour(AlertWindow::outlineColourId, Colour(0xffffffff));
		setColour(TextEditor::focusedOutlineColourId, accentColour);
		setColour(TextEditor::highlightedTextColourId, accentColour);
		setColour(TextEditor::textColourId, accentColour);
		setColour(TabbedButtonBar::tabOutlineColourId, accentColour);
		setColour(TabbedButtonBar::frontTextColourId, Colours::white);
		setColour(TabbedButtonBar::tabTextColourId, Colours::lightgrey);
		setColour(TabbedComponent::backgroundColourId, accentColour);
		setColour(TabbedComponent::outlineColourId, Colours::white);
		setColour(DocumentWindow::textColourId, Colours::grey);
		setColour(ProgressBar::foregroundColourId, accentColour);
		setColour(ProgressBar::backgroundColourId, Colours::white);
		setColour(0x1005003, /*MidiKeyboardComponent::mouseOverKeyOverlayColourId*/ accentColour.withAlpha(0.7f));
		setColour(0x1005004, /*MidiKeyboardComponent::keyDownOverlayColourId*/ accentColour.withAlpha(0.7f));
		setColour(0x1005008, /*MidiKeyboardComponent::shadowColourId*/ Colour(0xff575757));
		setColour(0x1005006, /*MidiKeyboardComponent::upDownButtonBackgroundColourId*/ Colours::whitesmoke);
		setColour(0x1005001, /*MidiKeyboardComponent::blackNoteColourId*/ Colour(0xff575757));
		setColour(0x1005007, /*MidiKeyboardComponent::upDownButtonArrowColourId*/ accentColour);
		setColour(TextEditor::highlightColourId, accentColour.withAlpha(0.2f));
		setColour(PopupMenu::highlightedBackgroundColourId, findColour(lightAccentColourId));
		setColour(PopupMenu::highlightedTextColourId, findColour(mainBackgroundColourId).contrasting());
		setColour(TextEditor::outlineColourId, Colours::lightgrey);
		setColour(ComboBox::outlineColourId, Colours::lightgrey);
		//setColour(ComboBox::backgroundColourId, accentColour);
		setColour(ComboBox::buttonColourId, Colours::transparentWhite);
		setColour(ComboBox::backgroundColourId, Colours::transparentWhite);
		setColour(ComboBox::arrowColourId, Colours::lightgrey);
		setColour(Slider::thumbColourId, accentColour);
		setColour(Slider::trackColourId, Colours::whitesmoke);

	}

	//==============================================================================


	Font getCustomFont()
	{
		Font font(typeface);
		return font;
	}

	Font getIconFont()
	{
		Font font(typefaceIcons);
		return font;
	}

	Font getLabelFont(Label& label) override
	{
		Font font(typeface);
		return font;
	}

	void drawLabel(Graphics& g, Label& label) override
	{

		g.fillAll(label.findColour(Label::backgroundColourId));

		if (!label.isBeingEdited())
		{
			const float alpha = label.isEnabled() ? 1.0f : 0.5f;
			//const Font font (getLabelFont (label));
			String name;
			name = label.getText();
			AttributedString a;
			a.setJustification(label.getJustificationType());

			String category;

			if (name.containsChar('~'))
			{
				category = name.upToFirstOccurrenceOf("~", false, false);
				name = name.fromFirstOccurrenceOf("~", false, false).trim();


				if (label.getHeight() > 20)
					category << "\n";
				else
					category << " ";
			}

			Font font1(typefaceIcons);
			font1.setHeight(30.0f);
			if (category.isNotEmpty()) {

				a.append(category, font1, label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
			}
			Font font2(typeface);
			font2.setHeight(14.0f);
			a.append(name, font2, label.findColour(Label::textColourId).withMultipliedAlpha(alpha));



			Rectangle<int> textArea(label.getBorderSize().subtractedFrom(label.getLocalBounds()));

			if (label.getWidth() > 0)
				a.draw(g, Rectangle<int>(label.getWidth(), label.getHeight()).toFloat());

			g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
		}
		else if (label.isEnabled())
		{
			g.setColour(label.findColour(Label::outlineColourId));
		}

		if (label.isEditable())
			g.drawRoundedRectangle(0, label.getHeight(), label.getWidth(), 0, 5, 3);

	}

	int getTextButtonWidthToFitText(TextButton& b, int buttonHeight) override
	{
		Font f(typeface);
		f.setHeight(jmin(15.0f, buttonHeight * 0.6f));
		return f.getStringWidth(b.getButtonText()) + buttonHeight;
	}

	static void drawButtonShape(Graphics& g, const Path& outline, Colour baseColour, float height)
	{
		const float mainBrightness = baseColour.getBrightness();
		const float mainAlpha = baseColour.getFloatAlpha();

		//g.setGradientFill (ColourGradient (baseColour.brighter (0.2f), 0.0f, 0.0f,
		//baseColour.darker (0.25f), 0.0f, height, false));
		g.fillPath(outline);

		g.setColour(Colours::white.withAlpha(0.4f * mainAlpha * mainBrightness * mainBrightness));
		g.strokePath(outline, PathStrokeType(1.0f), AffineTransform::translation(0.0f, 1.0f)
			.scaled(1.0f, (height - 1.6f) / height));

		g.setColour(Colours::black.withAlpha(0.4f * mainAlpha));
		g.strokePath(outline, PathStrokeType(1.0f));
	}

	void drawButtonText(Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
	{
		const float mouseOver = isMouseOverButton ? 0.5f : 1.0f;
		Colour textColour;
		//textColour = *new Colour(Colour (0xff000000));
		textColour = button.findColour(TextButton::textColourOnId).withAlpha(mouseOver);
		String name;
		name = button.getButtonText();
		AttributedString a;
		a.setJustification(Justification::centred);

		String category;

		if (name.containsChar('~'))
		{
			category = name.upToFirstOccurrenceOf("~", false, false);
			name = name.fromFirstOccurrenceOf("~", false, false).trim();


			if (button.getHeight() > 20)
				category << "\n";
			else
				category << " ";
		}
		Font font1(typefaceIcons);
		font1.setHeight(30.0f);
		if (category.isNotEmpty()) {

			a.append(category, font1, textColour);
		}
		Font font2(typeface);
		font2.setHeight(14.0f);
		a.append(name, font2, textColour);






		a.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
			: TextButton::textColourOffId)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

		const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
		const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

		const int fontHeight = roundToInt((font1.getHeight() + font2.getHeight()) * 0.6f);
		const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
		const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
		const int textWidth = button.getWidth() - leftIndent - rightIndent;

		const int edge = 4;
		const int offset = isButtonDown ? edge / 2 : 0;

		if (textWidth > 0)
			//g.drawFittedText (button.getButtonText(),
			//leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
			//Justification::centred, 2);
			a.draw(g, Rectangle<int>(button.getWidth(), button.getHeight()).toFloat());
	}


	void drawButtonBackground(Graphics& g,
		Button& button,
		const Colour& backgroundColour,
		bool isMouseOverButton,
		bool isButtonDown) override
	{
		const int width = button.getWidth();
		const int height = button.getHeight();

		const float outlineThickness = button.isEnabled() ? ((isButtonDown || isMouseOverButton) ? 1.2f : 0.7f) : 0.4f;
		const float halfThickness = outlineThickness * 0.5f;

		const float indentL = button.isConnectedOnLeft() ? 0.1f : halfThickness;
		const float indentR = button.isConnectedOnRight() ? 0.1f : halfThickness;
		const float indentT = button.isConnectedOnTop() ? 0.1f : halfThickness;
		const float indentB = button.isConnectedOnBottom() ? 0.1f : halfThickness;

		const Colour baseColour(LookAndFeelHelpers::createBaseColour(backgroundColour,
			button.hasKeyboardFocus(true),
			isMouseOverButton, isButtonDown)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

		drawGlassLozenge(g,
			indentL,
			indentT,
			width - indentL - indentR,
			height - indentT - indentB,
			baseColour, outlineThickness, -1.0f,
			button.isConnectedOnLeft(),
			button.isConnectedOnRight(),
			button.isConnectedOnTop(),
			button.isConnectedOnBottom());

	}

	int getTabButtonBestWidth(TabBarButton&, int) override
	{
		return 120;
	}

	Colour getTabBackgroundColour(TabBarButton& button)
	{
		const Colour bkg(button.findColour(mainBackgroundColourId).contrasting(0.15f));

		if (button.isFrontTab())
			return bkg.overlaidWith(Colours::yellow.withAlpha(0.5f));

		return bkg;
	}

	void drawTabButton(TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override
	{
		const Rectangle<int> activeArea(button.getActiveArea());

		const Colour bkg(getTabBackgroundColour(button));

		g.setGradientFill(ColourGradient(bkg.brighter(0.1f), 0, (float)activeArea.getY(),
			bkg.darker(0.1f), 0, (float)activeArea.getBottom(), false));
		g.fillRect(activeArea);

		g.setColour(button.findColour(mainBackgroundColourId).darker(0.3f));
		g.drawRect(activeArea);

		const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
		const Colour col(bkg.contrasting().withMultipliedAlpha(alpha));

		TextLayout textLayout;
		LookAndFeel_V3::createTabTextLayout(button, (float)activeArea.getWidth(), (float)activeArea.getHeight(), col, textLayout);

		textLayout.draw(g, button.getTextArea().toFloat());
	}

	void drawConcertinaPanelHeader(Graphics& g, const Rectangle<int>& area,
		bool isMouseOver, bool /*isMouseDown*/,
		ConcertinaPanel&, Component& panel) override
	{
		const Colour bkg(Colours::grey);

		g.setGradientFill(ColourGradient(Colour::greyLevel(isMouseOver ? 0.6f : 0.5f), 0, (float)area.getY(),
			Colour::greyLevel(0.4f), 0, (float)area.getBottom(), false));
		g.fillAll();

		g.setColour(bkg.contrasting().withAlpha(0.1f));
		g.fillRect(area.withHeight(1));
		g.fillRect(area.withTop(area.getBottom() - 1));

		g.setColour(bkg.contrasting());
		g.setFont(Font(area.getHeight() * 0.6f).boldened());
		g.drawFittedText(panel.getName(), 4, 0, area.getWidth() - 6, area.getHeight(), Justification::centredLeft, 1);
	}

	static Range<float> getBrightnessRange(const Image& im)
	{
		float minB = 1.0f, maxB = 0;
		const int w = im.getWidth();
		const int h = im.getHeight();

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				const float b = im.getPixelAt(x, y).getBrightness();
				minB = jmin(minB, b);
				maxB = jmax(maxB, b);
			}
		}

		return Range<float>(minB, maxB);
	}

	void fillWithBackgroundTexture(Graphics& g)
	{
		g.setColour(findColour(mainBackgroundColourId));
		g.fillAll();

	}

	void fillWithBackgroundTexture(Component& c, Graphics& g)
	{
		dynamic_cast<MiddleLookAndFeel&> (c.getLookAndFeel()).fillWithBackgroundTexture(g);
	}

	void drawDocumentWindowTitleBar(DocumentWindow& window, Graphics& g,
		int w, int h, int titleSpaceX, int titleSpaceW,
		const Image* icon, bool drawTitleTextOnLeft) override
	{
		if (w * h == 0)
			return;

		const bool isActive = window.isActiveWindow();

		g.setGradientFill(ColourGradient(window.getBackgroundColour(),
			0.0f, 0.0f,
			window.getBackgroundColour().contrasting(isActive ? 0.15f : 0.05f),
			0.0f, (float)h, false));
		g.setColour(Colours::lightgrey);
		g.drawRoundedRectangle(0, h, w, 0, 5, 3);
		//g.fillAll();

		//Font font(h * 0.65f, Font::plain);
		Font font(typeface);
		font.setHeight(14.0f);
		g.setFont(font);

		int textW = font.getStringWidth(window.getName());
		int iconW = 0;
		int iconH = 0;

		if (icon != nullptr)
		{
			iconH = (int)font.getHeight();
			iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
		}

		textW = jmin(titleSpaceW, textW + iconW);
		int textX = drawTitleTextOnLeft ? titleSpaceX
			: jmax(titleSpaceX, (w - textW) / 2);

		if (textX + textW > titleSpaceX + titleSpaceW)
			textX = titleSpaceX + titleSpaceW - textW;

		if (icon != nullptr)
		{
			g.setOpacity(isActive ? 1.0f : 0.6f);
			g.drawImageWithin(*icon, textX, (h - iconH) / 2, iconW, iconH,
				RectanglePlacement::centred, false);
			textX += iconW;
			textW -= iconW;
		}

		if (window.isColourSpecified(DocumentWindow::textColourId) || isColourSpecified(DocumentWindow::textColourId))
			g.setColour(window.findColour(DocumentWindow::textColourId));
		else
			g.setColour(window.getBackgroundColour().contrasting(isActive ? 0.7f : 0.4f));

		g.drawText(window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
	}

	Button* createDocumentWindowButton(int buttonType) override
	{
		Path shape;

		if (buttonType == DocumentWindow::closeButton)
		{
			//shape.addLineSegment(Line<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.1f);
			//shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 1.0f), 0.1f);

			Path close(getPathFromChar(0xE14C));
			DrawableButton* b = new DrawableButton("maximise", DrawableButton::ImageFitted);
			DrawablePath dp;
			dp.setPath(close);
			dp.setFill(findColour(DocumentWindow::textColourId));
			b->setImages(&dp);
			return b;
		}
		else if (buttonType == DocumentWindow::minimiseButton)
		{
			//shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.1f);

			Path minimize(getPathFromChar(0xE15B));
			DrawableButton* b = new DrawableButton("minimise", DrawableButton::ImageFitted);
			DrawablePath dp;
			dp.setPath(minimize);
			dp.setFill(findColour(DocumentWindow::textColourId));
			b->setImages(&dp);
			return b;
		}
		else if (buttonType == DocumentWindow::maximiseButton)
		{
			//shape.addLineSegment(Line<float>(0.5f, 0.0f, 0.5f, 1.0f), 0.1f);
			//shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.1f);

			Path maximize(getPathFromChar(0xE145));
			DrawableButton* b = new DrawableButton("maximise", DrawableButton::ImageFitted);
			DrawablePath dp;
			dp.setPath(maximize);
			dp.setFill(findColour(DocumentWindow::textColourId));
			b->setImages(&dp);
			return b;
		}

		jassertfalse;
		return nullptr;
	}
	void drawAlertBox(Graphics& g, AlertWindow& alert,
		const Rectangle<int>& textArea, TextLayout& textLayout) override
	{

		g.fillAll(alert.findColour(AlertWindow::backgroundColourId));

		int iconSpaceUsed = 0;

		const int iconWidth = 80;
		int iconSize = jmin(iconWidth + 50, alert.getHeight() + 20);

		if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
			iconSize = jmin(iconSize, textArea.getHeight() + 50);

		const Rectangle<int> iconRect(iconSize / -10, iconSize / -10,
			iconSize, iconSize);
		Font iconFont(typefaceIcons);

		iconFont.setHeight(iconRect.getHeight() * 0.5f);


		if (alert.getAlertType() != AlertWindow::NoIcon)
		{
			Path icon;
			uint32 colour;
			juce_wchar character;

			if (alert.getAlertType() == AlertWindow::WarningIcon)
			{
				colour = 0x55ff5555;
				character = 0xE000; //'!'

									//icon.addTriangle (iconRect.getX() + iconRect.getWidth() * 0.5f, (float) iconRect.getY(),
									//(float) iconRect.getRight(), (float) iconRect.getBottom(),
									//(float) iconRect.getX(), (float) iconRect.getBottom());

				icon = icon.createPathWithRoundedCorners(5.0f);
			}
			else
			{
				colour = alert.getAlertType() == AlertWindow::InfoIcon ? (uint32)0x605555ff : (uint32)0x40b69900;
				character = alert.getAlertType() == AlertWindow::InfoIcon ? 0xE887 /*'i'*/ : 0xE887; //'?'

																									 //icon.addEllipse (iconRect.toFloat());
			}

			GlyphArrangement ga;
			ga.addFittedText(iconFont,
				String::charToString(character),
				(float)iconRect.getX(), (float)iconRect.getY(),
				(float)iconRect.getWidth(), (float)iconRect.getHeight(),
				Justification::centred, false);
			ga.createPath(icon);

			icon.setUsingNonZeroWinding(false);
			g.setColour(Colour(Colours::lightgrey).withAlpha(0.4f));
			g.fillPath(icon);

			iconSpaceUsed = iconWidth;
		}

		g.setColour(alert.findColour(AlertWindow::textColourId));

		textLayout.draw(g, Rectangle<int>(textArea.getX() + iconSpaceUsed,
			textArea.getY(),
			textArea.getWidth() - iconSpaceUsed,
			textArea.getHeight()).toFloat());

		g.setColour(alert.findColour(AlertWindow::outlineColourId));
		g.drawRect(0, 0, alert.getWidth(), alert.getHeight());
	}

	Font getAlertWindowTitleFont() override
	{
		Font messageFont = getAlertWindowMessageFont();
		messageFont.setHeight(messageFont.getHeight() * 1.1f);
		messageFont.setBold(true);
		return messageFont;
	}

	Font getAlertWindowMessageFont() override
	{
		Font font(typeface);
		font.setHeight(15.0f);
		return font;
	}

	Font getAlertWindowFont() override
	{
		Font font(typeface);
		font.setHeight(12.0f);
		return font;
	}

	void drawResizableFrame(Graphics& g, int w, int h, const BorderSize<int>& border) override
	{
		if (!border.isEmpty())
		{
			const Rectangle<int> fullSize(0, 0, w, h);
			//const Rectangle<int> centreArea (border.subtractedFrom (fullSize));

			g.saveState();

			//g.excludeClipRegion (centreArea);

			g.setColour(Colour(0xffffffff));
			g.drawRect(fullSize);

			g.setColour(Colour(0xffffffff));
			//g.drawRect (centreArea.expanded (1, 1));

			g.restoreState();
		}
	}


	void drawTabAreaBehindFrontButton(TabbedButtonBar& bar, Graphics& g, const int w, const int h) override
	{
		const float shadowSize = 0.0f;

		Rectangle<int> shadowRect, line;
		ColourGradient gradient(Colours::black.withAlpha(bar.isEnabled() ? 0.08f : 0.04f), 0, 0,
			Colours::transparentBlack, 0, 0, false);

		switch (bar.getOrientation())
		{
		case TabbedButtonBar::TabsAtLeft:
			gradient.point1.x = (float)w;
			gradient.point2.x = w * (1.0f - shadowSize);
			shadowRect.setBounds((int)gradient.point2.x, 0, w - (int)gradient.point2.x, h);
			line.setBounds(w - 1, 0, 1, h);
			break;

		case TabbedButtonBar::TabsAtRight:
			gradient.point2.x = w * shadowSize;
			shadowRect.setBounds(0, 0, (int)gradient.point2.x, h);
			line.setBounds(0, 0, 1, h);
			break;

		case TabbedButtonBar::TabsAtTop:
			gradient.point1.y = (float)h;
			gradient.point2.y = h * (1.0f - shadowSize);
			shadowRect.setBounds(0, (int)gradient.point2.y, w, h - (int)gradient.point2.y);
			line.setBounds(0, h - 1, w, 1);
			break;

		case TabbedButtonBar::TabsAtBottom:
			gradient.point2.y = h * shadowSize;
			shadowRect.setBounds(0, 0, w, (int)gradient.point2.y);
			line.setBounds(0, 0, w, 1);
			break;

		default: break;
		}

		g.setGradientFill(gradient);
		g.fillRect(shadowRect.expanded(2, 2));

		g.setColour(bar.findColour(TabbedButtonBar::tabOutlineColourId));
		g.fillRect(line);
	}

	//==============================================================================
	void drawGlassSphere(Graphics& g, const float x, const float y,
		const float diameter, const Colour& colour,
		const float outlineThickness) noexcept
	{
		if (diameter <= outlineThickness)
			return;

		Path p;
		p.addEllipse(x, y, diameter, diameter);

		g.setColour(colour);
		g.drawEllipse(x, y, diameter, diameter, outlineThickness);
	}

	void drawComboBox(Graphics& g, int width, int height, const bool /*isButtonDown*/,
		int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box) override
	{
		Rectangle<int> buttonArea = box.getLocalBounds();
		g.fillAll(box.findColour(ComboBox::backgroundColourId));
		const Colour buttonColour(box.findColour(ComboBox::buttonColourId));

		if (box.isEnabled() && box.hasKeyboardFocus(false))
		{
			g.setColour(buttonColour);

			g.fillRoundedRectangle(buttonArea.toFloat().reduced(2.0f, 2.0f), 10.0f);
			g.setColour(findColour(mainAccentColourId).withMultipliedAlpha(box.isEnabled() ? 1.0f : 0.3f));
			g.drawRoundedRectangle(0, buttonArea.getHeight(), buttonArea.getWidth(), 0, 5, 3);

		}
		else
		{
			g.setColour(box.findColour(ComboBox::outlineColourId).withMultipliedAlpha(box.isEnabled() ? 1.0f : 0.3f));
			g.drawRoundedRectangle(0, buttonArea.getHeight(), buttonArea.getWidth(), 0, 5, 3);
		}

		const Path p(getPathFromChar(0xE5CF));
		Rectangle<float> iconArea(buttonArea.removeFromRight((buttonArea.getHeight() * 5) / 4).reduced(0).toFloat());

		//g.setColour(box.findColour(ComboBox::arrowColourId).withMultipliedAlpha(box.isEnabled() ? 1.0f : 0.3f));

		g.fillPath(p, p.getTransformToScaleToFit(iconArea.reduced(11), true));

	}

	void positionComboBoxText(ComboBox& box, Label& label) override
	{
		label.setBounds(10, 1,
			box.getWidth() - 10 - box.getHeight(),
			box.getHeight() - 2);

		label.setFont(getComboBoxFont(box));
		label.setJustificationType(Justification::centredLeft);
	}

	void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
		const bool isSeparator, const bool isActive,
		const bool isHighlighted, const bool isTicked,
		const bool hasSubMenu, const String& text,
		const String& shortcutKeyText,
		const Drawable* icon, const Colour* const textColourToUse) override
	{
		if (isSeparator)
		{
			Rectangle<int> r(area.reduced(5, 0));
			r.removeFromTop(r.getHeight() / 2 - 1);

			g.setColour(Colour(0x33000000));
			g.fillRect(r.removeFromTop(1));

			g.setColour(Colour(0x66ffffff));
			g.fillRect(r.removeFromTop(1));
		}
		else
		{
			Colour textColour(findColour(PopupMenu::textColourId));

			if (textColourToUse != nullptr)
				textColour = *textColourToUse;

			Rectangle<int> r(area.reduced(1));

			if (isHighlighted)
			{
				g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
				g.fillRect(r);

				g.setColour(findColour(PopupMenu::highlightedTextColourId));
			}
			else
			{
				g.setColour(textColour);
			}

			if (!isActive)
				g.setOpacity(0.3f);

			Font font(typeface);

			const float maxFontHeight = area.getHeight() / 1.3f;

			if (font.getHeight() > maxFontHeight)
				font.setHeight(maxFontHeight);

			g.setFont(font);

			Rectangle<float> iconArea(r.removeFromLeft((r.getHeight() * 5) / 4).reduced(0).toFloat());

			if (icon != nullptr)
			{
				icon->drawWithin(g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
			}
			else if (isTicked)
			{
				g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
				g.fillRect(r);
				g.fillRect(iconArea);
				g.setColour(findColour(mainAccentColourId));
				const Path tick(getPathFromChar(0xE5CA));
				g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(8), true));

				g.setColour(findColour(PopupMenu::highlightedTextColourId));

			}

			if (hasSubMenu)
			{
				const float arrowH = 0.6f * getPopupMenuFont().getAscent();

				const float x = (float)r.removeFromRight((int)arrowH).getX();
				const float halfH = (float)r.getCentreY();

				Path p;
				p.addTriangle(x, halfH - arrowH * 0.5f,
					x, halfH + arrowH * 0.5f,
					x + arrowH * 0.6f, halfH);

				g.fillPath(p);
			}

			r.removeFromRight(3);
			g.drawFittedText(text, r, Justification::centredLeft, 1);

			if (shortcutKeyText.isNotEmpty())
			{
				Font f2(font);
				f2.setHeight(f2.getHeight() * 0.75f);
				f2.setHorizontalScale(0.95f);
				g.setFont(f2);

				g.drawText(shortcutKeyText, r, Justification::centredRight, true);
			}
		}
	}

	Path getTickShape(const float height) override
	{
		Path p;
		p.startNewSubPath(1.5f, 3.0f);
		p.lineTo(3.0f, 6.0f);
		p.lineTo(6.0f, 0.0f);


		//Path p;
		//p.loadPathFromData (pathData, 559);
		p.scaleToFit(0, 0, height * 2.0f, height, true);
		return p;
	}

	//==============================================================================
	void drawLevelMeter(Graphics& g, int width, int height, float level) override
	{
		g.setColour(Colours::white.withAlpha(0.7f));
		g.fillRoundedRectangle(0.0f, 0.0f, (float)width, (float)height, 3.0f);
		g.setColour(Colours::black.withAlpha(0.2f));
		g.drawRoundedRectangle(1.0f, 1.0f, width - 2.0f, height - 2.0f, 3.0f, 1.0f);

		const int totalBlocks = 7;
		const int numBlocks = roundToInt(totalBlocks * level);
		const float w = (width - 6.0f) / (float)totalBlocks;

		for (int i = 0; i < totalBlocks; ++i)
		{
			if (i >= numBlocks)
				g.setColour(Colours::lightblue.withAlpha(0.6f));
			else
				g.setColour(i < totalBlocks - 1 ? findColour(mainAccentColourId).withAlpha(0.5f)
					: Colours::red);

			g.fillRoundedRectangle(3.0f + i * w + w * 0.1f, 3.0f, w * 0.8f, height - 6.0f, w * 0.4f);
		}
	}

	//==============================================================================
	void drawProgressBar(Graphics& g, ProgressBar& progressBar,
		int width, int height,
		double progress, const String& textToShow) override
	{
		const Colour background(progressBar.findColour(ProgressBar::backgroundColourId));
		const Colour foreground(progressBar.findColour(ProgressBar::foregroundColourId));

		g.fillAll(background);

		if (progress >= 0.0f && progress < 1.0f)
		{
			drawGlassLozenge(g, 1.0f, 1.0f,
				(float)jlimit(0.0, width - 2.0, progress * (width - 2.0)),
				(float)(height - 2),
				foreground,
				0.5f, 0.0f,
				true, true, true, true);
		}

		if (textToShow.isNotEmpty())
		{
			//g.setColour (Colour::contrasting (background, foreground));
			//g.setFont (height * 0.6f);

			//g.drawText (textToShow, 0, 0, width, height, Justification::centred, false);
		}
	}

	//==============================================================================
	void drawGlassLozenge(Graphics& g,
		const float x, const float y, const float width, const float height,
		const Colour& colour, const float outlineThickness, const float cornerSize,
		const bool flatOnLeft,
		const bool flatOnRight,
		const bool flatOnTop,
		const bool flatOnBottom) noexcept
	{
		if (width <= outlineThickness || height <= outlineThickness)
			return;

		const int intX = (int)x;
		const int intY = (int)y;
		const int intW = (int)width;
		const int intH = (int)height;

		const float cs = cornerSize < 0 ? jmin(width * 0.5f, height * 0.5f) : cornerSize;
		const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
		const int intEdge = (int)edgeBlurRadius;

		Path outline;
		outline.addRoundedRectangle(x, y, width, height, cs, cs,
			!(flatOnLeft || flatOnTop),
			!(flatOnRight || flatOnTop),
			!(flatOnLeft || flatOnBottom),
			!(flatOnRight || flatOnBottom));

		{
			g.setColour(colour);

			g.fillPath(outline);
		}

		if (!(flatOnLeft || flatOnTop || flatOnBottom))
		{
			g.saveState();
			g.setColour(colour);
			g.reduceClipRegion(intX, intY, intEdge, intH);
			g.fillPath(outline);
			g.restoreState();
		}

		if (!(flatOnRight || flatOnTop || flatOnBottom))
		{
			g.saveState();
			g.setColour(colour);
			g.reduceClipRegion(intX + intW - intEdge, intY, 2 + intEdge, intH);
			g.fillPath(outline);
			g.restoreState();
		}

		{
			const float leftIndent = flatOnTop || flatOnLeft ? 0.0f : cs * 0.4f;
			const float rightIndent = flatOnTop || flatOnRight ? 0.0f : cs * 0.4f;

			Path highlight;
			highlight.addRoundedRectangle(x + leftIndent,
				y + cs * 0.1f,
				width - (leftIndent + rightIndent),
				height * 0.4f,
				cs * 0.4f,
				cs * 0.4f,
				!(flatOnLeft || flatOnTop),
				!(flatOnRight || flatOnTop),
				!(flatOnLeft || flatOnBottom),
				!(flatOnRight || flatOnBottom));

			g.setColour(colour);
			g.fillPath(highlight);
		}

		g.setColour(colour);
		g.strokePath(outline, PathStrokeType(outlineThickness));
	}

	void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) override
	{
		if (textEditor.isEnabled())
		{
			if (textEditor.hasKeyboardFocus(true) && !textEditor.isReadOnly())
			{
				g.setColour(textEditor.findColour(TextEditor::focusedOutlineColourId));
				g.drawRoundedRectangle(0, height, width, 0, 5, 3);
			}
			else
			{
				g.setColour(textEditor.findColour(TextEditor::outlineColourId));
				g.drawRoundedRectangle(0, height, width, 0, 5, 3);
			}
		}
	}


	Path getPathFromChar(juce_wchar character,
		const Rectangle<int>& textArea)
	{

		//const int iconWidth = 80;
		int iconSize = jmin(textArea.getWidth() + 50, textArea.getHeight() + 20);

		const Rectangle<int> iconRect(iconSize / -10, iconSize / -10,
			iconSize, iconSize);
		Path icon;
		Colour colour;

		colour = findColour(mainAccentColourId);

		GlyphArrangement ga;
		Font iconFont(typefaceIcons);
		iconFont.setHeight(iconRect.getHeight() * 0.9f);
		ga.addFittedText(iconFont,
			String::charToString(character),
			(float)iconRect.getX(), (float)iconRect.getY(),
			(float)iconRect.getWidth(), (float)iconRect.getHeight(),
			Justification::centred, false);

		ga.createPath(icon);


		icon.setUsingNonZeroWinding(false);

		return icon;

	}

	Path getPathFromChar(juce_wchar codePoint) {
		Path p;
		Typeface *tface = typefaceIcons;
		Array<int> glyphs;
		Array<float> offsets;
		String s(CharPointer_UTF32(&codePoint), 1);
		tface->getGlyphPositions(s, glyphs, offsets);
		Path outline;
		tface->getOutlineForGlyph(glyphs[0], p);
		//p.addPath(outline, AffineTransform::scale(20));
		//g.fillPath(p);
		//p.getTransformToScaleToFit(0, 0, 80, 80, true);
		return p;
	}


	void drawTickBox(Graphics& g, Component& component,
		float x, float y, float w, float h,
		const bool ticked,
		const bool isEnabled,
		const bool /*isMouseOverButton*/,
		const bool isButtonDown) override
	{
		//Path box;
		//box.addRoundedRectangle (0.0f, 2.0f, 6.0f, 6.0f, 1.0f);

		g.setColour(isEnabled ? Colours::blue.withAlpha(isButtonDown ? 0.3f : 0.1f)
			: Colours::lightgrey.withAlpha(0.1f));

		//AffineTransform trans (AffineTransform::scale (w / 9.0f, h / 9.0f).translated (x, y));

		const float boxSize = w * 0.7f;
		Rectangle<float> tickArea(x, y + (h - boxSize) * 0.5f, boxSize, boxSize);

		Path box;

		ToggleButton*toggle = dynamic_cast<ToggleButton*>(&component);
		if (toggle)
		{
			// it's a ToggleButton
			if (toggle->getRadioGroupId() > 0)
			{
				box = (getPathFromChar(0xE836));
			}
			else
			{
				box = (getPathFromChar(0xE835));
			}
		}
		else
		{
			box = (getPathFromChar(0xE835));
		}



		g.setColour(Colours::black.withAlpha(0.6f));
		g.fillPath(box, box.getTransformToScaleToFit(tickArea.reduced(0), true));

		if (ticked)
		{

			Path tick;

			if (toggle)
			{
				// it's a ToggleButton
				if (toggle->getRadioGroupId() > 0)
				{
					tick = (getPathFromChar(0xE837));
				}
				else
				{
					tick = (getPathFromChar(0xE834));
				}
			}
			else
			{
				tick = (getPathFromChar(0xE834));
			}

			g.setColour(component.findColour(isEnabled ? ToggleButton::tickColourId
				: ToggleButton::tickDisabledColourId));

			g.fillPath(tick, tick.getTransformToScaleToFit(tickArea.reduced(0), true));
		}
	}

	void drawToggleButton(Graphics& g, ToggleButton& button,
		bool isMouseOverButton, bool isButtonDown) override
	{
		if (button.hasKeyboardFocus(true))
		{
			g.setColour(button.findColour(TextEditor::focusedOutlineColourId));
			//g.drawRect (0, 0, button.getWidth(), button.getHeight());
		}

		float fontSize = jmin(15.0f, button.getHeight() * 0.75f);
		const float tickWidth = fontSize * 1.1f;


		drawTickBox(g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
			tickWidth, tickWidth,
			button.getToggleState(),
			button.isEnabled(),
			isMouseOverButton,
			isButtonDown);


		g.setColour(button.findColour(ToggleButton::textColourId));
		Font font(typeface);
		font.setHeight(fontSize);
		g.setFont(font);

		if (!button.isEnabled())
			g.setOpacity(0.5f);

		const int textX = (int)tickWidth + 5;

		g.drawFittedText(button.getButtonText(),
			textX, 0,
			button.getWidth() - textX - 2, button.getHeight(),
			Justification::centredLeft, 10);
	}

	void changeToggleButtonWidthToFitText(ToggleButton& button) override
	{
		Font font(jmin(15.0f, button.getHeight() * 0.6f));

		const int tickWidth = jmin(24, button.getHeight());

		button.setSize(font.getStringWidth(button.getButtonText()) + tickWidth + 8,
			button.getHeight());
	}



	void drawSphere(Graphics& g, const float x, const float y,
		const float diameter, const Colour& colour,
		const float outlineThickness) noexcept
	{
		if (diameter <= outlineThickness)
			return;

		g.setColour(colour);
		g.fillEllipse(x, y, diameter, diameter);
	}


	void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const Slider::SliderStyle style, Slider& slider) override
	{
		g.fillAll(slider.findColour(Slider::backgroundColourId));

		if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
		{
			const float fx = (float)x, fy = (float)y, fw = (float)width, fh = (float)height;

			Path p;

			if (style == Slider::LinearBarVertical)
				p.addRectangle(fx, sliderPos, fw, 1.0f + fh - sliderPos);
			else
				p.addRectangle(fx, fy, sliderPos - fx, fh);

			Colour baseColour(slider.findColour(Slider::thumbColourId)
				.withMultipliedSaturation(slider.isEnabled() ? 1.0f : 0.5f)
				.withMultipliedAlpha(0.8f));

			//g.setGradientFill (ColourGradient (baseColour.brighter (0.08f), 0.0f, 0.0f,
			//baseColour.darker (0.08f), 0.0f, (float) height, false));
			g.fillAll(baseColour);
			g.fillPath(p);

			g.setColour(baseColour.darker(0.2f));

			if (style == Slider::LinearBarVertical)
				g.fillRect(fx, sliderPos, fw, 1.0f);
			else
				g.fillRect(sliderPos, fy, 1.0f, fh);
		}
		else
		{
			drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
			drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
		}
	}

	void drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height,
		float /*sliderPos*/,
		float /*minSliderPos*/,
		float /*maxSliderPos*/,
		const Slider::SliderStyle /*style*/, Slider& slider) override
	{
		const float sliderRadius = (float)(getSliderThumbRadius(slider) - 2);

		const Colour trackColour(slider.findColour(Slider::trackColourId));
		const Colour gradCol1(trackColour.overlaidWith(Colour(slider.isEnabled() ? 0x13000000 : 0x09000000)));
		const Colour gradCol2(trackColour.overlaidWith(Colour(0x06000000)));
		Path indent;

		if (slider.isHorizontal())
		{
			const float iy = y + height * 0.5f - sliderRadius * 0.5f;

			g.setGradientFill(ColourGradient(gradCol1, 0.0f, iy,
				gradCol2, 0.0f, iy + sliderRadius, false));

			indent.addRoundedRectangle(x - sliderRadius * 0.5f, iy, width + sliderRadius, sliderRadius, 5.0f);
		}
		else
		{
			const float ix = x + width * 0.5f - sliderRadius * 0.5f;

			g.setGradientFill(ColourGradient(gradCol1, ix, 0.0f,
				gradCol2, ix + sliderRadius, 0.0f, false));

			indent.addRoundedRectangle(ix, y - sliderRadius * 0.5f, sliderRadius, height + sliderRadius, 5.0f);
		}

		g.setColour(trackColour);
		g.fillPath(indent);
		//g.strokePath (indent, PathStrokeType (0.5f));
	}

	void drawLinearSliderThumb(Graphics& g, int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const Slider::SliderStyle style, Slider& slider) override
	{
		const float sliderRadius = (float)(getSliderThumbRadius(slider) - 2);

		Colour knobColour(LookAndFeelHelpers::createBaseColour(slider.findColour(Slider::thumbColourId),
			slider.hasKeyboardFocus(false) && slider.isEnabled(),
			slider.isMouseOverOrDragging() && slider.isEnabled(),
			slider.isMouseButtonDown() && slider.isEnabled()));

		const float outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;

		if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
		{
			float kx, ky;

			if (style == Slider::LinearVertical)
			{
				kx = x + width * 0.5f;
				ky = sliderPos;
			}
			else
			{
				kx = sliderPos;
				ky = y + height * 0.5f;
			}

			drawSphere(g,
				kx - sliderRadius,
				ky - sliderRadius,
				sliderRadius * 2.0f,
				knobColour, outlineThickness);
		}
		else
		{
			if (style == Slider::ThreeValueVertical)
			{
				drawSphere(g, x + width * 0.5f - sliderRadius,
					sliderPos - sliderRadius,
					sliderRadius * 2.0f,
					knobColour, outlineThickness);
			}
			else if (style == Slider::ThreeValueHorizontal)
			{
				drawSphere(g, sliderPos - sliderRadius,
					y + height * 0.5f - sliderRadius,
					sliderRadius * 2.0f,
					knobColour, outlineThickness);
			}

			if (style == Slider::TwoValueVertical || style == Slider::ThreeValueVertical)
			{
				const float sr = jmin(sliderRadius, width * 0.4f);

				drawGlassPointer(g, jmax(0.0f, x + width * 0.5f - sliderRadius * 2.0f),
					minSliderPos - sliderRadius,
					sliderRadius * 2.0f, knobColour, outlineThickness, 1);

				drawGlassPointer(g, jmin(x + width - sliderRadius * 2.0f, x + width * 0.5f), maxSliderPos - sr,
					sliderRadius * 2.0f, knobColour, outlineThickness, 3);
			}
			else if (style == Slider::TwoValueHorizontal || style == Slider::ThreeValueHorizontal)
			{
				const float sr = jmin(sliderRadius, height * 0.4f);

				drawGlassPointer(g, minSliderPos - sr,
					jmax(0.0f, y + height * 0.5f - sliderRadius * 2.0f),
					sliderRadius * 2.0f, knobColour, outlineThickness, 2);

				drawGlassPointer(g, maxSliderPos - sliderRadius,
					jmin(y + height - sliderRadius * 2.0f, y + height * 0.5f),
					sliderRadius * 2.0f, knobColour, outlineThickness, 4);
			}
		}
	}

private:
	Image backgroundTexture;
	Colour backgroundTextureBaseColour;
	ReferenceCountedObjectPtr<Typeface> typeface;
	ReferenceCountedObjectPtr<Typeface> typefaceIcons;
	ScopedPointer<Drawable> drawableImage;

};
}
#endif   // JUCE_MiddleLookAndFeel_H_INCLUDED
