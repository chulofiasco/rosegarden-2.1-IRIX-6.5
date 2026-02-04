################## MIDI mode ####################

proc IsKeyChange { index } {
    global midiId
    global mainTrack

    return [ llength [ midigrep $midiId $mainTrack [ list $index MetaKey * * ] ] ]
} 

proc SetKey { event } {
    global defaultTonality
    global defaultScale
    global defaultMode

    set defaultMode 0

    set key [ join [ lindex $event 2 ] "" ]

    if { [ lindex $event 3 ] == "minor" } { set defaultScale scaleMinor } \
        else { set defaultScale scaleMajor }
    
    global tonality$key

    set defaultTonality [ set tonality$key ]
}

proc GrepEventType { item type } {
    global midiId
    global mainTrack

    set r {}
    foreach evt $item {
	if { [ lindex $evt 1 ] == $type } { lappend r $evt }
    }
    return $r
} 

proc GetNoteEvents { item } { GrepEventType $item "Note" }

proc NbOfPitches { item } { return [ llength [ GetNoteEvents $item ] ] }

proc IsSinglePitch { item } { return [ expr [ NbOfPitches $item ] == 1 ] }

proc GetPitches { item } {
    set p {}
    foreach evt [ GetNoteEvents $item ] { lappend p [ lindex $evt 3 ] }
    return $p
}

proc GetOnePitch { item pitchNb } {
    return [ lindex [ lindex [ GetNoteEvents $item ] $pitchNb ] 3 ]
}

proc GetDuration { item } {
    return [ lindex [ lindex [ GetNoteEvents $item ] 0 ] end ]
}

proc AddPitches { item pitches } {
    set i [ lindex [ set item [ GetNoteEvents $item ] ] 0 ]
    foreach pitch $pitches {
	set e [ lreplace $i 3 3 $pitch ]
	lappend item $e
    }
    return $item
}

proc NewNoteItem { duration pitches } {
    global defaultVelocity
    global defaultChannel

    foreach pitch $pitches {
	lappend item [ list 0 "Note" $defaultChannel $pitch $defaultVelocity $duration ]
    }
    return $item
}

proc SetDuration { item duration } {
    foreach evt $item {
	lappend newItem [ lreplace $evt end end $duration ]
    }
    return $newItem
}

proc PetalGetItem { track index } {
    global midiId

    set e [ midiget $midiId $track $index ]
    if { [ lindex [ lindex $e 0 ] 0 ] != $index } {
	error "PetalGetItem : No such time index : $index"
    } else { return $e }
}

proc PetalPutItem { item index } { 
    global midiId
    global destTrack
    global defaultVelocity

    foreach evt $item {

	# For some reason tclmidi doesn't like inserting Note events
	# with a null velocity
	
	if { [ lindex $evt 1 ] == "Note" && [ lindex $evt 4 ] == 0 } {
	    set evt [ lreplace $evt 4 4 $defaultVelocity ] 
	}
        catch { midiput $midiId $destTrack [ lreplace $evt 0 0 $index ] }
    }
}

proc ApplyPitchFilter { filter {track $mainTrack} } {
    global midiId
    global mainTrack

    set track [ subst $track ]
    midirewind $midiId $track
    set prevEventType ""
    set prevEventTime 0

    # Cumulate pitches from Note events which occur at a same time
    while {[set event [midiget $midiId $track next]] != "EOT"}\
	{
	    set eventL [ join $event ]
	    set eventType [ lindex $eventL 1 ]
	    set eventTime [ lindex $eventL 0 ]

	    if { ($eventTime != $prevEventTime) && 
		 [ info exist eventList ] } {

		set res [ $filter $eventList ]

		if [ llength $res ] \
		    { PetalPutItem [ AddPitches $eventList $res ] $prevEventTime } \
		    else { PetalPutItem $eventList $prevEventTime }
		unset eventList
		set prevEventTime $eventTime
	    }

	    switch $eventType {
		"Note" { lappend eventList $event }
		"MetaKey" { SetKey $event }
	    }
				 
	}
}


proc PetalExit { { exitCode 0 } } {
    global midiId
    global destTrack

# For debugging purposes
#     set f [ open "/tmp/out.mid" "w" ]
#     midirewind $midiId
#     midiwrite $f $midiId
#     close $f

    midirewind $midiId

    midiwrite stdout $midiId 
    flush stdout
    # stdout must be flushed

    exit $exitCode
}


proc PetalMidiInit { {argMainTrack 0} {duplicate 1} {argRefTrack 1} } {
    global midiId
    global mainTrack
    global refTrack
    global destTrack
    global petalItemIndexes
    global defaultVelocity
    global defaultChannel
    
    package require tclmidi

    set midiId [ midiread stdin ]

    set defaultVelocity 64
    set defaultVelocity 0
    set defaultChannel 0

    set mainTrack $argMainTrack
    set refTrack $argRefTrack

    # Add filter destination track
    set destTrack [ lindex [ lindex [ midiconfig $midiId ] 2 ] 1 ]

    midiconfig $midiId [ list "tracks" [ expr $destTrack + 1 ] ]

    midirewind $midiId

    for { set trck 0 } { $trck < $destTrack } { incr trck } {
	set prevTime -1
	while { [ set evt [ midiget $midiId $trck next ] ] != "EOT" } {
	    set t [ lindex $evt 0 ]
	    if { $t != $prevTime } {
		lappend petalItemIndexes($trck) $t
		set prevTime $t
	    }
	}
    }
    # Copy mainTrack into destTrack if so required
    if $duplicate {
	midicopy [ list $midiId $destTrack ] 0 [ list $midiId $mainTrack ] \
	    0 0xFFFFFF
	foreach evt [ midigrep $midiId $destTrack { * "MetaEndOfTrack" } ] {
	    mididelete $midiId $destTrack $evt
	}
    }
}


package provide "PetalMidi" "0.2"
