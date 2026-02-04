#!/bin/sh
#
# NAME
#   make-bindist.sh -- make a Rosegarden binary distribution
#
# SYNOPSIS
#   make-bindist.sh [-h] [-f makefile] [version-id]
#
# DESCRIPTION
#   make-bindist.sh is a tool to assist in creating binary
#   distributions of Rosegarden and preparing them for upload to an
#   ftp site.  The Rosegarden developers use this script to prepare
#   their original binary distributions; if you have successfully
#   built the applications on a system not supported by the original
#   development team, you might consider packaging up a distribution
#   and sending it to the Rosegarden ftp site yourself.  Contact
#   cannam@zands.demon.co.uk for details of where and how to upload.
#
#   make-bindist.sh builds and packs up a binary distribution of
#   the Rosegarden program suite, named according to the architecture
#   of the host machine, using the current Makefile settings.
#
#   This involves running "make clean" and "make all", creating a
#   temporary directory (under /tmp) and moving the executables, help
#   files and test files from their places in the existing source
#   tree into the correct places in the new directory tree.  Files in
#   the current directory called "README", "ANNOUNCEMENT", "CHANGES"
#   and "COPYRIGHT" will also be included, plus INSTALL.bindist.  The
#   new directory is then tar'd and gzipped up ready for uploading.
#
#   You should only run make-bindist.sh from the top-level directory
#   of a Rosegarden source distribution.  If you have changed any
#   source files or help, test or other support files from the
#   original distributions for the version of Rosegarden you're
#   building, you should think very carefully before you do anything.
#
#   You must have "gzip" in your PATH in order to run this script
#   successfully.
#
#
# ARGUMENTS
#   version-id    Optional ID string for the version of Rosegarden
#                 you're building.  In this release of make-bindist.sh,
#                 the ID string will default to "2.1".  Change
#                 this if and only if you have changed the source code.
#
# OPTIONS
#   -h            Help.  Print this lengthy and rambling explanation.
#
# CAVEATS
#   Unless you are prepared to put your name to a new binary
#   distribution of Rosegarden, you probably have no use for this
#   script.
#
#   make-bindist.sh constructs the distribution file name from the
#   results of calling "uname" with various options.  This might
#   produce strange results on some systems, though mostly it's
#   probably not all that important.
#
#   This script is a quick hack, won't work well in anything other
#   than rather obvious situations, and contains no error checking.
#
# AUTHOR
#   Chris Cannam, July 1996

rosehome=`pwd`
myfilename=$0
product=rosegarden
versionid="2.1"

explain()
{
  cat $myfilename | sed '/^$/q' | tail +2 | sed 's/^#//' | \
   sed "s/make-bindist.sh/`basename $myfilename`/"
  exit 0
}

complain()
{
  echo "For help, run "$myfilename" -h"
  exit 2
}

while getopts fh c
do
  case $c in
    h) explain ;;
    \?) complain ;;
  esac
done
found=$OPTIND
shift `expr $found - 1`

if [ "$1" ] ; then versionid=$1 ; shift ; fi
if [ "$1" ] ; then complain ; fi

distname="$product"-"$versionid"-"`./config.guess`"

echo
echo 'Distribution will be called '"$distname"'.tar.gz --'
echo 'is this okay? [Y|n]'
read nameok
if [ t$nameok = tn -o t$nameok = tN ]; then
    echo Okay, you think of something better
    exit 1
fi

tmpdir=`pwd`/"$product"-distribution-$$

if [ -d $tmpdir ]; then
  echo Temporary directory $tmpdir already exists -- stopping
  exit 1
fi

echo 'Clean old sources and objects before rebuilding? [y|N]'
read doclean
if [ t$doclean = ty -a t$doclean = tY ]; then
    make distclean 2>/dev/null
fi

echo Building from sources...
echo

./configure || exit 1
make all || exit 1

echo
echo Installing into $tmpdir

./do-install <<EOF
$tmpdir/bin
$tmpdir/lib
EOF

cp README ANNOUNCEMENT CHANGES COPYRIGHT COPYING Rosegarden $tmpdir
sed -e '/^#/d;s/@distname@/'"$distname"'/g' scripts/INSTALL.bindist > \
  $tmpdir/INSTALL

cd $tmpdir
tar cvf package.tar bin lib
tar cvf - [A-Z]* package.tar | gzip -c > ../$distname.tar.gz

echo
echo Destroying the evidence
echo
cd ..
rm -rf $tmpdir

echo Done
echo
