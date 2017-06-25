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

#include "CustomMidiKeyboardComponent.h"

class CustomMidiKeyboardUpDownButton : public Button
{
public:
	CustomMidiKeyboardUpDownButton(CustomMidiKeyboardComponent& comp, const int d)
		: Button(String()), owner(comp), delta(d)
	{
	}

	void clicked() override
	{
		int note = owner.getLowestVisibleKey();
		int octave = owner.getKeyPressBaseOctave();

		if (delta < 0) {
			note = (note - 1) / 12;
			octave = octave - 1;
		}
		else {
			note = note / 12 + 1;
			octave = octave + 1;
		}

		owner.setLowestVisibleKey(note * 12);
		// Andrew edit
		owner.setKeyPressBaseOctave(octave);
	}

	void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
	{
		owner.drawUpDownButton(g, getWidth(), getHeight(),
			isMouseOverButton, isButtonDown,
			delta > 0);
	}

private:
	CustomMidiKeyboardComponent& owner;
	const int delta;

	JUCE_DECLARE_NON_COPYABLE(CustomMidiKeyboardUpDownButton)
};

//==============================================================================
CustomMidiKeyboardComponent::CustomMidiKeyboardComponent(MidiKeyboardState& s, Orientation o)
	: state(s),
	blackNoteLengthRatio(0.7f),
	xOffset(0),
	keyWidth(16.0f),
	orientation(o),
	midiChannel(1),
	midiInChannelMask(0xffff),
	velocity(1.0f),
	lowVelocity(50),
	highVelocity(90),
	shouldCheckState(false),
	rangeStart(0),
	rangeEnd(127),
	firstKey(12 * 4.0f),
	canScroll(true),
	useMousePositionForVelocity(true),
	shouldCheckMousePos(false),
	keyMappingOctave(4),
	octaveNumForMiddleC(3)
{
	addChildComponent(scrollDown = new CustomMidiKeyboardUpDownButton(*this, -1));
	addChildComponent(scrollUp = new CustomMidiKeyboardUpDownButton(*this, 1));

	// initialise with a default set of qwerty key-mappings..
	const char* const keymap = "q1w2er3t4y5ui6o7pa1s2df3g4h5jk6l7;";


	//for (int i = 0; keymap[i] != 0; ++i)
	//setKeyPressForNote (KeyPress (keymap[i], 0, 0), i);

	setKeyPressForNote(KeyPress('q', 0, 0), 0);
	setKeyPressForNote(KeyPress('1', 0, 0), 1);
	setKeyPressForNote(KeyPress('w', 0, 0), 2);
	setKeyPressForNote(KeyPress('2', 0, 0), 3);
	setKeyPressForNote(KeyPress('e', 0, 0), 4);
	setKeyPressForNote(KeyPress('r', 0, 0), 5);
	setKeyPressForNote(KeyPress('3', 0, 0), 6);
	setKeyPressForNote(KeyPress('t', 0, 0), 7);
	setKeyPressForNote(KeyPress('4', 0, 0), 8);
	setKeyPressForNote(KeyPress('y', 0, 0), 9);
	setKeyPressForNote(KeyPress('5', 0, 0), 10);
	setKeyPressForNote(KeyPress('u', 0, 0), 11);
	setKeyPressForNote(KeyPress('i', 0, 0), 12);
	setKeyPressForNote(KeyPress('6', 0, 0), 13);
	setKeyPressForNote(KeyPress('o', 0, 0), 14);
	setKeyPressForNote(KeyPress('7', 0, 0), 15);
	setKeyPressForNote(KeyPress('p', 0, 0), 16);
	setKeyPressForNote(KeyPress('a', 0, 0), 17);
	setKeyPressForNote(KeyPress('8', 0, 0), 18);
	setKeyPressForNote(KeyPress('s', 0, 0), 19);
	setKeyPressForNote(KeyPress('9', 0, 0), 20);
	setKeyPressForNote(KeyPress('d', 0, 0), 21);
	setKeyPressForNote(KeyPress('0', 0, 0), 22);
	setKeyPressForNote(KeyPress('f', 0, 0), 23);
	setKeyPressForNote(KeyPress('g', 0, 0), 24);
	setKeyPressForNote(KeyPress('h', 0, 0), 26);
	setKeyPressForNote(KeyPress('j', 0, 0), 28);
	setKeyPressForNote(KeyPress('k', 0, 0), 29);
	setKeyPressForNote(KeyPress('l', 0, 0), 31);
	setKeyPressForNote(KeyPress('z', 0, 0), 33);
	setKeyPressForNote(KeyPress('x', 0, 0), 35);
	setKeyPressForNote(KeyPress('c', 0, 0), 36);
	setKeyPressForNote(KeyPress('v', 0, 0), 38);
	setKeyPressForNote(KeyPress('b', 0, 0), 40);
	setKeyPressForNote(KeyPress('n', 0, 0), 41);
	setKeyPressForNote(KeyPress('m', 0, 0), 43);

	mouseOverNotes.insertMultiple(0, -1, 32);
	mouseDownNotes.insertMultiple(0, -1, 32);

	colourChanged();
	setWantsKeyboardFocus(true);

	state.addListener(this);

	startTimerHz(20);
}

