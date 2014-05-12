#! /bin/bash

PROJECT=mhttpd
VERSION=$(<VERSION)
DISTDIR=$PROJECT-$VERSION
DISTFILES='AUTHORS CMakeLists.txt dist.sh LICENSE README mhttpd VERSION mimosa INSTALL'

rm -rf $DISTDIR
mkdir $DISTDIR

cp -R $DISTFILES $DISTDIR
rm -rf $DISTDIR/mimosa/.git*

tar -cvvv $DISTDIR | xz -ze9c >$PROJECT-$VERSION.tar.xz
rm -rf $DISTDIR

md5sum $PROJECT-$VERSION.tar.xz >$PROJECT-$VERSION.tar.xz.md5sum
