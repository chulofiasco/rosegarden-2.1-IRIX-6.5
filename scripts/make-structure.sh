#!/bin/sh
#
# Remake the things CVS doesn't record (soft links and
# empty directories), and clear dependencies (which may
# be wrong, say from an old machine, and may cause
# problems especially if we have no makedepend on the
# current machine)
#
rose=$1
#
# make sure there's a config.h, because SysDeps.h includes it
test -f $rose/config.h || ( set -x ; cat /dev/null > $rose/config.h )
#
# make empty directories
test -d $rose/include || ( set -x ; mkdir $rose/include )
for x in interlock lists midi mapper regexp yawn ; do
  test -d $rose/$x/lib || ( set -x ; mkdir $rose/$x/lib )
done
#
# make symbolic links for includes
( cd $rose/include
  for x in common interlock lists midi mapper regexp yawn ; do
    for y in ../$x/include/*.h ; do
      z=./`basename $y`
      test -f $z || (set -x ; ln -s $y $z )
    done
  done )
#
# remove dependencies from Makefiles
for x in lists yawn interlock midi mapper regexp sequencer editor topbox ; do
  ( cd $rose/$x/src
    echo clearing dependencies from `pwd`/Makefile
    sed -e '/^# DO NOT DELETE/q' Makefile > Makefile.tmp
    if [ ! -f Makefile.dist ] ; then mv Makefile Makefile.dist
    else rm -f Makefile
    fi
    mv Makefile.tmp Makefile )
done