CustomMidiKeyboardComponent::~CustomMidiKeyboardComponent()
{
	state.removeListener(this);
}

//==============================================================================
void CustomMidiKeyboardComponent::setKeyWidth(const float widthInPixels)
{
	jassert(widthInPixels > 0);

	if (keyWidth != widthInPixels) // Prevent infinite recursion if the width is being computed in a 'resized()' call-back
	{
		keyWidth = widthInPixels;
		resized();
	}
}

void CustomMidiKeyboardComponent::setOrientation(const Orientation newOrientation)
{
	if (orientation != newOrientation)
	{
		orientation = newOrientation;
		resized();
	}
}

void CustomMidiKeyboardComponent::setAvailableRange(const int lowestNote,
	const int highestNote)
{
	jassert(lowestNote >= 0 && lowestNote <= 127);
	jassert(highestNote >= 0 && highestNote <= 127);
	jassert(lowestNote <= highestNote);

	if (rangeStart != lowestNote || rangeEnd != highestNote)
	{
		rangeStart = jlimit(0, 127, lowestNote);
		rangeEnd = jlimit(0, 127, highestNote);
		firstKey = jlimit((float)rangeStart, (float)rangeEnd, firstKey);
		resized();
	}
}

void CustomMidiKeyboardComponent::setLowestVisibleKey(int noteNumber)
{
	setLowestVisibleKeyFloat((float)noteNumber);
}

void CustomMidiKeyboardComponent::setLowestVisibleKeyFloat(float noteNumber)
{
	noteNumber = jlimit((float)rangeStart, (float)rangeEnd, noteNumber);

	if (noteNumber != firstKey)
	{
		const bool hasMoved = (((int)firstKey) != (int)noteNumber);
		firstKey = noteNumber;

		if (hasMoved)
			sendChangeMessage();

		resized();
	}
}

void CustomMidiKeyboardComponent::setScrollButtonsVisible(const bool newCanScroll)
{
	if (canScroll != newCanScroll)
	{
		canScroll = newCanScroll;
		resized();
	}
}

void CustomMidiKeyboardComponent::colourChanged()
{
	setOpaque(findColour(whiteNoteColourId).isOpaque());
	repaint();
}

//==============================================================================
void CustomMidiKeyboardComponent::setMidiChannel(const int midiChannelNumber)
{
	jassert(midiChannelNumber > 0 && midiChannelNumber <= 16);

	if (midiChannel != midiChannelNumber)
	{
		resetAnyKeysInUse();
		midiChannel = jlimit(1, 16, midiChannelNumber);
	}
}

void CustomMidiKeyboardComponent::setMidiChannelsToDisplay(const int midiChannelMask)
{
	midiInChannelMask = midiChannelMask;
	shouldCheckState = true;
}

void CustomMidiKeyboardComponent::setVelocity(const float v, const bool useMousePosition)
{
	velocity = jlimit(0.0f, 1.0f, v);
	useMousePositionForVelocity = useMousePosition;
}

