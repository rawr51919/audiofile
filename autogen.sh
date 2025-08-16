#!/bin/sh
# Run this to generate all the initial makefiles, etc.

test -n "$srcdir" || srcdir=`dirname $0`
test -n "$srcdir" || srcdir=.

AUTORECONF=`which autoreconf`
if test -z $AUTORECONF; then
	echo "autoreconf was not found; you must install automake, autoconf,"
	echo "and libtool to build libaudiofile."
	exit 1
fi

# Ensure POTFILES.in doesn't already exist before creating a blank one
if [ ! -f "$srcdir/po/POTFILES.in" ]; then
    mkdir -p "$srcdir/po"
    touch "$srcdir/po/POTFILES.in"
fi

rootme=`pwd`
cd $srcdir
autoreconf --install --force --verbose || exit $?
cd "$rootme"

$srcdir/configure "$@"

echo "\nNow type 'make' to compile libaudiofile."
