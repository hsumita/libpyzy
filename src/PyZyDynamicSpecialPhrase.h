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
#ifndef __PYZY_DYNAMIC_SPECIAL_PHRASE_H_
#define __PYZY_DYNAMIC_SPECIAL_PHRASE_H_

#include <ctime>
#include <string>
#include <glib.h>
#include "PyZySpecialPhrase.h"

namespace PyZy {

class DynamicSpecialPhrase : public SpecialPhrase {
public:
    DynamicSpecialPhrase (const std::string &text, guint pos) :
        SpecialPhrase (pos), m_text (text) { }
    ~DynamicSpecialPhrase (void);

    std::string text (void);
    const std::string dec (gint d, const gchar *fmt = "%d");
    const std::string year_cn (gboolean yy = FALSE);
    const std::string month_cn (void);
    const std::string weekday_cn (void);
    const std::string hour_cn (guint i);
    const std::string fullhour_cn (void);
    const std::string halfhour_cn (void);
    const std::string day_cn (void);
    const std::string minsec_cn (guint i);
    const std::string variable (const std::string &name);

private:
    std::string m_text;
    std::tm m_time;
};

};  // namespace PyZy

#endif  // __PYZY_DYNAMIC_SPECIAL_PHRASE_H_