//==============================================================================
void CustomMidiKeyboardComponent::getKeyPosition(int midiNoteNumber, const float keyWidth_, int& x, int& w) const
{
	jassert(midiNoteNumber >= 0 && midiNoteNumber < 128);

	static const float blackNoteWidth = 0.7f;

	static const float notePos[] = { 0.0f, 1 - blackNoteWidth * 0.6f,
		1.0f, 2 - blackNoteWidth * 0.4f,
		2.0f,
		3.0f, 4 - blackNoteWidth * 0.7f,
		4.0f, 5 - blackNoteWidth * 0.5f,
		5.0f, 6 - blackNoteWidth * 0.3f,
		6.0f };

	const int octave = midiNoteNumber / 12;
	const int note = midiNoteNumber % 12;

	x = roundToInt(octave * 7.0f * keyWidth_ + notePos[note] * keyWidth_);
	w = roundToInt(MidiMessage::isMidiNoteBlack(note) ? blackNoteWidth * keyWidth_ : keyWidth_);
}

void CustomMidiKeyboardComponent::getKeyPos(int midiNoteNumber, int& x, int& w) const
{
	getKeyPosition(midiNoteNumber, keyWidth, x, w);

	int rx, rw;
	getKeyPosition(rangeStart, keyWidth, rx, rw);

	x -= xOffset + rx;
}

Rectangle<int> CustomMidiKeyboardComponent::getRectangleForKey(const int note) const
{
	jassert(note >= rangeStart && note <= rangeEnd);

	int x, w;
	getKeyPos(note, x, w);

	if (MidiMessage::isMidiNoteBlack(note))
	{
		const int blackNoteLength = getBlackNoteLength();

		switch (orientation)
		{
		case horizontalKeyboard:            return Rectangle<int>(x, 0, w, blackNoteLength);
		case verticalKeyboardFacingLeft:    return Rectangle<int>(getWidth() - blackNoteLength, x, blackNoteLength, w);
		case verticalKeyboardFacingRight:   return Rectangle<int>(0, getHeight() - x - w, blackNoteLength, w);
		default:                            jassertfalse; break;
		}
	}
	else
	{
		switch (orientation)
		{
		case horizontalKeyboard:            return Rectangle<int>(x, 0, w, getHeight());
		case verticalKeyboardFacingLeft:    return Rectangle<int>(0, x, getWidth(), w);
		case verticalKeyboardFacingRight:   return Rectangle<int>(0, getHeight() - x - w, getWidth(), w);
		default:                            jassertfalse; break;
		}
	}

	return Rectangle<int>();
}

int CustomMidiKeyboardComponent::getKeyStartPosition(const int midiNoteNumber) const
{
	int x, w;
	getKeyPos(midiNoteNumber, x, w);
	return x;
}

int CustomMidiKeyboardComponent::getTotalKeyboardWidth() const noexcept
{
	int x, w;
	getKeyPos(rangeEnd, x, w);
	return x + w;
}

int CustomMidiKeyboardComponent::getNoteAtPosition(Point<int> p)
{
	float v;
	return xyToNote(p, v);
}

const uint8 CustomMidiKeyboardComponent::whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11 };
const uint8 CustomMidiKeyboardComponent::blackNotes[] = { 1, 3, 6, 8, 10 };

int CustomMidiKeyboardComponent::xyToNote(Point<int> pos, float& mousePositionVelocity)
{
	if (!reallyContains(pos, false))
		return -1;

	Point<int> p(pos);

	if (orientation != horizontalKeyboard)
	{
		p = Point<int>(p.y, p.x);

		if (orientation == verticalKeyboardFacingLeft)
			p = Point<int>(p.x, getWidth() - p.y);
		else
			p = Point<int>(getHeight() - p.x, p.y);
	}

	return remappedXYToNote(p + Point<int>(xOffset, 0), mousePositionVelocity);
}

