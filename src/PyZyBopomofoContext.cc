/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 * Copyright (c) 2010 BYVoid <byvoid1@gmail.com>
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
#include "PyZyBopomofoContext.h"
#include "PyZyConfig.h"
#include "PyZySimpTradConverter.h"
#include "PyZyPinyinParser.h"

namespace PyZy {
#include "PyZyBopomofoKeyboard.h"

const static gchar * bopomofo_select_keys[] = {
    "1234567890",
    "asdfghjkl;",
    "1qaz2wsxed",
    "asdfzxcvgb",
    "1234qweras",
    "aoeu;qjkix",
    "aoeuhtnsid",
    "aoeuidhtns",
    "qweasdzxcr"
};

BopomofoContext::BopomofoContext (Config & config, PhoneticContext::Observer *observer)
    : PhoneticContext (config, observer),
      m_select_mode (FALSE)
{
}

BopomofoContext::~BopomofoContext (void)
{
}

void
BopomofoContext::reset ()
{
    m_select_mode = FALSE;
    PhoneticContext::reset ();
}

gboolean
BopomofoContext::insert (gint ch)
{
    /* is full */
    if (G_UNLIKELY (m_text.length () >= MAX_PINYIN_LEN))
        return TRUE;

    m_text.insert (m_cursor++, ch);

    if (G_UNLIKELY (!(m_config.option () & PINYIN_INCOMPLETE_PINYIN))) {
        updateSpecialPhrases ();
        updatePinyin ();
    }
    else if (G_LIKELY (m_cursor <= m_pinyin_len + 2)) {
        updateSpecialPhrases ();
        updatePinyin ();
    }
    else {
        if (updateSpecialPhrases ()) {
            update ();
        }
        else {
            updatePreeditText ();
            updateAuxiliaryText ();
        }
    }
    return TRUE;
}

gboolean
BopomofoContext::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    m_text.erase (m_cursor, 1);

    updateSpecialPhrases ();
    updatePinyin ();

    return TRUE;
}

gboolean
BopomofoContext::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_text.erase (m_cursor, 1);
    updatePreeditText ();
    updateAuxiliaryText ();

    return TRUE;
}

gboolean
BopomofoContext::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    guint cursor;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        cursor = m_pinyin_len;
    }
    else {
        const Pinyin & p = *m_pinyin.back ();
        cursor = m_cursor - p.len;
        m_pinyin_len -= p.len;
        m_pinyin.pop_back ();
    }

    m_text.erase (cursor, m_cursor - cursor);
    m_cursor = cursor;
    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();
    return TRUE;
}

gboolean
BopomofoContext::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_text.erase (m_cursor, -1);
    updatePreeditText ();
    updateAuxiliaryText ();
    return TRUE;
}

gboolean
BopomofoContext::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    updateSpecialPhrases ();
    updatePinyin ();

    return TRUE;
}

gboolean
BopomofoContext::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_cursor ++;

    updateSpecialPhrases ();
    updatePinyin ();

    return TRUE;
}

gboolean
BopomofoContext::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        return TRUE;
    }

    const Pinyin & p = *m_pinyin.back ();
    m_cursor -= p.len;
    m_pinyin_len -= p.len;
    m_pinyin.pop_back ();

    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();

    return TRUE;
}

gboolean
BopomofoContext::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

gboolean
BopomofoContext::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor = 0;
    m_pinyin.clear ();
    m_pinyin_len = 0;

    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();

    return TRUE;
}

gboolean
BopomofoContext::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_cursor = m_text.length ();
    updateSpecialPhrases ();
    updatePinyin ();

    return TRUE;
}

