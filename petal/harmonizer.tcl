#!/bin/sh
# FilterName: Harmonize
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

# Trivial harmonizer

package require "Petal"

PetalInit

proc Harmonize { item } {

    if [ IsSinglePitch $item ] \
	{
	    set pitch [ GetOnePitch $item 0 ]
	    if [ isInScale $pitch ] { return [ Pitch2Triad $pitch ] }	    
	}
    return {}
}

ApplyPitchFilter Harmonize

PetalExit


