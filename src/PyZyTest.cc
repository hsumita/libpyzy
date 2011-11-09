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
#include <gtest/gtest.h>
#include <glib/gstdio.h>

#include <iostream>
#include <algorithm>

#include "PyZyConfig.h"
#include "PyZyDoublePinyinContext.h"
#include "PyZyFullPinyinContext.h"
#include "PyZyBopomofoContext.h"
#include "PyZyPhoneticContext.h"

#include "PyZyPinyinArray.h"
#include "PyZyDatabase.h"

using namespace std;
using namespace PyZy;

int main(int argc, char **argv) {
    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}

class PyZyTest : public testing::Test {
protected:
    virtual void SetUp () {
        InputContext::init ("libpyzy-test");
        PinyinConfig::init ();
        BopomofoConfig::init ();
    }

    virtual void TearDown () {
        PyZy::Database::finalize ();

        std::string db_file = g_get_user_cache_dir();
        db_file += G_DIR_SEPARATOR_S;
        db_file += "libpyzy-test";
        db_file += G_DIR_SEPARATOR_S;
        db_file += "user-1.3.db";
        g_unlink (db_file.c_str ());
    }
};

class CounterObserver : public PyZy::PhoneticContext::Observer {
public:
    void commitText (const InputContext *context, const std::string &commit_text) {
        m_commited_text = commit_text;
        ++m_commit_text;
    }
    void preeditTextChanged (const InputContext *context)     { ++m_preedit_text; }
    void auxiliaryTextChanged (const InputContext *context)   { ++m_auxiliary_text; }
    void lookupTableChanged (const InputContext *context)     { ++m_lookup_table; }

    string commitedText ()         { return m_commited_text; }
    guint commitTextCount ()       { return m_commit_text; }
    guint preeditTextCount ()      { return m_preedit_text; }
    guint auxiliaryTextCount ()    { return m_auxiliary_text; }
    guint lookupTableCount ()      { return m_lookup_table; }

    void clear () {
        m_commited_text.clear ();
        m_commit_text = 0;
        m_preedit_text = 0;
        m_auxiliary_text = 0;
        m_lookup_table = 0;
    }
private:
    string    m_commited_text;
    guint     m_commit_text;
    guint     m_preedit_text;
    guint     m_auxiliary_text;
    guint     m_lookup_table;
};

void insertKeys (PhoneticContext &context, const string &keys) {
    for (guint i = 0; i < keys.size (); ++i) {
        context.insert (keys[i]);
    }
}

