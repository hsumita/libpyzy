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
#include "PyZyDoublePinyinContext.h"
#include "PyZyConfig.h"
#include "PyZyPinyinParser.h"

namespace PyZy {

#define DEFINE_DOUBLE_PINYIN_TABLES
#include "PyZyDoublePinyinTable.h"

/*
 * c in 'a' ... 'z' => id = c - 'a'
 * c == ';'         => id = 26
 * else             => id = -1
 */
#define ID(c) \
    ((c >= 'a' && c <= 'z') ? c - 'a' : (c == ';' ? 26 : -1))

#define ID_TO_SHENG(id) \
    (double_pinyin_map[m_config.doublePinyinSchema ()].sheng[id])
#define ID_TO_YUNS(id) \
    (double_pinyin_map[m_config.doublePinyinSchema ()].yun[id])

#define IS_ALPHA(c) \
    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

DoublePinyinContext::DoublePinyinContext (Config & config, PhoneticContext::Observer *observer)
    : PinyinContext (config, observer)
{
}

DoublePinyinContext::~DoublePinyinContext ()
{
}

bool
DoublePinyinContext::insert (char ch)
{
    gint id;
    /* is full */
    if (G_UNLIKELY (m_text.length () >= MAX_PINYIN_LEN))
        return true;

    id = ID (ch);
    if (id == -1) {
        /* it is not availidate ch */
        return false;
    }

    if (G_UNLIKELY (m_text.empty () &&
        ID_TO_SHENG (id) == PINYIN_ID_VOID)) {
        return false;
    }

    m_text.insert (m_cursor++, ch);

    if (m_cursor > m_pinyin_len + 2 || updatePinyin (false) == false) {
        if (!IS_ALPHA (ch)) {
            m_text.erase (--m_cursor, 1);
            return false;
        }
        else {
            if (updateSpecialPhrases ()) {
                update ();
            }
            else {
                updatePreeditText ();
                updateAuxiliaryText ();
            }
            return true;
        }
    }
    else {
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
        return true;
    }
}

bool
DoublePinyinContext::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;
    m_text.erase (m_cursor, 1);

    if (updatePinyin (false)) {
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
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
DoublePinyinContext::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor, 1);
    if (updateSpecialPhrases ()) {
        update ();
    }
    else {
        updatePreeditText ();
        updateAuxiliaryText ();
    }
    return true;
}

bool
DoublePinyinContext::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_text.erase (m_pinyin_len, m_cursor - m_pinyin_len);
        m_cursor = m_pinyin_len;
        if (updateSpecialPhrases ()) {
            update ();
        }
        else {
            updatePreeditText ();
            updateAuxiliaryText ();
        }
    }
    else {
        m_pinyin_len = m_pinyin.back ().begin;
        m_pinyin.pop_back ();
        m_text.erase (m_pinyin_len, m_cursor - m_pinyin_len);
        m_cursor = m_pinyin_len;
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
    }

    return true;
}

bool
DoublePinyinContext::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor);
    if (updateSpecialPhrases ()) {
        update ();
    }
    else {
        updatePreeditText ();
        updateAuxiliaryText ();
    }
    return true;
}

bool
DoublePinyinContext::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;

    if (m_cursor >= m_pinyin_len) {
        if (updateSpecialPhrases ()) {
            update ();
        }
        else {
            updatePreeditText ();
            updateAuxiliaryText ();
        }
    }
    else {
        if (updatePinyin (false)) {
            updateSpecialPhrases ();
            updatePhraseEditor ();
            update ();
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
    }

    return true;
}

bool
DoublePinyinContext::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor ++;
    if (updatePinyin (false)) {
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
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
DoublePinyinContext::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        if (updateSpecialPhrases ()) {
            update ();
        }
        else {
            updatePreeditText ();
            updateAuxiliaryText ();
        }
    }
    else {
        m_cursor = m_pinyin_len = m_pinyin.back ().begin;
        m_pinyin.pop_back ();
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
    }

    return true;
}

bool
DoublePinyinContext::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

