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

    enum CommitType {
        TYPE_RAW,
        TYPE_PHONETIC,
        TYPE_CONVERTED
    };

    /* member functions */
    virtual bool insert (char ch) = 0;
    virtual void commit (CommitType type = TYPE_CONVERTED) = 0;
    virtual void reset (void) = 0;

    virtual bool moveCursorRight (void) = 0;
    virtual bool moveCursorLeft (void) = 0;
    virtual bool moveCursorRightByWord (void) = 0;
    virtual bool moveCursorLeftByWord (void) = 0;
    virtual bool moveCursorToBegin (void) = 0;
    virtual bool moveCursorToEnd (void) = 0;

    virtual bool selectCandidate (unsigned int index) = 0;
    virtual bool focusCandidatePrevious (void) = 0;
    virtual bool focusCandidateNext (void) = 0;
    virtual bool resetCandidate (unsigned int index) = 0;
    virtual bool unselectCandidates () = 0;

    virtual bool removeCharBefore (void) = 0;
    virtual bool removeCharAfter (void) = 0;
    virtual bool removeWordBefore (void) = 0;
    virtual bool removeWordAfter (void) = 0;

    virtual void bopomofoSelectMode () = 0;

    /* static functions */
    static void init ();
    static void init (const std::string & user_data_dir);
    static void finalize ();
    static InputContext * create (InputContext::InputType type,
                                 Config & config,
                                 InputContext::Observer * observer);

    /* accessors */
    virtual std::string inputText () const = 0;
    virtual std::string selectedText (void) const = 0;
    virtual std::string conversionText (void) const = 0;
    virtual std::string restText (void) const = 0;
    virtual std::string auxiliaryText (void) const = 0;
    virtual std::vector<Candidate> candidates () const = 0;
    virtual unsigned int cursor () const = 0;
    virtual unsigned int focusedCandidate () const = 0;
};

}; // namespace PyZy

#endif  // __PYZY_INPUT_CONTEXT_H_
