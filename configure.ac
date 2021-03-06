# vim:set et ts=4:
#
# libpyzy - The Chinese PinYin and Bopomofo conversion library.
#
# Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

# if not 1, append datestamp to the version number.
m4_define([ibus_released], [0])
m4_define([ibus_major_version], [1])
m4_define([ibus_minor_version], [3])
m4_define([ibus_micro_version], [99])
m4_define(ibus_maybe_datestamp,
    m4_esyscmd([if test x]ibus_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([ibus_version],
    ibus_major_version.ibus_minor_version.ibus_micro_version[]ibus_maybe_datestamp)

AC_INIT([libpyzy], [ibus_version], [http://code.google.com/p/ibus/issues/entry],[libpyzy])
AM_INIT_AUTOMAKE([1.10])
AC_GNU_SOURCE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# define PACKAGE_VERSION_* variables
AS_VERSION
AS_NANO
AM_SANITY_CHECK
AM_MAINTAINER_MODE
AM_DISABLE_STATIC
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

# check glib2
AM_PATH_GLIB_2_0
PKG_CHECK_MODULES(GLIB2, [
    glib-2.0 >= 2.26.0
])

# check sqlite
PKG_CHECK_MODULES(SQLITE, [
    sqlite3
])
AC_PATH_PROG(SQLITE3, sqlite3)

# check uuid
AC_CHECK_FUNCS([uuid_create], [], [
    PKG_CHECK_MODULES(LIBUUID, uuid, [
        # uuid.pc exist
        AC_DEFINE(HAVE_LIBUUID, 1, [Define if found libuuid])
        HAVE_LIBUUID=yes
    ], [
        # uuid.pc does not exist
        uuid_old_LIBS=$LIBS
        AC_CHECK_HEADERS(uuid/uuid.h, [
            AC_SEARCH_LIBS(uuid_generate, uuid, [
                test x"$ac_cv_search_uuid_generate" != x"none required" && LIBUUID_LIBS=$ac_cv_search_uuid_generate
                AC_DEFINE(HAVE_LIBUUID, 1, [Define if found libuuid])
                HAVE_LIBUUID=yes
            ], [
                AC_MSG_ERROR([Can not find uuid_create and uuid_generate in libc or libuuid])
            ])
        ], [
            AC_MSG_ERROR([Can not find uuid_create and uuid_generate in libc or libuuid])
        ])
        LIBS=$uuid_old_LIBS
    ])
])

AM_CONDITIONAL(HAVE_LIBUUID, test x"$HAVE_LIBUUID" = x"yes")

# check env
AC_PATH_PROG(ENV, env)
AC_SUBST(ENV)

# --disble-boost
AC_ARG_ENABLE(boost,
    AC_HELP_STRING([--disable-boost],
        [Do not use boost and use stdc++0x to replace boost.]),
        [enable_boost=$enableval],
        [enable_boost=yes]
)
if test x"$enable_boost" = x"yes"; then
    # check boost
    BOOST_REQUIRE([1.39])
    BOOST_FIND_HEADER([boost/bind.hpp])
    BOOST_FIND_HEADER([boost/signals2.hpp])
fi
AM_CONDITIONAL(HAVE_BOOST, test x"$enable_boost" = x"yes")

# --enable-opencc
AC_ARG_ENABLE(opencc,
    AC_HELP_STRING([--enable-opencc],
        [Use opencc for simplified and traditional Chinese conversion]),
        [enable_opencc=$enableval],
        [enable_opencc=no]
)
if test x"$enable_opencc" = x"yes"; then
    # check opencc
    PKG_CHECK_MODULES(OPENCC, [opencc], [
        AC_DEFINE(HAVE_OPENCC, 1, [Define if found opencc])
    ])
fi

# --enable-db-android
AC_ARG_ENABLE(db-android,
    AS_HELP_STRING([--disable-db-android],
        [do not build database from pinyin in android]),
    [enable_db_android=$enableval],
    [enable_db_android=yes]
)
AM_CONDITIONAL(IBUS_BUILD_DB_ANDROID, [test x"$enable_db_android" = x"yes" ])

# --enable-db-open-phrase
AC_ARG_ENABLE(db-open-phrase,
    AS_HELP_STRING([--enable-db-open-phrase],
        [build open-phrase database]),
    [enable_db_open_phrase=$enableval],
    [enable_db_open_phrase=no]
)
AM_CONDITIONAL(IBUS_BUILD_DB_OPEN_PHRASE, [test x"$enable_db_open_phrase" = x"yes" ])

# OUTPUT files
AC_CONFIG_FILES([
Makefile
libpyzy.spec
src/Makefile
data/Makefile
data/db/Makefile
data/db/android/Makefile
data/db/open-phrase/Makefile
m4/Makefile
])

AC_OUTPUT

AC_MSG_RESULT([
Build options:
    Version                     $VERSION
    Install prefix              $prefix
    Use boost                   $enable_boost
    Use opencc                  $enable_opencc
    Build database android      $enable_db_android
    Build database open-phrase  $enable_db_open_phrase
])

