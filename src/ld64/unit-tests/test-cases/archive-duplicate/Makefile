##
# Copyright (c) 2006 Apple Computer, Inc. All rights reserved.
#
# @APPLE_LICENSE_HEADER_START@
# 
# This file contains Original Code and/or Modifications of Original Code
# as defined in and that are subject to the Apple Public Source License
# Version 2.0 (the 'License'). You may not use this file except in
# compliance with the License. Please obtain a copy of the License at
# http://www.opensource.apple.com/apsl/ and read it before using this
# file.
# 
# The Original Code and all software distributed under the License are
# distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
# Please see the License for the specific language governing rights and
# limitations under the License.
# 
# @APPLE_LICENSE_HEADER_END@
##
TESTROOT = ../..
include ${TESTROOT}/include/common.makefile

#
# The point of this test is a sanity check that ia an archive
# is listed multiple times, the extras are ignored.
# This is done in some makefiles because the traditional linker
# semantics is to only search an archive once.
#

run: all

all:
	${CC} ${CCFLAGS} foo.c -c -o foo-${ARCH}.o 
	${CC} ${CCFLAGS} bar.c -c -o bar-${ARCH}.o 
	libtool -static foo-${ARCH}.o  bar-${ARCH}.o -o libfoobar-${ARCH}.a
	${CC} ${CCFLAGS} main.c -lfoobar-${ARCH} -lfoobar-${ARCH} -L. -o main-${ARCH} -all_load
	${FAIL_IF_BAD_MACHO} main-${ARCH}
	${CC} ${CCFLAGS} main.c ./libfoobar-${ARCH}.a ./libfoobar-${ARCH}.a -L. -o main-${ARCH} -all_load
	${PASS_IFF_GOOD_MACHO} main-${ARCH}

clean:
	rm -rf main-* *.o *.a
