#!/bin/sh

intltoolize --automake --copy --force
aclocal
autoconf --force
autoheader
libtoolize
automake --add-missing --gnu --copy --no-force
