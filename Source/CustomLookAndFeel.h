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

#ifndef JUCE_CUSTOMLOOKANDFEEL_H_INCLUDED
#define JUCE_CUSTOMLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AppColours.h"
#include "BinaryData.h"


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

class CustomLookAndFeel : public LookAndFeel_V3
{
public:
	CustomLookAndFeel()
	{

		const Colour accentColour(0xff2196F3);
        //const Colour accentColour(Colours::skyblue);
		setColour(mainAccentColourId, accentColour);
        setColour(lightAccentColourId, Colour(0xffFAFAFA));
		setColour(mainBackgroundColourId, Colours::white);
		LookAndFeel::setDefaultSansSerifTypefaceName("Quicksand");
		setColour(TextButton::textColourOnId, accentColour);
		setColour(TextButton::buttonColourId, findColour(mainBackgroundColourId));
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
		setColour(DocumentWindow::textColourId, Colours::white);
		setColour(ProgressBar::foregroundColourId, accentColour);
        setColour(ProgressBar::backgroundColourId, Colours::white);
        setColour(0x1005003, /*MidiKeyboardComponent::mouseOverKeyOverlayColourId*/ accentColour);
        setColour(0x1005004, /*MidiKeyboardComponent::keyDownOverlayColourId*/ accentColour);
        setColour(0x1005008, /*MidiKeyboardComponent::shadowColourId*/ Colours::black);
		setColour(MidiKeyboardComponent::upDownButtonBackgroundColourId, Colours::whitesmoke);
		setColour(MidiKeyboardComponent::upDownButtonArrowColourId, accentColour);
		setColour(TextEditor::highlightColourId, accentColour.withAlpha(0.2f));
		setColour(ComboBox::arrowColourId, accentColour);
		setColour(PopupMenu::highlightedBackgroundColourId, accentColour.withAlpha(0.2f));
		setColour(PopupMenu::highlightedTextColourId, accentColour.withAlpha(0.2f).contrasting());

        
		//setColour(ComboBox::backgroundColourId, accentColour);
		//setColour(ComboBox::buttonColourId, accentColour);
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

	void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
		bool isMouseOverButton, bool isButtonDown) override
	{
		Rectangle<int> buttonArea = button.getLocalBounds();

		const float mouseOver = isMouseOverButton ? 0.3f : 0.0f;

		g.setColour(findColour(mainAccentColourId).withAlpha(mouseOver));
		g.fillRoundedRectangle(buttonArea.toFloat().reduced(2.0f, 2.0f), 10.0f);
		g.setColour(findColour(mainAccentColourId));
		g.drawRoundedRectangle(buttonArea.toFloat().reduced(2.0f, 2.0f), 10.0f, 1.35f);
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
		dynamic_cast<CustomLookAndFeel&> (c.getLookAndFeel()).fillWithBackgroundTexture(g);
	}

