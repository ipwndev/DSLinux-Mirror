## Makefile for the gettext-tools/libgettextpo subdirectory of GNU gettext
## Copyright (C) 1995-1998, 2000-2006 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation,
## Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

## Process this file with automake to produce Makefile.in.

AUTOMAKE_OPTIONS = 1.5 gnits no-dependencies
EXTRA_DIST =
BUILT_SOURCES =
MOSTLYCLEANFILES = core *.stackdump
CLEANFILES =
DISTCLEANFILES =
SUFFIXES =

lib_LTLIBRARIES = libgettextpo.la

nodist_include_HEADERS = gettext-po.h

noinst_LTLIBRARIES =

nodist_noinst_HEADERS = config.h

AM_CPPFLAGS = \
  -I. -I$(srcdir) \
  -I.. -I$(top_srcdir) \
  -I../src -I$(top_srcdir)/src \
  -I../intl -I$(top_srcdir)/../gettext-runtime/intl

# libgettextpo contains the public API for PO files.
libgettextpo_la_SOURCES = \
  gettext-po.c \
  $(libgettextpo_la_AUXSOURCES)
# These are auxiliary sources whose symbols should not be exported.
libgettextpo_la_AUXSOURCES = \
  ../src/str-list.c \
  ../src/dir-list.c \
  ../src/message.c \
  ../src/msgl-ascii.c \
  ../src/po-error.c \
  ../src/po-xerror.c \
  ../src/write-catalog.c \
  ../src/write-po.c \
  ../src/open-catalog.c \
  ../src/po-charset.c \
  ../src/po-lex.c \
  ../src/po-gram-gen.c \
  ../src/read-po.c \
  ../src/read-catalog-abstract.c \
  ../src/read-catalog.c \
  ../src/plural-table.c \
  ../src/format-c.c \
  ../src/format-sh.c \
  ../src/format-python.c \
  ../src/format-lisp.c \
  ../src/format-elisp.c \
  ../src/format-librep.c \
  ../src/format-scheme.c \
  ../src/format-java.c \
  ../src/format-csharp.c \
  ../src/format-awk.c \
  ../src/format-pascal.c \
  ../src/format-ycp.c \
  ../src/format-tcl.c \
  ../src/format-perl.c \
  ../src/format-perl-brace.c \
  ../src/format-php.c \
  ../src/format-gcc-internal.c \
  ../src/format-qt.c \
  ../src/format-boost.c \
  ../src/format.c \
  ../src/plural-exp.c \
  ../src/plural-eval.c \
  ../src/msgl-check.c

# Libtool's library version information for libgettextpo.
# See the libtool documentation, section "Library interface versions".
LTV_CURRENT=3
LTV_REVISION=0
LTV_AGE=3

# How to build libgettextpo.la.
# Don't make it depend on libgettextsrc.la or libgettextlib.la, since they
# define an uncontrolled amount of symbols.
libgettextpo_la_LIBADD = libgnu.la
libgettextpo_la_LDFLAGS = \
  -version-info $(LTV_CURRENT):$(LTV_REVISION):$(LTV_AGE) \
  -rpath $(libdir) \
  @LTLIBINTL@ @LTLIBICONV@ -lc @LTNOUNDEF@

# Tell the mingw or Cygwin linker which symbols to export.
if WOE32DLL
libgettextpo_la_SOURCES += ../woe32dll/gettextpo-exports.c
libgettextpo_la_LDFLAGS += -Wl,--export-all-symbols
endif

# Hide symbols that are defined by libgettextpo_la_AUXSOURCES or libgnu.la
# from the global namespace, by prefixing them with "libgettextpo_".
BUILT_SOURCES += config.h
config.h:
	{ echo '/* DO NOT EDIT! GENERATED AUTOMATICALLY! */'; \
	  : "Avoid double inclusion, to avoid a warning about redefinition of DLL_VARIABLE."; \
	  echo '#ifndef GTPO_CONFIG_H'; \
	  echo '#define GTPO_CONFIG_H'; \
	  echo; \
	  echo '#include "../config.h"'; \
	  : "Turn all gettext() calls into dgettext() calls."; \
	  echo '#define DEFAULT_TEXT_DOMAIN "gettext-tools"'; \
	  : "All code is collected in a single library,"; \
	  : "No references to variables in other libraries exist."; \
	  echo '#undef DLL_VARIABLE'; \
	  echo '#define DLL_VARIABLE'; \
	  echo; \
	  echo '#endif /* GTPO_CONFIG_H */'; \
	} > config.h && \
	{ $(MAKE) $(BUILT_SOURCES) || { rm -f config.h; exit 1; }; } && \
	if test -n "$(HAVE_GLOBAL_SYMBOL_PIPE)"; then \
	  { \
	    for f in $(libgettextpo_la_AUXSOURCES) $(libgnu_la_SOURCES) $(libgnu_la_LIBADD); do \
	      case $$f in \
	        *.c | *.$(OBJEXT) | *.lo ) \
	          sf=`echo "$$f" | sed -e 's,\\.[^.]*$$,,'`.c; \
	          of=`echo "$$f" | sed -e 's,^.*/,,' -e 's,\\.[^.]*$$,,'`.$(OBJEXT); \
	          $(COMPILE) -c $(srcdir)/$$sf || { rm -f config.h; exit 1; }; \
	          sh ./exported.sh $$of 1>&5; \
	          rm -f $$of; \
	          ;; \
	      esac; \
	    done; \
	  } 5>&1 \
	    | sed -e 's,.* ,,' | LC_ALL=C sort | LC_ALL=C uniq \
	    | sed -e 's,^obstack_free$$,__obstack_free,' \
	    | sed -e 's,^\(.*\)$$,#define \1 libgettextpo_\1,' > config.h-t && \
	  if test -f config.h; then \
	    cat config.h-t >> config.h; \
	    rm -f config.h-t; \
	  else \
	    rm -f config.h-t; \
	    exit 1; \
	  fi \
	fi
MOSTLYCLEANFILES += config.h config.h-t

# Special rules for installable include files that export variables.

EXTRA_DIST += gettext-po.h.in
BUILT_SOURCES += gettext-po.h
CLEANFILES += gettext-po.h
gettext-po.h: gettext-po.h.in
if WOE32DLL
	sed -e 's/extern \([^()]*\);/extern __declspec (dllimport) \1;/' $(srcdir)/gettext-po.h.in > gettext-po.h-tmp
else
	cp $(srcdir)/gettext-po.h.in gettext-po.h-tmp
endif
	mv gettext-po.h-tmp gettext-po.h

# A small set of gnulib modules is needed here as well.
include Makefile.gnulib
