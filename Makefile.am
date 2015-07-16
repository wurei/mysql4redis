ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects

SUBDIRS = src po

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmysql4redis.pc

$(pkgconfig_DATA): config.status

EXTRA_DIST = autogen.sh