	void drawDocumentWindowTitleBar(DocumentWindow& window, Graphics& g,
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
		g.setColour(findColour(mainAccentColourId));
		g.fillAll();

		Font font(h * 0.65f, Font::plain);
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
			shape.addLineSegment(Line<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.1f);
			shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 1.0f), 0.1f);

			ShapeButton* const b = new ShapeButton("close",
				Colour(0xffffffff),
				Colour(0xffffffff),
				Colour(0xffffffff));

			b->setShape(shape, true, true, true);
			return b;
		}
		else if (buttonType == DocumentWindow::minimiseButton)
		{
			shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.1f);

			DrawableButton* b = new DrawableButton("minimise", DrawableButton::ImageFitted);
			DrawablePath dp;
			dp.setPath(shape);
			dp.setFill(Colours::white);
			b->setImages(&dp);
			return b;
		}
		else if (buttonType == DocumentWindow::maximiseButton)
		{
			shape.addLineSegment(Line<float>(0.5f, 0.0f, 0.5f, 1.0f), 0.1f);
			shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.1f);

			DrawableButton* b = new DrawableButton("maximise", DrawableButton::ImageFitted);
			DrawablePath dp;
			dp.setPath(shape);
			dp.setFill(Colours::white);
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

		if (alert.getAlertType() != AlertWindow::NoIcon)
		{
			Path icon;
			uint32 colour;
			char character;

			if (alert.getAlertType() == AlertWindow::WarningIcon)
			{
				colour = 0x55ff5555;
				character = '!';

				icon.addTriangle(iconRect.getX() + iconRect.getWidth() * 0.5f, (float)iconRect.getY(),
					(float)iconRect.getRight(), (float)iconRect.getBottom(),
					(float)iconRect.getX(), (float)iconRect.getBottom());

				icon = icon.createPathWithRoundedCorners(5.0f);
			}
			else
			{
				colour = alert.getAlertType() == AlertWindow::InfoIcon ? (uint32)0x605555ff : (uint32)0x40b69900;
				character = alert.getAlertType() == AlertWindow::InfoIcon ? 'i' : '?';

				icon.addEllipse(iconRect.toFloat());
			}

			GlyphArrangement ga;
			ga.addFittedText(Font(iconRect.getHeight() * 0.9f, Font::bold),
				String::charToString((juce_wchar)(uint8)character),
				(float)iconRect.getX(), (float)iconRect.getY(),
				(float)iconRect.getWidth(), (float)iconRect.getHeight(),
				Justification::centred, false);
			ga.createPath(icon);

			icon.setUsingNonZeroWinding(false);
			g.setColour(findColour(mainBackgroundColourId));
			//g.fillPath (icon);

			iconSpaceUsed = 0;
		}

		g.setColour(alert.findColour(AlertWindow::textColourId));

		textLayout.draw(g, Rectangle<int>(textArea.getX() + iconSpaceUsed,
			textArea.getY(),
			textArea.getWidth() - iconSpaceUsed,
			textArea.getHeight()).toFloat());

		g.setColour(alert.findColour(AlertWindow::outlineColourId));
		g.drawRect(0, 0, alert.getWidth(), alert.getHeight());
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

	void drawTickBox(Graphics& g, Component& component,
		float x, float y, float w, float h,
		const bool ticked,
		const bool isEnabled,
		const bool isMouseOverButton,
		const bool isButtonDown) override
	{
		const float boxSize = w * 0.7f;
		drawGlassSphere(g, x, y + (h - boxSize) * 0.5f, boxSize,
			LookAndFeelHelpers::createBaseColour(component.findColour(mainAccentColourId)
				.withMultipliedAlpha(isEnabled ? 1.0f : 0.5f),
				true, isMouseOverButton, isButtonDown),
			isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.1f : 0.5f) : 0.3f);

		g.setColour(isEnabled ? findColour(mainAccentColourId).withAlpha(isButtonDown ? 0.3f : 0.1f)
			: Colours::lightgrey.withAlpha(0.1f));

		AffineTransform trans(AffineTransform::scale(w / 9.0f, h / 9.0f).translated(x, y));

		if (ticked)
		{
			Path tick;
			tick.startNewSubPath(1.5f, 3.0f);
			tick.lineTo(3.0f, 6.0f);
			tick.lineTo(6.0f, 0.0f);

			g.setColour(isEnabled ? findColour(mainAccentColourId) : Colours::grey);
			g.strokePath(tick, PathStrokeType(1.0f), trans);
		}
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
			g.setColour(findColour(mainAccentColourId));
			g.drawRoundedRectangle(buttonArea.toFloat().reduced(2.0f, 2.0f), 10.0f, 1.35f);
			//g.drawRect (0, 0, width, height, 2);
		}
		else
		{
			g.setColour(box.findColour(ComboBox::outlineColourId));
			g.drawRoundedRectangle(buttonArea.toFloat().reduced(2.0f, 2.0f), 10.0f, 1.35f);
			//g.drawRect (0, 0, width, height);
		}

		const float arrowX = 0.25f;
		const float arrowH = 0.2f;

		Path p;

		p.startNewSubPath(buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f);
		p.lineTo(buttonX + buttonW * 0.5f, buttonY + buttonH * (0.45f + arrowH));
		p.lineTo(buttonX + buttonW * arrowX, buttonY + buttonH * 0.45f);

		g.setColour(box.findColour(ComboBox::arrowColourId).withMultipliedAlpha(box.isEnabled() ? 1.0f : 0.3f));
		g.strokePath(p, PathStrokeType(1.35f));
	}
    
    void positionComboBoxText (ComboBox& box, Label& label) override
    {
        label.setBounds (1, 1,
                         box.getWidth() + 3 - box.getHeight(),
                         box.getHeight() - 2);
        
        label.setFont (getComboBoxFont (box));
        label.setJustificationType(Justification::centred);
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

			Font font(getPopupMenuFont());

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
				const Path tick(getTickShape(1.0f));
				g.setColour(findColour(mainAccentColourId));
				g.strokePath(tick, PathStrokeType(1.0f), tick.getTransformToScaleToFit(iconArea.reduced(6), true));
				g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
				g.fillRect(r);
				g.fillRect(iconArea);

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
    void drawProgressBar (Graphics& g, ProgressBar& progressBar,
                          int width, int height,
                          double progress, const String& textToShow) override
    {
        const Colour background (progressBar.findColour (ProgressBar::backgroundColourId));
        const Colour foreground (progressBar.findColour (ProgressBar::foregroundColourId));
        
        g.fillAll (background);
        
        if (progress >= 0.0f && progress < 1.0f)
        {
            drawGlassLozenge (g, 1.0f, 1.0f,
                              (float) jlimit (0.0, width - 2.0, progress * (width - 2.0)),
                              (float) (height - 2),
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
    void drawGlassLozenge (Graphics& g,
                           const float x, const float y, const float width, const float height,
                           const Colour& colour, const float outlineThickness, const float cornerSize,
                           const bool flatOnLeft,
                           const bool flatOnRight,
                           const bool flatOnTop,
                           const bool flatOnBottom) noexcept
    {
        if (width <= outlineThickness || height <= outlineThickness)
            return;
        
        const int intX = (int) x;
        const int intY = (int) y;
        const int intW = (int) width;
        const int intH = (int) height;
        
        const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
        const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
        const int intEdge = (int) edgeBlurRadius;
        
        Path outline;
        outline.addRoundedRectangle (x, y, width, height, cs, cs,
                                     ! (flatOnLeft || flatOnTop),
                                     ! (flatOnRight || flatOnTop),
                                     ! (flatOnLeft || flatOnBottom),
                                     ! (flatOnRight || flatOnBottom));
        
        {
            g.setColour (colour);
            
            g.fillPath (outline);
        }
        
        if (! (flatOnLeft || flatOnTop || flatOnBottom))
        {
            g.saveState();
            g.setColour (colour);
            g.reduceClipRegion (intX, intY, intEdge, intH);
            g.fillPath (outline);
            g.restoreState();
        }
        
        if (! (flatOnRight || flatOnTop || flatOnBottom))
        {
            g.saveState();
            g.setColour (colour);
            g.reduceClipRegion (intX + intW - intEdge, intY, 2 + intEdge, intH);
            g.fillPath (outline);
            g.restoreState();
        }
        
        {
            const float leftIndent = flatOnTop || flatOnLeft ? 0.0f : cs * 0.4f;
            const float rightIndent = flatOnTop || flatOnRight ? 0.0f : cs * 0.4f;
            
            Path highlight;
            highlight.addRoundedRectangle (x + leftIndent,
                                           y + cs * 0.1f,
                                           width - (leftIndent + rightIndent),
                                           height * 0.4f,
                                           cs * 0.4f,
                                           cs * 0.4f,
                                           ! (flatOnLeft || flatOnTop),
                                           ! (flatOnRight || flatOnTop),
                                           ! (flatOnLeft || flatOnBottom),
                                           ! (flatOnRight || flatOnBottom));
            
            g.setColour (colour);
            g.fillPath (highlight);
        }
        
        g.setColour (colour);
        g.strokePath (outline, PathStrokeType (outlineThickness));
        }

private:
	Image backgroundTexture;
	Colour backgroundTextureBaseColour;
};




#endif   // JUCE_CUSTOMLOOKANDFEEL_H_INCLUDED
