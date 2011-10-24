# vim:set noet ts=4:
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

SUBDIRS = \
	data \
	src \
	m4 \
	$(NULL)

DISTCHECK_CONFIGURE_FLAGS = \
	CFLAGS='-g3 -O3 -Wall' \
	CXXFLAGS='-g3 -O3 -Wall' \
	--enable-db-android \
	--enable-db-open-phrase \
	$(NULL)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	autogen.sh \
	libpyzy.spec.in \
	$(NULL)

noinst_DIST = \
	$(NULL)


DISTCLEANFILES = \
	$(NULL)

dist-hook:
	if test -d .git ; then \
		git log --name-status --date=iso > $(distdir)/ChangeLog ; \
	fi

rpm: dist @PACKAGE_NAME@.spec
	rpmbuild -bb \
			--define "_sourcedir `pwd`" \
			--define "_builddir `pwd`/rpm" \
			--define "_srcrpmdir `pwd`/rpm" \
			--define "_rpmdir `pwd`/rpm" \
			--define "_specdir `pwd`" \
			@PACKAGE_NAME@.spec

srpm: dist @PACKAGE_NAME@.spec
	rpmbuild -bs \
			--define "_sourcedir `pwd`" \
			--define "_builddir `pwd`/rpm" \
			--define "_srcrpmdir `pwd`/rpm" \
			--define "_rpmdir `pwd`/rpm" \
			--define "_specdir `pwd`" \
			@PACKAGE_NAME@.spec

.PHONY: debian/changelog
debian/changelog:
	$(AM_V_GEN) \
	( \
		. /etc/lsb-release; \
		date=`date -R`; \
		version=@VERSION@; \
		serie=$(serie); \
		if test -z "$$serie"; then \
		  serie=$$DISTRIB_CODENAME; \
		fi; \
		if test -z "$$release"; then \
		  release=1; \
		fi; \
		s=`cat debian/changelog.in`; \
		eval "echo \"$${s}\""; \
	) > $@

ppa: dist debian/changelog
	$(AM_V_GEN) \
	( \
		mkdir ppa; \
		cp $(distdir).tar.gz ppa/@PACKAGE_NAME@_@PACKAGE_VERSION@.orig.tar.gz ; \
		cd ppa; \
		tar zxvf ../$(distdir).tar.gz ; \
		cd $(distdir); \
		cp -a ../../debian . ; \
		cd debian; \
		debuild -S -sa ; \
	)

dpkg: dist debian/changelog
	$(AM_V_GEN) \
	( \
		mkdir ppa; \
		cd ppa; \
		tar zxvf ../$(distdir).tar.gz ; \
		cd $(distdir); \
		cp -a ../../debian . ; \
		cd debian; \
		debuild -b -uc -us; \
	)

upload: dist
	./tools/googlecode_upload.py \
		-s "libpyzy source code" \
		-p ibus \
		-l "Type-Source,OpSys-Linux,Chinese,Unstable" \
		$(distdir).tar.gz

clean-rpm:
	$(RM) -r "`uname -i`"

clean-local: clean-rpm