bool
DoublePinyinContext::moveCursorToBegin (void)
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
DoublePinyinContext::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor = m_text.length ();
    if (updatePinyin (false)) {
        updateSpecialPhrases ();
        updatePhraseEditor ();
        update ();
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

inline const Pinyin *
DoublePinyinContext::isPinyin (gint i)
{
    if ((m_config.option () & PINYIN_INCOMPLETE_PINYIN) == 0) {
        return NULL;
    }

    gint8 sheng = ID_TO_SHENG (i);

    if (sheng == PINYIN_ID_VOID) {
        return NULL;
    }

    return PinyinParser::isPinyin (sheng, 0, PINYIN_INCOMPLETE_PINYIN);
}

inline const Pinyin *
DoublePinyinContext::isPinyin (gint i, gint j)
{
    const Pinyin *pinyin;
    gint8 sheng = ID_TO_SHENG (i);
    const gint8 *yun = ID_TO_YUNS (j);

    if (sheng == PINYIN_ID_VOID || yun[0] == PINYIN_ID_VOID)
        return NULL;

    if (sheng == PINYIN_ID_ZERO && yun[0] == PINYIN_ID_ZERO)
        return NULL;

    if (yun[1] == PINYIN_ID_VOID) {
        return PinyinParser::isPinyin (sheng, yun[0],
                        m_config.option () & (PINYIN_FUZZY_ALL | PINYIN_CORRECT_V_TO_U));
    }

    pinyin = PinyinParser::isPinyin (sheng, yun[0],
                    m_config.option () & (PINYIN_FUZZY_ALL));
    if (pinyin == NULL)
        pinyin = PinyinParser::isPinyin (sheng, yun[1],
                        m_config.option () & (PINYIN_FUZZY_ALL));
    if (pinyin != NULL)
        return pinyin;

    /* if sheng == j q x y and yun == v, try to correct v to u */
    if ((m_config.option () & PINYIN_CORRECT_V_TO_U) == 0)
        return NULL;

    if (yun[0] != PINYIN_ID_V && yun[1] != PINYIN_ID_V)
        return NULL;

    switch (sheng) {
    case PINYIN_ID_J:
    case PINYIN_ID_Q:
    case PINYIN_ID_X:
    case PINYIN_ID_Y:
        return PinyinParser::isPinyin (sheng, PINYIN_ID_V,
                            m_config.option () & (PINYIN_FUZZY_ALL | PINYIN_CORRECT_V_TO_U));
    default:
        return NULL;
    }
}

inline bool
DoublePinyinContext::updatePinyin (bool all)
{
    gboolean retval = false;

    if (all &&
        (m_pinyin_len != 0 || !m_pinyin.empty ())) {
        m_pinyin.clear ();
        m_pinyin_len = 0;
        retval = true;
    }

    if (m_pinyin_len > m_cursor) {
        retval = true;
        while (m_pinyin_len > m_cursor) {
            m_pinyin_len = m_pinyin.back ().begin;
            m_pinyin.pop_back ();
        }
    }

    if (m_pinyin_len == m_cursor) {
        return retval;
    }

    if (m_pinyin_len < m_cursor) {
        guint len = m_pinyin_len;
        if (m_pinyin.empty () == false &&
            m_pinyin.back ()->flags & PINYIN_INCOMPLETE_PINYIN) {
            const Pinyin *pinyin = isPinyin (ID (m_text[m_pinyin_len -1]),ID (m_text[m_pinyin_len]));
            if (pinyin) {
                m_pinyin.pop_back ();
                m_pinyin.append (pinyin, m_pinyin_len - 1, 2);
                m_pinyin_len += 1;
            }
        }
        while (m_pinyin_len < m_cursor && m_pinyin.size () < MAX_PHRASE_LEN) {
            const Pinyin *pinyin = NULL;
            if (m_pinyin_len == m_cursor - 1) {
                pinyin = isPinyin (ID (m_text[m_pinyin_len]));
            }
            else {
                pinyin = isPinyin (ID (m_text[m_pinyin_len]), ID (m_text[m_pinyin_len + 1]));
                if (pinyin == NULL)
                    pinyin = isPinyin (ID (m_text[m_pinyin_len]));
            }
            if (pinyin == NULL)
                break;
            if (pinyin->flags & PINYIN_INCOMPLETE_PINYIN) {
                m_pinyin.append (pinyin, m_pinyin_len, 1);
                m_pinyin_len += 1;
            }
            else {
                m_pinyin.append (pinyin, m_pinyin_len, 2);
                m_pinyin_len += 2;
            }
        }
        if (len == m_pinyin_len)
            return retval;
        return true;
    }
    return retval;
}

};  // namespace PyZy


