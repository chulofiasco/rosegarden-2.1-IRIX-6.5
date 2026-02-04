#!/bin/sh
# FilterName: Instantiate Pattern
# the next line restarts using tclsh \
exec /usr/freeware/bin/tclsh "$0" "$@"

# Pattern instantiator (very dumb)
# Main staff is the pattern,
# ref. staff contains the successive tonics we want to transpose it to

# Current limitations: the pattern has to fit in one key and one
# octave If it doesn't fit in one octave, the resulting transposed
# patterns will be "compressed" in one octave anyway. Key changes will
# cause an error, though.

package require "Petal"
PetalInit

# Analyse pattern, ie turn each pitch into its offset in the scale
# (simplistic approach which compresses everything in one octave)
set notesInMainTrack {}
foreach idx $petalItemIndexes($mainTrack) {
    set item [ PetalGetItem $mainTrack $idx ]
    set pitches [ GetPitches $item ]

    if { ![ llength $pitches ] } { continue }

    lappend notesInMainTrack $idx

    foreach pitch $pitches {
	lappend pitchesOffsets [ noteIndex $pitch ]
    }

    set pattern($idx) $pitchesOffsets
    unset pitchesOffsets
}

set timeIdx [ expr [ lindex $petalItemIndexes($mainTrack) end ] + 0 ]

# Instantiate it for each tonic in the reference staff
foreach tonicIdx $petalItemIndexes($refTrack) {
    set tonic [ GetOnePitch [ PetalGetItem $refTrack $tonicIdx ] 0 ]

    if {$tonic == {} } { continue }

    foreach idx $notesInMainTrack {

	set item [ PetalGetItem $mainTrack $idx ]
	set itemDuration [ GetDuration $item ]

	foreach offset $pattern($idx) {
	    lappend newPitches [ ith $tonic $offset ]
	}

	PetalPutItem [ NewNoteItem $itemDuration $newPitches ] $timeIdx
	unset newPitches
	incr timeIdx $itemDuration
    }
}

PetalExit


