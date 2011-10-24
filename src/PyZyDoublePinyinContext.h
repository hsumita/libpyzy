/* vim:set et ts=4 sts=4:
 *
 * ibus-pinyin - The Chinese PinYin engine for IBus
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __DOUBLE_PINYIN_CONTEXT_H_
#define __DOUBLE_PINYIN_CONTEXT_H_

#include "PyZyPinyinContext.h"

namespace PyZy {

class Config;

class DoublePinyinContext : public PinyinContext {

public:
    DoublePinyinContext (Config & config, PhoneticContext::Observer *observer);

    gboolean insert (gint ch);

    gboolean removeCharBefore (void);
    gboolean removeCharAfter (void);
    gboolean removeWordBefore (void);
    gboolean removeWordAfter (void);

    gboolean moveCursorLeft (void);
    gboolean moveCursorRight (void);
    gboolean moveCursorLeftByWord (void);
    gboolean moveCursorRightByWord (void);
    gboolean moveCursorToBegin (void);
    gboolean moveCursorToEnd (void);

    /* override virtual functions */
    bool processKeyEvent (unsigned short key_event);
    void reset (void);

protected:
    gboolean updatePinyin (gboolean all);

private:
    const Pinyin *isPinyin (gint i, gint j);
    const Pinyin *isPinyin (gint i);

};

};  // namespace PyZy

#endif  // __DOUBLE_PINYIN_CONTEXT_H_