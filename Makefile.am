SUBDIRS = external

AUTOMAKE_OPTIONS = subdir-objects

CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	-isystem $(top_srcdir)/external/sqlite3 \
	-isystem $(top_srcdir)/external/sqlite3pp/headeronly_src \
	$(LIBEGT_CFLAGS)

CUSTOM_LDADD = $(LIBEGT_LIBS) $(top_builddir)/external/libsqlite3.la

AM_CXXFLAGS = -DDATADIR=\"$(datadir)\"

bin_PROGRAMS = egt-thermostat

egt_thermostat_SOURCES = src/thermostat.cpp \
src/logic.h \
src/logic.cpp \
src/pages.h \
src/pages.cpp \
src/window.h \
src/window.cpp \
src/settings.h \
src/settings.cpp \
src/sensors.h \
src/sensors.cpp
egt_thermostat_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
egt_thermostat_LDADD = $(CUSTOM_LDADD) -ldl
egt_thermostatdir = $(prefix)/share/egt/thermostat
dist_egt_thermostat_DATA = $(wildcard $(top_srcdir)/images/*.png) \
	$(wildcard $(top_srcdir)/*.png) \
	$(wildcard $(top_srcdir)/egt-thermostat.xml)
egt_thermostat_LDFLAGS = $(AM_LDFLAGS)
