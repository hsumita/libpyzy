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
#include "PyZyPhoneticContext.h"

#include "PyZyFullPinyinContext.h"
#include "PyZyDoublePinyinContext.h"
#include "PyZyBopomofoContext.h"
#include "PyZyDatabase.h"

namespace PyZy {

void
InputContext::init ()
{
    init ("libpyzy");
}

void
InputContext::init (const std::string & user_data_dir)
{
    Database::init (user_data_dir);
}

void
InputContext::finalize ()
{
    Database::finalize ();
}


InputContext *
InputContext::create (InputContext::InputType type,
                      Config & config,
                      InputContext::Observer * observer) {
    switch (type) {
    case FULL_PINYIN:
        return new FullPinyinContext (config, observer);
    case DOUBLE_PINYIN:
        return new DoublePinyinContext (config, observer);
    case BOPOMOFO:
        return new BopomofoContext (config, observer);
    default:
        g_warning ("unknown context type.\n");
        return NULL;
    }
}

}  // namespace PyZy
