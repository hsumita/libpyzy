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
#include "PyZyFullPinyinContext.h"
#include "PyZyConfig.h"
#include "PyZyPinyinParser.h"

namespace PyZy {

FullPinyinContext::FullPinyinContext (Config & config, PhoneticContext::Observer *observer)
  : PinyinContext (config, observer)
{
}

FullPinyinContext::~FullPinyinContext (void)
{
}

bool
FullPinyinContext::insert (char ch)
{
    /* is full */
    if (G_UNLIKELY (m_text.length () >= MAX_PINYIN_LEN))
        return true;

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
    return true;
}

bool
FullPinyinContext::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;
    m_text.erase (m_cursor, 1);

    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
FullPinyinContext::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor, 1);
    updatePreeditText ();
    updateAuxiliaryText ();

    return true;
}

bool
FullPinyinContext::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

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
    return true;
}

bool
FullPinyinContext::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor, -1);
    updatePreeditText ();
    updateAuxiliaryText ();
    return true;
}

bool
FullPinyinContext::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
FullPinyinContext::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor ++;
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
FullPinyinContext::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        return true;
    }

    const Pinyin & p = *m_pinyin.back ();
    m_cursor -= p.len;
    m_pinyin_len -= p.len;
    m_pinyin.pop_back ();

    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();
    return true;
}

bool
FullPinyinContext::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

bool
FullPinyinContext::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor = 0;
    m_pinyin.clear ();
    m_pinyin_len = 0;

    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();
    return true;
}

bool
FullPinyinContext::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor = m_text.length ();
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

void
FullPinyinContext::updatePinyin (void)
{
    if (G_UNLIKELY (m_text.empty ())) {
        m_pinyin.clear ();
        m_pinyin_len = 0;
    }
    else {
        m_pinyin_len = PinyinParser::parse (m_text,              // text
                                            m_cursor,            // text length
                                            m_config.option (),   // option
                                            m_pinyin,            // result
                                            MAX_PHRASE_LEN);     // max result length
    }

    updatePhraseEditor ();
    update ();
}

};  // namespace PyZy