int CustomMidiKeyboardComponent::remappedXYToNote(Point<int> pos, float& mousePositionVelocity) const
{
	const int blackNoteLength = getBlackNoteLength();

	if (pos.getY() < blackNoteLength)
	{
		for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
		{
			for (int i = 0; i < 5; ++i)
			{
				const int note = octaveStart + blackNotes[i];

				if (note >= rangeStart && note <= rangeEnd)
				{
					int kx, kw;
					getKeyPos(note, kx, kw);
					kx += xOffset;

					if (pos.x >= kx && pos.x < kx + kw)
					{
						mousePositionVelocity = pos.y / (float)blackNoteLength;
						return note;
					}
				}
			}
		}
	}

	for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
	{
		for (int i = 0; i < 7; ++i)
		{
			const int note = octaveStart + whiteNotes[i];

			if (note >= rangeStart && note <= rangeEnd)
			{
				int kx, kw;
				getKeyPos(note, kx, kw);
				kx += xOffset;

				if (pos.x >= kx && pos.x < kx + kw)
				{
					const int whiteNoteLength = (orientation == horizontalKeyboard) ? getHeight() : getWidth();
					mousePositionVelocity = pos.y / (float)whiteNoteLength;
					return note;
				}
			}
		}
	}

	mousePositionVelocity = 0;
	return -1;
}

//==============================================================================
void CustomMidiKeyboardComponent::repaintNote(const int noteNum)
{
	if (noteNum >= rangeStart && noteNum <= rangeEnd)
		repaint(getRectangleForKey(noteNum));
}

void CustomMidiKeyboardComponent::paint(Graphics& g)
{
	g.fillAll(findColour(whiteNoteColourId));

	const Colour lineColour(findColour(keySeparatorLineColourId));
	const Colour textColour(findColour(textLabelColourId));

	for (int octave = 0; octave < 128; octave += 12)
	{
		for (int white = 0; white < 7; ++white)
		{
			const int noteNum = octave + whiteNotes[white];

			if (noteNum >= rangeStart && noteNum <= rangeEnd)
			{
				Rectangle<int> pos = getRectangleForKey(noteNum);

				drawWhiteNote(noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
					state.isNoteOnForChannels(midiInChannelMask, noteNum),
					mouseOverNotes.contains(noteNum), lineColour, textColour);
			}
		}
	}

	float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
	const int width = getWidth();
	const int height = getHeight();

	if (orientation == verticalKeyboardFacingLeft)
	{
		x1 = width - 1.0f;
		x2 = width - 5.0f;
	}
	else if (orientation == verticalKeyboardFacingRight)
		x2 = 5.0f;
	else
		y2 = 5.0f;

	int x, w;
	getKeyPos(rangeEnd, x, w);
	x += w;

	const Colour shadowCol(findColour(shadowColourId));

	if (!shadowCol.isTransparent())
	{
		g.setColour(shadowCol);

		switch (orientation)
		{
		case horizontalKeyboard:            g.fillRect(0, 0, x, 1); break;
		case verticalKeyboardFacingLeft:    g.fillRect(width - 5, 0, 5, x); break;
		case verticalKeyboardFacingRight:   g.fillRect(0, 0, 5, x); break;
		default: break;
		}
	}

	if (!lineColour.isTransparent())
	{
		g.setColour(lineColour);

		switch (orientation)
		{
		case horizontalKeyboard:            g.fillRect(0, height - 1, x, 1); break;
		case verticalKeyboardFacingLeft:    g.fillRect(0, 0, 1, x); break;
		case verticalKeyboardFacingRight:   g.fillRect(width - 1, 0, 1, x); break;
		default: break;
		}
	}

	const Colour blackNoteColour(findColour(blackNoteColourId));

	for (int octave = 0; octave < 128; octave += 12)
	{
		for (int black = 0; black < 5; ++black)
		{
			const int noteNum = octave + blackNotes[black];

			if (noteNum >= rangeStart && noteNum <= rangeEnd)
			{
				Rectangle<int> pos = getRectangleForKey(noteNum);

				drawBlackNote(noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
					state.isNoteOnForChannels(midiInChannelMask, noteNum),
					mouseOverNotes.contains(noteNum), blackNoteColour);
			}
		}
	}
}

