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
#ifndef __DATABASE_H_
#define __DATABASE_H_

#include "PyZyUtil.h"
#include "PyZyString.h"
#include "PyZyTypes.h"
#include "PyZyPhraseArray.h"

typedef struct sqlite3 sqlite3;

namespace PyZy {

class PinyinArray;
struct Phrase;

class SQLStmt;
typedef std::shared_ptr<SQLStmt> SQLStmtPtr;

class Database;

class Query {
public:
    Query (const PinyinArray    & pinyin,
           guint                  pinyin_begin,
           guint                  pinyin_len,
           guint                  option);
    ~Query (void);
    gint fill (PhraseArray &phrases, gint count);

private:
    const PinyinArray & m_pinyin;
    guint m_pinyin_begin;
    guint m_pinyin_len;
    guint m_option;
    SQLStmtPtr m_stmt;
};

class Database {
public:
    ~Database ();
protected:
    Database (const std::string & user_data_dir);

public:
    static void init (void);
    static void init (const std::string & data_dir);

    SQLStmtPtr query (const PinyinArray   & pinyin,
                      guint                 pinyin_begin,
                      guint                 pinyin_len,
                      gint                  m,
                      guint                 option);
    void commit (const PhraseArray  & phrases);
    void remove (const Phrase & phrase);

    void conditionsDouble (void);
    void conditionsTriple (void);

    static void finalize (void);
    static Database & instance (void)
    {
        if (m_instance == NULL) {
            g_error ("Error: Please call PhoneticContext::init () !");
        }
        return *m_instance;
    }

private:
    gboolean open (void);
    gboolean loadUserDB (void);
    gboolean saveUserDB (void);
    void prefetch (void);
    void phraseSql (const Phrase & p, String & sql);
    void phraseWhereSql (const Phrase & p, String & sql);
    gboolean executeSQL (const gchar *sql, sqlite3 *db = NULL);
    void modified (void);
    static gboolean timeoutCallback (gpointer data);

private:
    sqlite3 *m_db;              /* sqlite3 database */

    String m_sql;        /* sql stmt */
    String m_buffer;     /* temp buffer */
    guint m_timeout_id;
    GTimer *m_timer;
    String m_user_data_dir;

private:
    static std::unique_ptr<Database> m_instance;
};

};  // namespace PyZy

#endif  // __DATABASE_H_
