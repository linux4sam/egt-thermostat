AUTOMAKE_OPTIONS = subdir-objects

CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	-isystem $(top_srcdir)/external/rapidxml \
	$(LIBEGT_CFLAGS)

CUSTOM_LDADD = $(LIBEGT_LIBS)

bin_PROGRAMS = thermostat

thermostat_SOURCES = src/thermostat.cpp
thermostat_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
thermostat_LDADD = $(LIBEGT_LIBS) $(CUSTOM_LDADD)
thermostatdir = $(prefix)/share/egt/samples/thermostat
dist_thermostat_DATA = $(wildcard $(top_srcdir)/images/*.png)
thermostat_LDFLAGS = $(AM_LDFLAGS)