TEST_F (PyZyTest, FullPinyinBasicTest) {
    CounterObserver observer;
    FullPinyinContext context(PinyinConfig::instance (), &observer);

    {  // Reset
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.reset ();
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());
    }

    {  // Commit directly
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("nihao", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select candidate
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("你好", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select special phrases.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("AA制", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select special phrases with some operations.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhii");
        EXPECT_EQ (6, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊啊之", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("a a zhi i|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (6, observer.preeditTextCount ());
        EXPECT_EQ (6, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("aazhi|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("AA制", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("AA制i", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    { // Many operations
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.focusCandidate (1);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊啊之", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a a zhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (4);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("阿", context.selectedText ());
        EXPECT_EQ ("啊之", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.unselectCandidates ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊啊之", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a a zhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("a a zh|i", context.restText ());
        EXPECT_EQ ("a a zh|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.insert ('i');
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("aazhi|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeCharBefore ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("a a zh|i", context.restText ());
        EXPECT_EQ ("a a zh|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorRight ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeWordBefore ();
        EXPECT_EQ (2, context.cursor ());
        EXPECT_EQ ("aa", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊啊", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a a|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ (7, context.cursor ());
        EXPECT_EQ ("aanihao", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊啊你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a a ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (1);
        EXPECT_EQ (7, context.cursor ());
        EXPECT_EQ ("aanihao", context.inputText ()); 
        EXPECT_EQ ("啊啊", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("啊啊nihao", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }
}

TEST_F (PyZyTest, DoublePinyinBasicTest) {
    CounterObserver observer;
    DoublePinyinContext context(PinyinConfig::instance (), &observer);

    {  // Reset
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihk");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("nihk", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.reset ();
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());
    }

    {  // Commit directly
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihk");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("nihk", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("nihk", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select candidate
        context.reset ();

        observer.clear ();
        insertKeys (context, "nihk");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("nihk", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("你好", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select special phrases.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("AA制", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select special phrases with some operations.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhii");
        EXPECT_EQ (6, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊展翅", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zang chi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (6, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("aazhi|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("AA制", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("AA制i", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    { // Many operations
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.focusCandidate (1);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊战场", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zang ch|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (3);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("阿", context.selectedText ());
        EXPECT_EQ ("战场", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("zang ch|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        // TODO(hsumita) fix. maybe we should get AA制
        observer.clear ();
        context.unselectCandidates ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊战场", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zang ch|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("a zang|i", context.restText ());
        EXPECT_EQ ("a zang|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.insert ('i');
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("i", context.restText ());
        EXPECT_EQ ("aazhi|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeCharBefore ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("a zang|i", context.restText ());
        EXPECT_EQ ("a zang|i", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorRight ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("AA制", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("aazhi|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeWordBefore ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazh", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊张", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zang|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        insertKeys (context, "nihk");
        EXPECT_EQ (8, context.cursor ());
        EXPECT_EQ ("aazhnihk", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("啊张你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("a zang ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (1);
        EXPECT_EQ (8, context.cursor ());
        EXPECT_EQ ("aazhnihk", context.inputText ()); 
        EXPECT_EQ ("啊", context.selectedText ());
        EXPECT_EQ ("张你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("zang ni hao|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("啊zhnihk", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }
}

TEST_F (PyZyTest, BopomofoBasicTest) {
    CounterObserver observer;
    BopomofoContext context(BopomofoConfig::instance (), &observer);

    {  // Reset
        context.reset ();

        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.reset ();
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());
    }

    {  // Commit directly
        context.reset ();

        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_RAW);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("sucl", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select candidate
        context.reset ();

        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("你好", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Don't use special phrases for bopomofo.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("妈妈好吃哦", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    {  // Select special phrases with some operations.
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhii");
        EXPECT_EQ (6, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃哦哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (6, observer.preeditTextCount ());
        EXPECT_EQ (6, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("ㄇㄇㄈㄘㄛ ㄛ", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|ㄛ", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (0);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ()); 
        EXPECT_EQ ("妈妈好吃哦", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("ㄛ", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("妈妈好吃哦ㄛ", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }

    { // Many operations
        context.reset ();

        observer.clear ();
        insertKeys (context, "aazhi");
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (5, observer.preeditTextCount ());
        EXPECT_EQ (5, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.focusCandidate (1);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (3);
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("慢慢", context.selectedText ());
        EXPECT_EQ ("好吃哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        // TODO(hsumita) maybe we should get AA制
        observer.clear ();
        context.unselectCandidates ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        // TODO (hsumita): should we use a space as a separator of restText
        // TODO (hsumita): "ㄇㄇㄈㄘ ㄛ" is conversionText or restText ?
        observer.clear ();
        context.moveCursorLeft ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("ㄇㄇㄈㄘ ㄛ", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ|ㄛ", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.insert ('i');
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhii", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("ㄇㄇㄈㄘㄛ ㄛ", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|ㄛ", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeCharBefore ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("ㄇㄇㄈㄘ ㄛ", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ|ㄛ", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.moveCursorRight ();
        EXPECT_EQ (5, context.cursor ());
        EXPECT_EQ ("aazhi", context.inputText ());
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃哦", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.removeWordBefore ();
        EXPECT_EQ (4, context.cursor ());
        EXPECT_EQ ("aazh", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        insertKeys (context, "nihk");
        EXPECT_EQ (8, context.cursor ());
        EXPECT_EQ ("aazhnihk", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("妈妈好吃是哦车", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄇ,ㄇ,ㄈ,ㄘ,ㄙ,ㄛ,ㄘㄜ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (4, observer.preeditTextCount ());
        EXPECT_EQ (4, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.selectCandidate (1);
        EXPECT_EQ (8, context.cursor ());
        EXPECT_EQ ("aazhnihk", context.inputText ()); 
        EXPECT_EQ ("妈妈", context.selectedText ());
        EXPECT_EQ ("好吃是哦车", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("ㄈ,ㄘ,ㄙ,ㄛ,ㄘㄜ|", context.auxiliaryText ());
        EXPECT_LT (0, context.candidates ().size ());
        EXPECT_EQ ("", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (0, observer.commitTextCount ());

        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ (0, context.cursor ());
        EXPECT_EQ ("", context.inputText ()); 
        EXPECT_EQ ("", context.selectedText ());
        EXPECT_EQ ("", context.conversionText ());
        EXPECT_EQ ("", context.restText ());
        EXPECT_EQ ("", context.auxiliaryText ());
        EXPECT_EQ (0, context.candidates ().size ());
        EXPECT_EQ ("妈妈ㄈㄘㄙㄛㄘㄜ", observer.commitedText ());
        EXPECT_EQ (1, observer.preeditTextCount ());
        EXPECT_EQ (1, observer.lookupTableCount ());
        EXPECT_EQ (1, observer.commitTextCount ());
    }
}

TEST_F (PyZyTest, Commit) {
    {  // Pinyin commit
        CounterObserver observer;
        FullPinyinContext context(PinyinConfig::instance (), &observer);

        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());

        observer.clear ();
        context.commit (InputContext::TYPE_RAW);
        EXPECT_EQ ("nihao", observer.commitedText ());

        context.reset ();
        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());

        observer.clear ();
        context.commit (InputContext::TYPE_PHONETIC);
        EXPECT_EQ ("nihao", observer.commitedText ());

        context.reset ();
        observer.clear ();
        insertKeys (context, "nihao");
        EXPECT_EQ ("nihao", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ni hao|", context.auxiliaryText ());

        // To get "你好", we should call selectCandidate().
        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ ("nihao", observer.commitedText ());
    }

    {  // Bopomofo commit
        CounterObserver observer;
        BopomofoContext context(BopomofoConfig::instance (), &observer);

        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());

        observer.clear ();
        context.commit (InputContext::TYPE_RAW);
        EXPECT_EQ ("sucl", observer.commitedText ());

        context.reset ();
        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());

        observer.clear ();
        context.commit (InputContext::TYPE_PHONETIC);
        EXPECT_EQ ("ㄋㄧㄏㄠ", observer.commitedText ());

        context.reset ();
        observer.clear ();
        insertKeys (context, "sucl");
        EXPECT_EQ ("sucl", context.inputText ());
        EXPECT_EQ ("你好", context.conversionText ());
        EXPECT_EQ ("ㄋㄧ,ㄏㄠ|", context.auxiliaryText ());

        // To get "你好", we should call selectCandidate().
        observer.clear ();
        context.commit (InputContext::TYPE_CONVERTED);
        EXPECT_EQ ("ㄋㄧㄏㄠ", observer.commitedText ());
    }
}
