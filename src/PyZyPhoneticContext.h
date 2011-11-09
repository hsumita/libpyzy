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
#ifndef __PYZY_PHONETIC_CONTEXT_H_
#define __PYZY_PHONETIC_CONTEXT_H_

#include <string>
#include <vector>
#include "PyZyInputContext.h"
#include "PyZyConfig.h"
#include "PyZyPinyinArray.h"
#include "PyZyPhraseEditor.h"
#include "PyZySpecialPhraseTable.h"

namespace PyZy {

struct Preedit {
    std::string selected_text;
    std::string candidate_text;
    std::string rest_text;

    void clear () {
        selected_text.clear ();
        candidate_text.clear ();
        rest_text.clear ();
    }
};


class PhoneticContext : public InputContext {
public:
    PhoneticContext (Config & config, PhoneticContext::Observer *observer);
    virtual ~PhoneticContext (void);

    /* API of InputContext */
    virtual void reset (void);
    
    bool selectCandidate (unsigned int i);
    bool focusCandidate (unsigned int i);
    bool focusCandidatePrevious ();
    bool focusCandidateNext ();
    bool resetCandidate (unsigned int i);
    bool unselectCandidates ();

    /* inline functions */
    virtual void bopomofoSelectMode () { }

    /* Accessors of InputContext. */
    virtual std::string inputText () const { return m_text; }
    virtual std::string selectedText (void) const { return m_preedit_text.selected_text; }
    virtual std::string conversionText (void) const { return m_preedit_text.candidate_text; }
    virtual std::string restText (void) const { return m_preedit_text.rest_text; }
    virtual std::string auxiliaryText (void) const { return m_auxiliary_text; }
    virtual std::vector<Candidate> candidates () const { return m_candidates; }
    virtual unsigned int cursor () const { return m_cursor; }
    virtual unsigned int focusedCandidate () const { return m_focused_candidate; }

protected:
    virtual void resetContext (void);
    virtual void update (void);
    virtual void commitText (const std::string & commit_text);
    virtual void updateLookupTable (void);
    virtual void updateAuxiliaryText (void);
    virtual void updatePreeditText (void);

    virtual bool updateSpecialPhrases (void);

    /* inline functions */
    void updatePhraseEditor (void)
    {
        m_phrase_editor.update (m_pinyin);
    }

    const gchar * textAfterPinyin () const
    {
        return (const gchar *)m_text + m_pinyin_len;
    }

    const gchar * textAfterPinyin (guint i) const
    {
        g_assert (i <= m_pinyin.size ());
        if ( G_UNLIKELY (i == 0))
            return m_text;
        i--;
        return (const gchar *)m_text + m_pinyin[i].begin + m_pinyin[i].len;
    }

    const gchar * textAfterCursor () const
    {
        return (const gchar *)m_text + m_cursor;
    }

    /* variables */
    Config                     &m_config;
    guint                       m_cursor;
    guint                       m_focused_candidate;
    PinyinArray                 m_pinyin;
    guint                       m_pinyin_len;
    String                      m_buffer;
    PhraseEditor                m_phrase_editor;
    std::vector<std::string>    m_special_phrases;
    std::string                 m_selected_special_phrase;
    String                      m_text;
    Preedit                     m_preedit_text;
    std::vector<Candidate>      m_candidates;
    std::string                 m_auxiliary_text;

private:
    PhoneticContext::Observer  *m_observer;
};

}; // namespace PyZy

#endif  // __PYZY_PHONETIC_CONTEXT_H_
