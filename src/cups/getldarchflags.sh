#!/bin/sh
#
# Extract architecture flags needed for programs.
#
# Currently passes x86_64.
#

ldarchflags=""
while test $# -gt 0; do
	opt=$1
	shift

	if test $opt != -arch; then
		continue
	fi

	arch=$1
	shift

	case $arch in
		x86_64)
			ldarchflags="$ldarchflags -arch $arch"
			;;
	esac
done

echo $ldarchflags
