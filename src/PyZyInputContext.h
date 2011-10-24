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
#ifndef __PYZY_INPUT_CONTEXT_H_
#define __PYZY_INPUT_CONTEXT_H_

#include <string>
#include <vector>

namespace PyZy {

class Config;

enum CandidateType {
    NORMAL_PHRASE,
    USER_PHRASE,
    SPECIAL_PHRASE,
};

struct Candidate {
    std::string text;
    CandidateType type;
};

// lower 8bits of VKeyCode should be zero.
enum VKeyCode {
    VKEY_FIRST = 1 << 8,

    VKEY_COMMIT = (10 << 8),
    VKEY_RESET = (11 << 8),

    VKEY_CURSOR_RIGHT = (20 << 8),
    VKEY_CURSOR_LEFT = (21 << 8),
    VKEY_CURSOR_RIGHT_BY_WORD = (22 << 8),
    VKEY_CURSOR_LEFT_BY_WORD = (23 << 8),
    VKEY_CURSOR_TO_BEGIN = (24 << 8),
    VKEY_CURSOR_TO_END = (25 << 8),

    VKEY_CANDIDATE_SELECT = (30 << 8),
    VKEY_CANDIDATE_FOCUS = (31 << 8),
    VKEY_CANDIDATE_FOCUS_PREVIOUS = (32 << 8),
    VKEY_CANDIDATE_FOCUS_NEXT = (33 << 8),
    VKEY_CANDIDATE_RESET = (34 << 8),

    VKEY_PAGE_PREVIOUS = (40 << 8),
    VKEY_PAGE_NEXT = (41 << 8),
    VKEY_PAGE_BEGIN = (42 << 8),
    VKEY_PAGE_END = (43 << 8),

    VKEY_DELETE_CHARACTER_BEFORE = (50 << 8),
    VKEY_DELETE_CHARACTER_AFTER = (51 << 8),
    VKEY_DELETE_WORD_BEFORE = (52 << 8),
    VKEY_DELETE_WORD_AFTER = (53 << 8),

    VKEY_BOPOMOFO_SELECT_MODE = (60 << 8),
};

class InputContext {
public:
    virtual ~InputContext (void) { }

    class Observer {
    public:
        virtual ~Observer () { }
        virtual void commitText (const InputContext * context,
                                 const std::string &commit_text) = 0;
        virtual void preeditTextChanged (const InputContext * context) = 0;
        virtual void auxiliaryTextChanged (const InputContext * context) = 0;
        virtual void lookupTableChanged (const InputContext * context) = 0;
    };

    enum InputType {
        FULL_PINYIN,
        DOUBLE_PINYIN,
        BOPOMOFO,
    };

    /* static functions */
    static void init ();
    static void init (const std::string & user_data_dir);
    static void finalize ();
    static InputContext * create (InputContext::InputType type,
                                 Config & config,
                                 InputContext::Observer * observer);

    virtual bool processKeyEvent (unsigned short key_event) = 0;
    virtual void update (void) = 0;
    virtual void commit (void) = 0;
    virtual void reset (void) = 0;

    virtual std::string selectedText (void) const = 0;
    virtual std::string conversionText (void) const = 0;
    virtual std::string restText (void) const = 0;
    virtual std::string auxiliaryText (void) const = 0;
    virtual std::vector<Candidate> candidates () const = 0;
    virtual std::string inputText () const = 0;
    virtual unsigned int cursor () const = 0;
    virtual unsigned int focusedCandidate () const = 0;
    virtual unsigned int page () const = 0;
};

}; // namespace PyZy

#endif  // __PYZY_INPUT_CONTEXT_H_
