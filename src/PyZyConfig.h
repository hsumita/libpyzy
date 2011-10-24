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
#ifndef __CONFIG_H_
#define __CONFIG_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>

namespace PyZy {

#define MAX_PINYIN_LEN 64

class Config {
protected:
    Config (void);
    virtual ~Config (void);

private:
    Config (const Config & obj) { }
    
public:
    virtual void readDefaultValues (void);

    unsigned int option (void) const;
    unsigned int doublePinyinSchema (void) const;
    bool specialPhrases (void) const;
    bool modeSimp (void) const;
    unsigned int bopomofoKeyboardMapping (void) const;
    unsigned int pageSize (void) const;

    void setOption (unsigned int value);
    void setDoublePinyinSchema (unsigned int value);
    void setSpecialPhrases (bool value);
    void setModeSimp (bool value);
    void setBopomofoKeyboardMapping (unsigned int value);
    void setPageSize (unsigned int value);

protected:
    struct ConfigImpl;
    ConfigImpl *m_impl;
};

/* PinyinConfig */
class PinyinConfig : public Config {
public:
    static void init ();
    static PinyinConfig & instance (void);

protected:
    PinyinConfig ();
    virtual void readDefaultValues (void);

private:
    struct PinyinConfigImpl;
    PinyinConfigImpl *m_pinyin_impl;
};

/* Bopomof Config */
class BopomofoConfig : public Config {
public:
    static void init ();
    static BopomofoConfig & instance (void);

protected:
    BopomofoConfig ();
    virtual void readDefaultValues (void);

private:
    struct BopomofoConfigImpl;
    BopomofoConfigImpl *m_bopomofo_impl;
};

};  // namespace PyZy

#endif  // __CONFIG_H_
