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
#ifndef __PYZY_PHRASE_EDITOR_H_
#define __PYZY_PHRASE_EDITOR_H_

#include "PyZyUtil.h"
#include "PyZyString.h"
#include "PyZyPhraseArray.h"
#include "PyZyPinyinArray.h"

#define FILL_GRAN (12)

namespace PyZy {

class Query;
class Database;
class Config;

class PhraseEditor {
public:
    PhraseEditor (Config & config);
    ~PhraseEditor (void);

    const String & selectedString (void) const  { return m_selected_string; }
    const PinyinArray & pinyin (void) const     { return m_pinyin; }
    const PhraseArray & candidates (void) const { return m_candidates; }
    guint cursor (void) const                   { return m_cursor; }

    guint cursorInChar (void) const
    {
        return m_cursor == 0 ? 0 : m_pinyin[m_cursor - 1].begin + m_pinyin[m_cursor - 1].len;
    }

    gboolean pinyinExistsAfterCursor (void) const
    {
        return m_pinyin.size () > m_cursor;
    }

    const Phrase & candidate (guint i) const
    {
        return m_candidates[i];
    }

    gboolean fillCandidates (void);

    const PhraseArray & candidate0 (void) const
    {
        return m_candidate_0_phrases;
    }

    gboolean candidateIsUserPhrase (guint i) const
    {
        const Phrase & phrase = m_candidates[i];
        return phrase.len > 1 && phrase.user_freq > 0 && phrase.freq == 0;
    }

    gboolean unselectCandidates (void)
    {
        if (m_cursor == 0) {
            return FALSE;
        }
        else {
            m_selected_phrases.clear ();
            m_selected_string.truncate (0);
            m_cursor = 0;
            updateCandidates ();
            return TRUE;
        }
    }

    void reset (void)
    {
        m_candidates.clear ();
        m_selected_phrases.clear ();
        m_selected_string.truncate (0);
        m_candidate_0_phrases.clear ();
        m_pinyin.clear ();
        m_cursor = 0;
        m_query.reset ();
    }

    gboolean update (const PinyinArray &pinyin);
    gboolean selectCandidate (guint i);
    gboolean resetCandidate (guint i);
    void commit (void);

    gboolean empty (void) const
    {
        return m_selected_string.empty () && m_candidate_0_phrases.empty ();
    }

    operator gboolean (void) const
    {
        return !empty ();
    }

private:
    void updateCandidates (void);
    void updateTheFirstCandidate (void);

private:
    PhraseArray m_candidates;           // candidates phrase array
    PhraseArray m_selected_phrases;     // selected phrases, before cursor
    String      m_selected_string;      // selected phrases, in string format
    PhraseArray m_candidate_0_phrases;  // the first candidate in phrase array format
    PinyinArray m_pinyin;
    guint m_cursor;
    std::shared_ptr<Query> m_query;
    Config    & m_config;
};

};  // namespace PyZy

#endif  // __PYZY_PHRASE_EDITOR_H_
