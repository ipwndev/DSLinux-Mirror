## Process this file with automake to produce Makefile.in -*-Makefile-*-.
AUTOMAKE_OPTIONS = 1.1 gnits

TESTS = \
  p-thru-slink \
  p-3 p-1 p-2 p-v special-1 perm parents t-slash p-slashdot
EXTRA_DIST = $(TESTS)
TESTS_ENVIRONMENT = \
  srcdir=$(srcdir) \
  PATH="$(VG_PATH_PREFIX)`pwd`/../../src$(PATH_SEPARATOR)$$PATH"
