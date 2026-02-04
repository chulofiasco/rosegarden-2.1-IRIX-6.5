#!/bin/sh
# the next line restarts using tclsh \
exec /usr/freeware/bin/tclsh "$0" "$@"

package require "Petal"
PetalInit

foreach i $petalItemIndexes(0) {
    set e [ PetalGetItem 0 $i ]
    puts stderr "e : $e"
}

PetalExit
