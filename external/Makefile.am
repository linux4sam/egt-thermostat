noinst_LTLIBRARIES = libsqlite3.la

libsqlite3_la_SOURCES = \
sqlite3/sqlite3.c \
sqlite3/sqlite3ext.h \
sqlite3/sqlite3.h

libsqlite3_la_CFLAGS = \
	-I$(top_srcdir)/external/sqlite3 \
	$(AM_CFLAGS)

EXTRA_DIST = \
	sqlite3 \
	sqlite3pp
