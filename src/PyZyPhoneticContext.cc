/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
#include "PyZyPhoneticContext.h"
#include "PyZySimpTradConverter.h"
#include "PyZyDatabase.h"
#include "PyZyPhraseEditor.h"

namespace PyZy {

PhoneticContext::PhoneticContext (Config & config, PhoneticContext::Observer *observer)
    : m_config (config),
      m_phrase_editor (config),
      m_observer (observer)
{
    reset ();
}

PhoneticContext::~PhoneticContext ()
{
}

bool
PhoneticContext::processKeyEvent (unsigned short key)
{
    const guint key_code = key & 0x00ff;
    const guint vkey_code = key & 0xff00;

    if (vkey_code == 0) {
        if ('a' <= key_code && key_code <= 'z') {
            return insert (key_code);
        }
    } else {
        const guint key_num = key_code - '1';

        switch (vkey_code) {
        case VKEY_COMMIT:
            commit ();
            return TRUE;
        case VKEY_RESET:
            reset ();
            update ();
            return TRUE;

        case VKEY_CURSOR_RIGHT:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorRight();
            }
        case VKEY_CURSOR_LEFT:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorLeft();
            }
        case VKEY_CURSOR_RIGHT_BY_WORD:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorRightByWord ();
            }
        case VKEY_CURSOR_LEFT_BY_WORD:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorLeftByWord ();
            }
        case VKEY_CURSOR_TO_BEGIN:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorToBegin();
            }
        case VKEY_CURSOR_TO_END:
            if (m_phrase_editor.unselectCandidates ()) {
                update ();
                return TRUE;
            } else {
                return moveCursorToEnd();
            }

        case VKEY_CANDIDATE_SELECT:
            return selectCandidateInPage (key_num);
        case VKEY_CANDIDATE_FOCUS:
            return focusCandidateInPage (key_num);
        case VKEY_CANDIDATE_FOCUS_PREVIOUS:
            return focusCandidatePrevious ();
        case VKEY_CANDIDATE_FOCUS_NEXT:
            return focusCandidateNext ();
        case VKEY_CANDIDATE_RESET:
            return resetCandidateInPage (key_num);

        case VKEY_PAGE_PREVIOUS:
            selectPage (MAX(1, page ()) - 1);
            return TRUE;
        case VKEY_PAGE_NEXT:
            selectPage (page () + 1);
            return TRUE;
        case VKEY_PAGE_BEGIN:
            selectPage (0);
            return TRUE;
        case VKEY_PAGE_END:
            {
                const guint guint_max = (guint)-1;
                selectPage (guint_max);
            }
            return TRUE;

        case VKEY_DELETE_CHARACTER_BEFORE:
            return removeCharBefore();
        case VKEY_DELETE_CHARACTER_AFTER:
            return removeCharAfter();
        case VKEY_DELETE_WORD_BEFORE:
            return removeWordBefore();
        case VKEY_DELETE_WORD_AFTER:
            return removeWordAfter();
        }
    }

    g_warning ("Can't handle KeyEvent (keycode=%d, vkeycode=%d)\n",
               key_code, vkey_code);
    return FALSE;
}

gboolean
PhoneticContext::updateSpecialPhrases (void)
{
    guint size = m_special_phrases.size ();
    m_special_phrases.clear ();

    if (!m_config.specialPhrases ())
        return FALSE;

    if (!m_selected_special_phrase.empty ())
        return FALSE;

    guint begin = m_phrase_editor.cursorInChar ();
    guint end = m_cursor;

    if (begin < end) {
        SpecialPhraseTable::instance ().lookup (
            m_text.substr (begin, m_cursor - begin),
            m_special_phrases);
    }

    return size != m_special_phrases.size () || size != 0;
}

void
PhoneticContext::commitText (const std::string & commit_text)
{
    m_observer->commitText (this, commit_text);
}

