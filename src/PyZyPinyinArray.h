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
#ifndef __PINYIN_ARRAY_H_
#define __PINYIN_ARRAY_H_

#include <vector>
#include "PyZyTypes.h"

namespace PyZy {

struct PinyinSegment {
    const Pinyin *pinyin;
    guint begin;
    guint len;

    PinyinSegment (const Pinyin *pinyin = NULL, guint begin = 0, guint len = 0)
        : pinyin (pinyin), begin (begin), len (len) { }

    operator const Pinyin * (void) const
    {
        return pinyin;
    }

    const Pinyin * operator-> (void) const
    {
        return pinyin;
    }

    gboolean operator == (const PinyinSegment & p) const
    {
        return (pinyin == p.pinyin) && (begin == p.begin) && (len == p.len);
    }

    gboolean operator == (const Pinyin *p) const
    {
        return pinyin == p;
    }
};

class PinyinArray: public std::vector<PinyinSegment> {
public:
    PinyinArray (guint init_size = 0)
    {
        std::vector<PinyinSegment>::reserve (init_size);
    }

    void append (const Pinyin *pinyin, guint begin, guint len)
    {
        push_back (PinyinSegment (pinyin, begin, len));
    }
};

};  // namespace PyZy

#endif  // __PINYIN_ARRAY_H_