void CustomMidiKeyboardComponent::drawWhiteNote(int midiNoteNumber,
	Graphics& g, int x, int y, int w, int h,
	bool isDown, bool isOver,
	const Colour& lineColour,
	const Colour& textColour)
{
	Colour c(Colours::transparentWhite);

	if (isDown)  c = findColour(keyDownOverlayColourId);
	if (isOver)  c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

	g.setColour(c);
	g.fillRect(x, y, w, h);

	const String text(getWhiteNoteText(midiNoteNumber));

	if (text.isNotEmpty())
	{
		const float fontHeight = jmin(12.0f, keyWidth * 0.9f);

		g.setColour(textColour);
        
        ReferenceCountedObjectPtr<Typeface> typeface;
        typeface = Typeface::createSystemTypefaceFor(BinaryData::quicksand_regular_ttf, BinaryData::quicksand_regular_ttf_Size);
        Font font(typeface);
        font.setHeight(fontHeight);
		g.setFont(font.withHorizontalScale(0.8f));

		switch (orientation)
		{
		case horizontalKeyboard:            g.drawText(text, x + 1, y, w - 1, h - 2, Justification::centredBottom, false); break;
		case verticalKeyboardFacingLeft:    g.drawText(text, x + 2, y + 2, w - 4, h - 4, Justification::centredLeft, false); break;
		case verticalKeyboardFacingRight:   g.drawText(text, x + 2, y + 2, w - 4, h - 4, Justification::centredRight, false); break;
		default: break;
		}
	}

	if (!lineColour.isTransparent())
	{
		g.setColour(lineColour);

		switch (orientation)
		{
		case horizontalKeyboard:            g.fillRect(x, y, 1, h); break;
		case verticalKeyboardFacingLeft:    g.fillRect(x, y, w, 1); break;
		case verticalKeyboardFacingRight:   g.fillRect(x, y + h - 1, w, 1); break;
		default: break;
		}

		if (midiNoteNumber == rangeEnd)
		{
			switch (orientation)
			{
			case horizontalKeyboard:            g.fillRect(x + w, y, 1, h); break;
			case verticalKeyboardFacingLeft:    g.fillRect(x, y + h, w, 1); break;
			case verticalKeyboardFacingRight:   g.fillRect(x, y - 1, w, 1); break;
			default: break;
			}
		}
	}
}

void CustomMidiKeyboardComponent::drawBlackNote(int /*midiNoteNumber*/,
	Graphics& g, int x, int y, int w, int h,
	bool isDown, bool isOver,
	const Colour& noteFillColour)
{
	Colour c(noteFillColour);

	if (isDown)  c = c.overlaidWith(findColour(keyDownOverlayColourId));
	if (isOver)  c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

	g.setColour(c);
	g.fillRect(x, y, w, h);

	if (isDown)
	{
		g.setColour(noteFillColour);
		g.drawRect(x, y, w, h);
	}
	else
	{
		//g.setColour(c.brighter());
		const int xIndent = jmax(1, jmin(w, h) / 8);

		switch (orientation)
		{
		case horizontalKeyboard:            g.fillRect(x + xIndent, y, w - xIndent * 2, 7 * h / 8); break;
		case verticalKeyboardFacingLeft:    g.fillRect(x + w / 8, y + xIndent, w - w / 8, h - xIndent * 2); break;
		case verticalKeyboardFacingRight:   g.fillRect(x, y + xIndent, 7 * w / 8, h - xIndent * 2); break;
		default: break;
		}
	}
}

void CustomMidiKeyboardComponent::setOctaveForMiddleC(const int octaveNum)
{
	octaveNumForMiddleC = octaveNum;
	repaint();
}

String CustomMidiKeyboardComponent::getWhiteNoteText(const int midiNoteNumber)
{
	if (midiNoteNumber % 12 == 0)
		return MidiMessage::getMidiNoteName(midiNoteNumber, true, true, octaveNumForMiddleC);

	return String();
}