bool
BopomofoContext::processKeyEvent (unsigned short key_event)
{
  const guint key_code = key_event & 0x00ff;
  const guint vkey_code = key_event & 0xff00;

  if (vkey_code == 0 && keyvalToBopomofo (key_code) != BOPOMOFO_ZERO) {
      m_select_mode = FALSE;
      return insert (key_code);
  }

  switch (vkey_code) {
  case VKEY_BOPOMOFO_SELECT_MODE:
      m_select_mode = TRUE;
      return TRUE;
      
  case VKEY_CANDIDATE_SELECT:
  case VKEY_CANDIDATE_FOCUS:
  case VKEY_CANDIDATE_RESET:
  case VKEY_PAGE_PREVIOUS:
  case VKEY_PAGE_NEXT:
  case VKEY_PAGE_BEGIN:
  case VKEY_PAGE_END:
      m_select_mode = TRUE;
      break;
      
  case VKEY_CURSOR_RIGHT:
  case VKEY_CURSOR_LEFT:
  case VKEY_CURSOR_RIGHT_BY_WORD:
  case VKEY_CURSOR_LEFT_BY_WORD:
  case VKEY_CURSOR_TO_BEGIN:
  case VKEY_CURSOR_TO_END:
  case VKEY_DELETE_CHARACTER_AFTER:
  case VKEY_DELETE_WORD_BEFORE:
  case VKEY_DELETE_WORD_AFTER:
      m_select_mode = FALSE;
      break;
  }

  return PhoneticContext::processKeyEvent (key_event);
}

void
BopomofoContext::updatePinyin (void)
{
    if (G_UNLIKELY (m_text.empty ())) {
        m_pinyin.clear ();
        m_pinyin_len = 0;
    }
    else {
        bopomofo.clear();
        for(String::iterator i = m_text.begin (); i != m_text.end (); ++i) {
            bopomofo += bopomofo_char[keyvalToBopomofo (*i)];
        }

        m_pinyin_len = PinyinParser::parseBopomofo (bopomofo,            // bopomofo
                                                    m_cursor,            // text length
                                                    m_config.option (),   // option
                                                    m_pinyin,            // result
                                                    MAX_PHRASE_LEN);     // max result length
    }

    updatePhraseEditor ();
    update ();
}

void
BopomofoContext::updateAuxiliaryText (void)
{
    if (G_UNLIKELY (m_text.empty () ||
        m_phrase_editor.candidates ().size () == 0)) {
        m_auxiliary_text = "";
        m_observer->auxiliaryTextChanged ();
        return;
    }

    m_buffer.clear ();

    if (m_selected_special_phrase.empty ()) {
        guint si = 0;
        guint m_text_len = m_text.length();
        for (guint i = m_phrase_editor.cursor (); i < m_pinyin.size (); ++i) {
            if (G_LIKELY (i != m_phrase_editor.cursor ()))
                m_buffer << ',';
            m_buffer << (gunichar *)m_pinyin[i]->bopomofo;
            for (guint sj = 0; m_pinyin[i]->bopomofo[sj] == bopomofo_char[keyvalToBopomofo(m_text.c_str()[si])] ; si++,sj++);
        
            if (si < m_text_len) {
                gint ch = keyvalToBopomofo(m_text.c_str()[si]);
                if (ch >= BOPOMOFO_TONE_2 && ch <= BOPOMOFO_TONE_5) {
                    m_buffer.appendUnichar(bopomofo_char[ch]);
                    ++si;
                }
            }
        }

        for (String::iterator i = m_text.begin () + m_pinyin_len; i != m_text.end (); i++) {
            if (m_cursor == (guint)(i - m_text.begin ()))
                m_buffer << '|';
            m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*i)]);
        }
        if (m_cursor == m_text.length ())
            m_buffer << '|';
    }
    else {
        if (m_cursor < m_text.size ()) {
            m_buffer << '|' << textAfterCursor ();
        }
    }

    m_auxiliary_text = m_buffer;
    m_observer->auxiliaryTextChanged ();
}

void
BopomofoContext::commit ()
{
    if (G_UNLIKELY (m_buffer.empty ()))
        return;

    m_buffer.clear ();

    if (m_select_mode) {
        m_buffer << m_phrase_editor.selectedString ();

        const gchar *p;

        if (m_selected_special_phrase.empty ()) {
            p = textAfterPinyin (m_buffer.utf8Length ());
        }
        else {
            m_buffer << m_selected_special_phrase;
            p = textAfterCursor ();
        }

        while (*p != '\0') {
            m_buffer.appendUnichar ((gunichar)bopomofo_char[keyvalToBopomofo (*p++)]);
        }
    }
    else {
        m_buffer << m_text;
    }
    
    m_phrase_editor.commit ();
    reset ();
    update ();
    m_observer->commitText (m_buffer);
}

