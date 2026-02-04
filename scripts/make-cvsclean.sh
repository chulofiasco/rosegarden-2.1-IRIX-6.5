#!/bin/sh
#
# Don't run this.  This script is used to tidy up a Rosegarden source
# directory tree that's been checked out with cvs export; not all of
# the files are necessarily needed, so this just compares against an
# existing source tree and removes any extra files.  This is a stupid
# and dangerous script; don't use it unless you know exactly what
# you're doing, because without the right comparator tree in the right
# place, it has an effect more or less equivalent to "rm -r ."

for x in `find . -type d -print` ; do
  for y in $x/* ; do
    if [ ! -f $HOME/rosegarden/$y -a ! -d $HOME/rosegarden/$y ]
    then echo $y
      if [ -d $y ]
      then rmdir $y
      else rm $y
      fi
    fi
  done
done