void CustomMidiKeyboardComponent::drawUpDownButton(Graphics& g, int w, int h,
	const bool mouseOver,
	const bool buttonDown,
	const bool movesOctavesUp)
{
	g.fillAll(findColour(upDownButtonBackgroundColourId));

	float angle;

	switch (orientation)
	{
	case horizontalKeyboard:            angle = movesOctavesUp ? 0.5f : 0.0f;  break;
	case verticalKeyboardFacingLeft:    angle = movesOctavesUp ? 0.25f : 0.75f; break;
	case verticalKeyboardFacingRight:   angle = movesOctavesUp ? 0.75f : 0.25f; break;
	default:                            jassertfalse; angle = 0; break;
	}

//	Path path;
//	//path.addTriangle (0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
//	path.startNewSubPath(0.0f, 0.0f);
//	path.lineTo(0.0f, 1.0f);
//	path.lineTo(1.0f, 0.5f);
    
    Path path(Peels::MiddleLookAndFeel().getPathFromChar(0xE314));

	path.applyTransform(AffineTransform::rotation(float_Pi * 2.0f * angle, 0.5f, 0.5f));

	g.setColour(findColour(upDownButtonArrowColourId)
		.withAlpha(buttonDown ? 1.0f : (mouseOver ? 0.8f : 0.6f)));

	g.fillPath (path, path.getTransformToScaleToFit (1.0f, 1.0f, w - 2.0f, h - 2.0f, true));
    
	//g.strokePath(path, PathStrokeType(1.35f), path.getTransformToScaleToFit(1.0f, 1.0f, w - 3.0f, h - 3.0f, true));
}

void CustomMidiKeyboardComponent::setBlackNoteLengthProportion(float ratio) noexcept
{
	jassert(ratio >= 0.0f && ratio <= 1.0f);
	if (blackNoteLengthRatio != ratio)
	{
		blackNoteLengthRatio = ratio;
		resized();
	}
}

int CustomMidiKeyboardComponent::getBlackNoteLength() const noexcept
{
	const int whiteNoteLength = orientation == horizontalKeyboard ? getHeight() : getWidth();

	return roundToInt(whiteNoteLength * blackNoteLengthRatio);
}

void CustomMidiKeyboardComponent::resized()
{
	int w = getWidth();
	int h = getHeight();

	if (w > 0 && h > 0)
	{
		if (orientation != horizontalKeyboard)
			std::swap(w, h);

		int kx2, kw2;
		getKeyPos(rangeEnd, kx2, kw2);

		kx2 += kw2;

		if ((int)firstKey != rangeStart)
		{
			int kx1, kw1;
			getKeyPos(rangeStart, kx1, kw1);

			if (kx2 - kx1 <= w)
			{
				firstKey = (float)rangeStart;
				sendChangeMessage();
				repaint();
			}
		}

		scrollDown->setVisible(canScroll/* && firstKey > (float) rangeStart*/);

		xOffset = 0;

		if (canScroll)
		{
			const int scrollButtonW = jmin(12, w / 2);
			Rectangle<int> r(getLocalBounds());

			if (orientation == horizontalKeyboard)
			{
				scrollDown->setBounds(r.removeFromLeft(scrollButtonW));
				scrollUp->setBounds(r.removeFromRight(scrollButtonW));
			}
			else if (orientation == verticalKeyboardFacingLeft)
			{
				scrollDown->setBounds(r.removeFromTop(scrollButtonW));
				scrollUp->setBounds(r.removeFromBottom(scrollButtonW));
			}
			else
			{
				scrollDown->setBounds(r.removeFromBottom(scrollButtonW));
				scrollUp->setBounds(r.removeFromTop(scrollButtonW));
			}

			int endOfLastKey, kw;
			getKeyPos(rangeEnd, endOfLastKey, kw);
			endOfLastKey += kw;

			float mousePositionVelocity;
			const int spaceAvailable = w;
			const int lastStartKey = remappedXYToNote(Point<int>(endOfLastKey - spaceAvailable, 0), mousePositionVelocity) + 1;

			if (lastStartKey >= 0 && ((int)firstKey) > lastStartKey)
			{
				firstKey = (float)jlimit(rangeStart, rangeEnd, lastStartKey);
				sendChangeMessage();
			}

			int newOffset = 0;
			getKeyPos((int)firstKey, newOffset, kw);
			xOffset = newOffset;
		}
		else
		{
			firstKey = (float)rangeStart;
		}

		getKeyPos(rangeEnd, kx2, kw2);
		scrollUp->setVisible(canScroll/*&& kx2 > w*/);
		repaint();
	}
}

