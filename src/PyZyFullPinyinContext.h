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
#ifndef __PYZY_FULL_PINYIN_CONTEXT_H_
#define __PYZY_FULL_PINYIN_CONTEXT_H_

#include "PyZyPinyinContext.h"

namespace PyZy {

class Config;

class FullPinyinContext : public PinyinContext {

public:
    FullPinyinContext (Config & config, PhoneticContext::Observer *observer);
    virtual ~FullPinyinContext (void);

public:
    virtual bool insert (char ch);

    virtual bool removeCharBefore (void);
    virtual bool removeCharAfter (void);
    virtual bool removeWordBefore (void);
    virtual bool removeWordAfter (void);

    virtual bool moveCursorLeft (void);
    virtual bool moveCursorRight (void);
    virtual bool moveCursorLeftByWord (void);
    virtual bool moveCursorRightByWord (void);
    virtual bool moveCursorToBegin (void);
    virtual bool moveCursorToEnd (void);

protected:
    virtual void updatePinyin (void);

};

};  // namespace PyZy

#endif  // __PYZY_FULL_PINYIN_CONTEXT_H_