void
PhoneticContext::updateLookupTable (void)
{
    m_candidates.clear ();

    for (gint i = 0; i < m_special_phrases.size (); ++i) {
        Candidate candidate;
        candidate.text = m_special_phrases[i];
        candidate.type = SPECIAL_PHRASE;
        m_candidates.push_back (candidate);
    }

    const PhraseArray & phrase_array = m_phrase_editor.candidates ();
    for (gint i = 0; i < phrase_array.size (); ++i) {
        CandidateType candidate_type;

        if (i < m_special_phrases.size ()) {
            candidate_type = SPECIAL_PHRASE;
        } else if (m_phrase_editor.candidateIsUserPhrase (i - m_special_phrases.size ())) {
            candidate_type = USER_PHRASE;
        } else {
            candidate_type = NORMAL_PHRASE;
        }

        Candidate candidate;
        candidate.text = phrase_array[i].phrase;
        candidate.type = candidate_type;
        m_candidates.push_back (candidate);
    }

    m_observer->lookupTableChanged (this);
}

void
PhoneticContext::updateAuxiliaryText (void)
{
    m_observer->auxiliaryTextChanged (this);
}

void
PhoneticContext::updatePreeditText (void)
{
    m_observer->preeditTextChanged (this);
}

void
PhoneticContext::reset (void)
{
    m_pinyin.clear ();
    m_pinyin_len = 0;
    m_phrase_editor.reset ();
    m_special_phrases.clear ();
    m_selected_special_phrase.clear ();

    m_cursor = 0;
    m_focused_candidate = 0;
    m_text.clear ();
    m_preedit_text.clear ();
}

gboolean
PhoneticContext::focusCandidateInPage (guint i)
{
    return focusCandidate (page () * m_config.pageSize () + i);
}

gboolean
PhoneticContext::focusCandidatePrevious ()
{
    if (G_UNLIKELY (m_focused_candidate == 0)) {
        return FALSE;
    }
    return focusCandidate (m_focused_candidate - 1);
}

gboolean
PhoneticContext::focusCandidateNext ()
{
    if (G_UNLIKELY (m_focused_candidate >= m_candidates.size ())) {
        return FALSE;
    }
    return focusCandidate (m_focused_candidate + 1);
}

gboolean
PhoneticContext::focusCandidate (guint i)
{
    if (G_UNLIKELY (i >= m_candidates.size ())) {
        g_warning ("Too big index. Can't focus to selected candidate.");
        return FALSE;
    }
    m_focused_candidate = i;

    update ();

    return TRUE;
}

void
PhoneticContext::selectPage (guint i)
{
    if (G_UNLIKELY (m_candidates.size () == 0)) {
        m_focused_candidate = 0;
        return;
    }

    const guint size = m_config.pageSize ();
    const guint max_page = (m_candidates.size () - 1) / size;
    if (i > max_page) {
        i = max_page;
    }

    m_focused_candidate = MIN (m_candidates.size (), i * size + m_focused_candidate % size);

    update ();
}

void
PhoneticContext::update ()
{
    updateLookupTable ();
    updatePreeditText ();
    updateAuxiliaryText ();
}

gboolean
PhoneticContext::selectCandidate (guint i)
{
    if (i >= m_config.pageSize ()) {
        g_warning ("selectCandidate(%ud): Too big index!\n", i);
    }

    if (i < m_special_phrases.size ()) {
        // select a special phrase
        m_selected_special_phrase = m_special_phrases[i];
        m_focused_candidate = 0;
        if (m_cursor == m_text.size ()) {
            commit ();
        }
        else {
            updateSpecialPhrases ();
            update ();
        }

        return TRUE;
    }

    i -= m_special_phrases.size ();
    if (m_phrase_editor.selectCandidate (i)) {
        m_focused_candidate = 0;
        if (m_phrase_editor.pinyinExistsAfterCursor () ||
            *textAfterPinyin () != '\0') {
            updateSpecialPhrases ();
            update ();
        }
        else {
            commit ();
        }
        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticContext::selectCandidateInPage (guint i)
{
    return selectCandidate (page () * m_config.pageSize () + i);
}

gboolean
PhoneticContext::resetCandidate (guint i)
{
    if (i < m_special_phrases.size ()) {
        return FALSE;
    }
    i -= m_special_phrases.size ();

    if (m_phrase_editor.resetCandidate (i)) {
        update ();
    }

    return TRUE;
}

gboolean
PhoneticContext::resetCandidateInPage (guint i)
{
    return resetCandidate (page () * m_config.pageSize () + i);
}

};  // namespace PyZy
