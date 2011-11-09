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
    resetContext ();
}

PhoneticContext::~PhoneticContext ()
{
}

bool
PhoneticContext::updateSpecialPhrases (void)
{
    guint size = m_special_phrases.size ();
    m_special_phrases.clear ();

    if (!m_config.specialPhrases ())
        return false;

    if (!m_selected_special_phrase.empty ())
        return false;

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
PhoneticContext::reset (void)
{
    resetContext ();
    update ();
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
PhoneticContext::resetContext (void)
{
    m_cursor = 0;
    m_focused_candidate = 0;
    m_pinyin.clear ();
    m_pinyin_len = 0;
    m_phrase_editor.reset ();
    m_special_phrases.clear ();
    m_selected_special_phrase.clear ();
    m_text.clear ();
    m_preedit_text.clear ();
    m_candidates.clear ();
    m_auxiliary_text.clear ();
}

bool
PhoneticContext::focusCandidatePrevious ()
{
    if (G_UNLIKELY (m_focused_candidate == 0)) {
        return false;
    }
    return focusCandidate (m_focused_candidate - 1);
}

bool
PhoneticContext::focusCandidateNext ()
{
    if (G_UNLIKELY (m_focused_candidate >= m_candidates.size ())) {
        return false;
    }
    return focusCandidate (m_focused_candidate + 1);
}

bool
PhoneticContext::focusCandidate (unsigned int i)
{
    if (G_UNLIKELY (i >= m_candidates.size ())) {
        g_warning ("Too big index. Can't focus to selected candidate.");
        return false;
    }
    m_focused_candidate = i;

    update ();

    return true;
}

void
PhoneticContext::update ()
{
    updateLookupTable ();
    updatePreeditText ();
    updateAuxiliaryText ();
}

bool
PhoneticContext::selectCandidate (unsigned int i)
{
    if (i >= m_candidates.size ()) {
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

        return true;
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
        return true;
    }

    return false;
}

bool
PhoneticContext::resetCandidate (unsigned int i)
{
    if (i < m_special_phrases.size ()) {
        return false;
    }
    i -= m_special_phrases.size ();

    if (m_phrase_editor.resetCandidate (i)) {
        update ();
    }

    return true;
}

bool
PhoneticContext::unselectCandidates ()
{
    if (!m_phrase_editor.unselectCandidates ()) {
        return false;
    }
    update ();
    return true;
}

};  // namespace PyZy
