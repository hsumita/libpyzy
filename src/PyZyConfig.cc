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
#include "PyZyConfig.h"

#include "PyZyUtil.h"
#include "PyZyTypes.h"
#include "PyZyDoublePinyinTable.h"

namespace PyZy {

const unsigned int PINYIN_DEFAULT_OPTION =
        PINYIN_INCOMPLETE_PINYIN |
        PINYIN_CORRECT_ALL |
        PINYIN_FUZZY_C_CH |
        // PINYIN_FUZZY_CH_C |
        PINYIN_FUZZY_Z_ZH |
        // PINYIN_FUZZY_ZH_Z |
        PINYIN_FUZZY_S_SH |
        // PINYIN_FUZZY_SH_S |
        PINYIN_FUZZY_L_N |
        // PINYIN_FUZZY_N_L |
        PINYIN_FUZZY_F_H |
        // PINYIN_FUZZY_H_F |
        // PINYIN_FUZZY_L_R |
        // PINYIN_FUZZY_R_L |
        PINYIN_FUZZY_K_G |
        PINYIN_FUZZY_G_K |
        PINYIN_FUZZY_AN_ANG |
        PINYIN_FUZZY_ANG_AN |
        PINYIN_FUZZY_EN_ENG |
        PINYIN_FUZZY_ENG_EN |
        PINYIN_FUZZY_IN_ING |
        PINYIN_FUZZY_ING_IN |
        // PINYIN_FUZZY_IAN_IANG |
        // PINYIN_FUZZY_IANG_IAN |
        // PINYIN_FUZZY_UAN_UANG |
        // PINYIN_FUZZY_UANG_UAN |
        0;

struct Config::ConfigImpl {
    bool         m_special_phrases;
    bool         m_mode_simp;
    unsigned int m_option;
    unsigned int m_option_mask;
    unsigned int m_double_pinyin_schema;
    unsigned int m_bopomofo_keyboard_mapping;
};

Config::Config ()
    : m_impl (new Config::ConfigImpl)
{
    readDefaultValues ();
}

Config::~Config ()
{
    delete m_impl;
}

/* Accessors */
unsigned int
Config::option (void) const
{
    return m_impl->m_option;
}

unsigned int
Config::doublePinyinSchema (void) const
{
    return m_impl->m_double_pinyin_schema;
}

bool
Config::specialPhrases (void) const
{
    return m_impl->m_special_phrases;
}

bool
Config::modeSimp (void) const
{
    return m_impl->m_mode_simp;
}

unsigned int
Config::bopomofoKeyboardMapping (void) const
{
    return m_impl->m_bopomofo_keyboard_mapping;
}

void
Config::setOption (unsigned int value)
{
    m_impl->m_option = value;
}

void
Config::setDoublePinyinSchema (unsigned int value)
{
    m_impl->m_double_pinyin_schema = value;
}

void
Config::setSpecialPhrases (bool value)
{
    m_impl->m_special_phrases = value;
}

void
Config::setModeSimp (bool value)
{
    m_impl->m_mode_simp = value;
}

void
Config::setBopomofoKeyboardMapping (unsigned int value)
{
    m_impl->m_bopomofo_keyboard_mapping = value;
}

void
Config::readDefaultValues ()
{
    m_impl->m_option = PINYIN_DEFAULT_OPTION;
    m_impl->m_option_mask = PINYIN_INCOMPLETE_PINYIN | PINYIN_CORRECT_ALL;
    m_impl->m_double_pinyin_schema = 0;
    m_impl->m_mode_simp = TRUE;
    m_impl->m_special_phrases = TRUE;
    m_impl->m_bopomofo_keyboard_mapping = 0;
}

/* Pinyin Config */
struct PinyinConfig::PinyinConfigImpl {
    static std::unique_ptr <PinyinConfig> m_instance;
};

std::unique_ptr<PinyinConfig> PinyinConfig::PinyinConfigImpl::m_instance;

PinyinConfig::PinyinConfig ()
    : Config ()
{
}

PinyinConfig &
PinyinConfig::instance ()
{
    return *PinyinConfigImpl::m_instance;
}

void
PinyinConfig::init ()
{
    if (PinyinConfigImpl::m_instance == NULL) {
        PinyinConfigImpl::m_instance.reset (new PinyinConfig ());
        PinyinConfigImpl::m_instance->readDefaultValues ();
    }
}

void
PinyinConfig::readDefaultValues ()
{
    Config::readDefaultValues ();
}


/* Bopomofo Config */

struct BopomofoConfig::BopomofoConfigImpl {
    static std::unique_ptr <BopomofoConfig> m_instance;
};

std::unique_ptr<BopomofoConfig> BopomofoConfig::BopomofoConfigImpl::m_instance;

BopomofoConfig::BopomofoConfig ()
    : Config ()
{
}

BopomofoConfig &
BopomofoConfig::instance ()
{
    return *BopomofoConfigImpl::m_instance;
}

void
BopomofoConfig::init ()
{
    if (BopomofoConfigImpl::m_instance == NULL) {
        BopomofoConfigImpl::m_instance.reset (new BopomofoConfig ());
        BopomofoConfigImpl::m_instance->readDefaultValues ();
    }
}

void
BopomofoConfig::readDefaultValues (void)
{
    Config::readDefaultValues ();
    m_impl->m_special_phrases = FALSE;
}

};  // namespace PyZy
