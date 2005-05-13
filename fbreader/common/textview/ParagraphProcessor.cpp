/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cctype>
#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "ParagraphCursor.h"
#include "Word.h"

#include "../model/Paragraph.h"

ParagraphCursor::ParagraphProcessor::ParagraphProcessor(const Paragraph &paragraph, const std::vector<TextMark> &marks, int paragraphNumber, const shared_ptr<TextElementVector> &elements) : myParagraph(paragraph), myElements(elements) {
	myFirstMark = std::lower_bound(marks.begin(), marks.end(), TextMark(paragraphNumber, 0, 0));
	myLastMark = myFirstMark;
	for (; (myLastMark != marks.end()) && (myLastMark->ParagraphNumber == paragraphNumber); myLastMark++);
	myWordCounter = 0;
	myOffset = 0;
}

ParagraphCursor::ParagraphProcessor::~ParagraphProcessor() {
}

void ParagraphCursor::ParagraphProcessor::beforeAddWord() {
	if (myWordCounter == 0) {
		myElements->push_back(TextElementPool::BeforeParagraphElement);
		if (myParagraph.kind() == Paragraph::TEXT_PARAGRAPH) {
			myElements->push_back(TextElementPool::IndentElement);
		} else if (myParagraph.kind() == Paragraph::TREE_PARAGRAPH) {
			TreeParagraph &tp = (TreeParagraph&)myParagraph;
			for (int i = 1; i < tp.depth() - 1; i++) {
				myElements->push_back(new TreeElement(TreeElement::TREE_ELEMENT_SKIP));
			}
			if (tp.depth() > 1) {
				TreeElement::TreeElementKind tek =
					(tp.parent()->children().back() == &tp) ?
						TreeElement::TREE_ELEMENT_TOP_RIGHT_LINE :
						TreeElement::TREE_ELEMENT_TOP_BOTTOM_RIGHT_LINE;
				myElements->push_back(new TreeElement(tek));
			}
			if (tp.children().empty()) {
				myElements->push_back(new TreeElement(TreeElement::TREE_ELEMENT_LEAF));
			} else if (tp.isOpen()) {
				myElements->push_back(new TreeElement(TreeElement::TREE_ELEMENT_OPEN_NODE));
			} else {
				myElements->push_back(new TreeElement(TreeElement::TREE_ELEMENT_CLOSED_NODE));
			}
		}
	}
	myWordCounter++;
}

void ParagraphCursor::ParagraphProcessor::addWord(const std::string &str, int start, int len) {
	beforeAddWord();
	Word *word = TextElementPool::getWord(str.data() + start, len, myOffset + start);
	for (std::vector<TextMark>::const_iterator mit = myFirstMark; mit != myLastMark; mit++) {
		TextMark mark = *mit;
		if ((mark.Offset - myOffset < start + len) && (mark.Offset - myOffset + mark.Length > start)) {
			word->addMark(mark.Offset - myOffset - start, mark.Length);
		}
	}
	myElements->push_back(word);
}

void ParagraphCursor::ParagraphProcessor::fill() {
	const std::vector<ParagraphEntry*> &entries = myParagraph.entries();
	for (std::vector<ParagraphEntry*>::const_iterator it = entries.begin(); it != entries.end(); it++) {
		switch ((*it)->entryKind()) {
			case ParagraphEntry::CONTROL_ENTRY:
				myElements->push_back(new ControlElement((ControlEntry&)**it));
				break;
			case ParagraphEntry::IMAGE_ENTRY:
			{
				beforeAddWord();
				const Image *image = ((ImageEntry*)*it)->image();
				if (image != NULL) {
					myElements->push_back(new ImageElement(*image));
				}
				break;
			}
			case ParagraphEntry::TEXT_ENTRY:
			{
				const std::string &text = ((TextEntry*)*it)->text();
				if (!text.empty()) {
					if (isspace(text[0])) {
						myElements->push_back(TextElementPool::HSpaceElement);
					}
					const int len = text.length();
					int firstNonSpace = -1;
					for (int i = 0; i < len; i++) {
						if (isspace(text[i])) {
							if (firstNonSpace != -1) {
								addWord(text, firstNonSpace, i - firstNonSpace);
								myElements->push_back(TextElementPool::HSpaceElement);
								firstNonSpace = -1;
							}
						} else if (firstNonSpace == -1) {
							firstNonSpace = i;
						}
					}
					if (firstNonSpace != -1) {
						addWord(text, firstNonSpace, len - firstNonSpace);
					}
					myOffset += len;
				}
				break;
			}
		}
	}
	myElements->push_back(TextElementPool::AfterParagraphElement);
}