void
BopomofoContext::updatePreeditText (void)
{
    /* preedit text = selected phrases + highlight candidate + rest text */
    if (G_UNLIKELY (m_phrase_editor.empty () && m_text.empty ())) {
        m_preedit_text.clear ();
        m_observer->preeditTextChanged ();
        return;
    }

    guint edit_begin_word = 0;
    guint edit_end_word = 0;
    guint edit_begin_byte = 0;
    guint edit_end_byte = 0;

    m_buffer.clear ();
    m_preedit_text.clear ();

    /* add selected phrases */
    m_buffer << m_phrase_editor.selectedString ();

    if (G_UNLIKELY (! m_selected_special_phrase.empty ())) {
        /* add selected special phrase */
        m_buffer << m_selected_special_phrase;
        edit_begin_word = edit_end_word = m_buffer.utf8Length ();
        edit_begin_byte = edit_end_byte = m_buffer.size ();
      
        /* append text after cursor */
        m_buffer << textAfterCursor ();
    }
    else {
        edit_begin_word = m_buffer.utf8Length ();
        edit_begin_byte = m_buffer.size ();

        if (m_candidates.size () > 0) {
            guint index = m_focused_candidate;

            if (index < m_special_phrases.size ()) {
                m_buffer << m_special_phrases[index].c_str ();
                edit_end_word = m_buffer.utf8Length ();
                edit_end_byte = m_buffer.size ();

                /* append text after cursor */
                m_buffer << textAfterCursor ();
            }
            else {
                const Phrase & candidate = m_phrase_editor.candidate (index - m_special_phrases.size ());
                if (m_text.size () == m_cursor) {
                    /* cursor at end */
                    if (m_config.modeSimp ())
                        m_buffer << candidate;
                    else
                        SimpTradConverter::simpToTrad (candidate, m_buffer);
                    edit_end_word = m_buffer.utf8Length ();
                    edit_end_byte = m_buffer.size ();
              
                    /* append rest text */
                    for (const gchar *p=m_text.c_str() + m_pinyin_len; *p ;++p) {
                        m_buffer.appendUnichar(bopomofo_char[keyvalToBopomofo(*p)]);
                    }
                }
                else {
                    for (const gchar *p = m_text.c_str (); *p; ++p) {
                        if ((guint) (p - m_text.c_str ()) == m_cursor)
                            m_buffer << ' ';
                        m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*p)]);
                    }
                    edit_end_word = m_buffer.utf8Length ();
                    edit_end_byte = m_buffer.size ();
                }
            }
        }
        else {
            edit_end_word = m_buffer.utf8Length ();
            edit_end_byte = m_buffer.size ();
            for (const gchar *p=m_text.c_str () + m_pinyin_len; *p ; ++p) {
                m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*p)]);
            }
        }
    }

    if (edit_end_byte != 0) {
        m_preedit_text.selected_text = m_buffer.substr (0, edit_begin_byte);
        m_preedit_text.candidate_text = m_buffer.substr (edit_begin_byte, edit_end_byte - edit_begin_byte);
        m_preedit_text.rest_text = m_buffer.substr (edit_end_byte);
    }

    m_observer->preeditTextChanged ();
}

static gint
keyboard_cmp (gconstpointer p1, gconstpointer p2)
{
    const gint s1 = GPOINTER_TO_INT (p1);
    const guint8 *s2 = (const guint8 *) p2;
    return s1 - s2[0];
}

gint
BopomofoContext::keyvalToBopomofo(gint ch)
{
    const gint keyboard = m_config.bopomofoKeyboardMapping ();
    const guint8 *brs;
    brs = (const guint8 *) std::bsearch (GINT_TO_POINTER (ch),
                                       bopomofo_keyboard[keyboard],
                                       G_N_ELEMENTS (bopomofo_keyboard[keyboard]),
                                       sizeof(bopomofo_keyboard[keyboard][0]),
                                       keyboard_cmp);
    if (G_UNLIKELY (brs == NULL))
        return BOPOMOFO_ZERO;
    return brs[1];
}

};  // namespace PyZy
