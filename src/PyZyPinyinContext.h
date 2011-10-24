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
#ifndef __PINYIN_CONTEXT_H_
#define __PINYIN_CONTEXT_H_

#include "PyZyPhoneticContext.h"

namespace PyZy {

class PinyinContext : public PhoneticContext {
public:
    PinyinContext (Config & config, PhoneticContext::Observer *observer);
    virtual ~PinyinContext (void);
    virtual void reset (void);
    virtual void commit (void);

protected:
    void updateAuxiliaryText (void);
    void updatePreeditText (void);
};

}; // namespace PyZy

#endif  // __PINYIN_CONTEXT_H_