//==============================================================================
void CustomMidiKeyboardComponent::handleNoteOn(MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
	shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

void CustomMidiKeyboardComponent::handleNoteOff(MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
	shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

//==============================================================================
void CustomMidiKeyboardComponent::resetAnyKeysInUse()
{
	if (!keysPressed.isZero())
	{
		for (int i = 128; --i >= 0;)
			if (keysPressed[i])
				state.noteOff(midiChannel, i, 0.0f);

		keysPressed.clear();
	}

	for (int i = mouseDownNotes.size(); --i >= 0;)
	{
		const int noteDown = mouseDownNotes.getUnchecked(i);

		if (noteDown >= 0)
		{
			state.noteOff(midiChannel, noteDown, 0.0f);
			mouseDownNotes.set(i, -1);
		}

		mouseOverNotes.set(i, -1);
	}
}

void CustomMidiKeyboardComponent::updateNoteUnderMouse(const MouseEvent& e, bool isDown)
{
	updateNoteUnderMouse(e.getEventRelativeTo(this).getPosition(), isDown, e.source.getIndex());
}

void CustomMidiKeyboardComponent::updateNoteUnderMouse(Point<int> pos, bool isDown, int fingerNum)
{
	float mousePositionVelocity = 0.0f;
	const int newNote = xyToNote(pos, mousePositionVelocity);
	const int oldNote = mouseOverNotes.getUnchecked(fingerNum);
	const int oldNoteDown = mouseDownNotes.getUnchecked(fingerNum);
	const float eventVelocity = useMousePositionForVelocity ? mousePositionVelocity * velocity : 1.0f;

	if (oldNote != newNote)
	{
		repaintNote(oldNote);
		repaintNote(newNote);
		mouseOverNotes.set(fingerNum, newNote);
	}

	if (isDown)
	{
		if (newNote != oldNoteDown)
		{
			if (oldNoteDown >= 0)
			{
				mouseDownNotes.set(fingerNum, -1);

				if (!mouseDownNotes.contains(oldNoteDown))
					state.noteOff(midiChannel, oldNoteDown, eventVelocity);
			}

			if (newNote >= 0 && !mouseDownNotes.contains(newNote))
			{
				state.noteOn(midiChannel, newNote, eventVelocity);
				mouseDownNotes.set(fingerNum, newNote);
			}
		}
	}
	else if (oldNoteDown >= 0)
	{
		mouseDownNotes.set(fingerNum, -1);

		if (!mouseDownNotes.contains(oldNoteDown))
			state.noteOff(midiChannel, oldNoteDown, eventVelocity);
	}
}

void CustomMidiKeyboardComponent::mouseMove(const MouseEvent& e)
{
	updateNoteUnderMouse(e, false);
	shouldCheckMousePos = false;
}

void CustomMidiKeyboardComponent::mouseDrag(const MouseEvent& e)
{
	float mousePositionVelocity;
	const int newNote = xyToNote(e.getPosition(), mousePositionVelocity);

	if (newNote >= 0)
		mouseDraggedToKey(newNote, e);

	updateNoteUnderMouse(e, true);
}

bool CustomMidiKeyboardComponent::mouseDownOnKey(int, const MouseEvent&) { return true; }
void CustomMidiKeyboardComponent::mouseDraggedToKey(int, const MouseEvent&) {}
void CustomMidiKeyboardComponent::mouseUpOnKey(int, const MouseEvent&) {}

void CustomMidiKeyboardComponent::mouseDown(const MouseEvent& e)
{
	float mousePositionVelocity;
	const int newNote = xyToNote(e.getPosition(), mousePositionVelocity);

	if (newNote >= 0 && mouseDownOnKey(newNote, e))
	{
		updateNoteUnderMouse(e, true);
		shouldCheckMousePos = true;
	}
}

void CustomMidiKeyboardComponent::mouseUp(const MouseEvent& e)
{
	updateNoteUnderMouse(e, false);
	shouldCheckMousePos = false;

	float mousePositionVelocity;
	const int note = xyToNote(e.getPosition(), mousePositionVelocity);
	if (note >= 0)
		mouseUpOnKey(note, e);
}

void CustomMidiKeyboardComponent::mouseEnter(const MouseEvent& e)
{
	updateNoteUnderMouse(e, false);
}

void CustomMidiKeyboardComponent::mouseExit(const MouseEvent& e)
{
	updateNoteUnderMouse(e, false);
}

void CustomMidiKeyboardComponent::mouseWheelMove(const MouseEvent&, const MouseWheelDetails& wheel)
{
	const float amount = (orientation == horizontalKeyboard && wheel.deltaX != 0)
		? wheel.deltaX : (orientation == verticalKeyboardFacingLeft ? wheel.deltaY
			: -wheel.deltaY);

	setLowestVisibleKeyFloat(firstKey - amount * keyWidth);
}

void CustomMidiKeyboardComponent::timerCallback()
{
	if (shouldCheckState)
	{
		shouldCheckState = false;

		for (int i = rangeStart; i <= rangeEnd; ++i)
		{
			if (keysCurrentlyDrawnDown[i] != state.isNoteOnForChannels(midiInChannelMask, i))
			{
				keysCurrentlyDrawnDown.setBit(i, state.isNoteOnForChannels(midiInChannelMask, i));
				repaintNote(i);
			}
		}
	}

	if (shouldCheckMousePos)
	{
		const Array<MouseInputSource>& mouseSources = Desktop::getInstance().getMouseSources();

		for (MouseInputSource* mi = mouseSources.begin(), *const e = mouseSources.end(); mi != e; ++mi)
			if (mi->getComponentUnderMouse() == this || isParentOf(mi->getComponentUnderMouse()))
				updateNoteUnderMouse(getLocalPoint(nullptr, mi->getScreenPosition()).roundToInt(), mi->isDragging(), mi->getIndex());
	}
}

//==============================================================================
void CustomMidiKeyboardComponent::clearKeyMappings()
{
	resetAnyKeysInUse();
	keyPressNotes.clear();
	keyPresses.clear();
}

void CustomMidiKeyboardComponent::setKeyPressForNote(const KeyPress& key, int midiNoteOffsetFromC)
{
	removeKeyPressForNote(midiNoteOffsetFromC);

	keyPressNotes.add(midiNoteOffsetFromC);
	keyPresses.add(key);
}

void CustomMidiKeyboardComponent::removeKeyPressForNote(const int midiNoteOffsetFromC)
{
	for (int i = keyPressNotes.size(); --i >= 0;)
	{
		if (keyPressNotes.getUnchecked(i) == midiNoteOffsetFromC)
		{
			keyPressNotes.remove(i);
			keyPresses.remove(i);
		}
	}
}

void CustomMidiKeyboardComponent::setKeyPressBaseOctave(const int newOctaveNumber)
{
	//jassert (newOctaveNumber >= 0 && newOctaveNumber <= 10);
	if (newOctaveNumber >= 0 && newOctaveNumber <= 10)
		keyMappingOctave = newOctaveNumber;
}

int CustomMidiKeyboardComponent::getKeyPressBaseOctave()
{
	return keyMappingOctave;
}

bool CustomMidiKeyboardComponent::keyStateChanged(const bool /*isKeyDown*/)
{
	bool keyPressUsed = false;

	for (int i = keyPresses.size(); --i >= 0;)
	{
		const int note = 12 * keyMappingOctave + keyPressNotes.getUnchecked(i);

		if (keyPresses.getReference(i).isCurrentlyDown())
		{
			if (!keysPressed[note])
			{
				if (midiChannel > 0 && midiChannel <= 16 && isPositiveAndBelow(note, (int)128))
				{
					int randVelocity = random.nextInt({ lowVelocity, highVelocity });
					keysPressed.setBit(note);
					state.noteOn(midiChannel, note, (float)randVelocity / 128);
					keyPressUsed = true;
				}
			}
		}
		else
		{
			if (keysPressed[note])
			{
				if (midiChannel > 0 && midiChannel <= 16 && isPositiveAndBelow(note, (int)128))
				{
					keysPressed.clearBit(note);
					state.noteOff(midiChannel, note, 0.0f);
					keyPressUsed = true;
				}
			}
		}
	}

	return keyPressUsed;
}

bool CustomMidiKeyboardComponent::keyPressed(const KeyPress& key)
{
	return keyPresses.contains(key);
}

void CustomMidiKeyboardComponent::focusLost(FocusChangeType)
{
	resetAnyKeysInUse();
}
