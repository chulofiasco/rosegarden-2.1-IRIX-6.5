################## Editor mode ####################

proc DumpStaff { staffNb } {
    global sortTypes
    upvar \#0 staff$staffNb staff

    if { ! [ info exist staff ] } { error "There ain't no staff$staffNb" }

    set l [ lsort $sortTypes(staff$staffNb) [ array names staff ] ]

    foreach itemTime $l {
	set item $staff($itemTime)
	if [ IsKeyChange $itemTime ] { set itemType "K" } \
	    elseif { [ llength $item ] > 1 } { set itemType "N" } \
	    else { set itemType "R" }

	puts stdout "$itemType $item"
    }
}

proc FindStaffSortType { staff } {
    global sortTypes
    global $staff

    if { ! [ array exists $staff ] } { error "Staff $staff doesn't exist" }

    if { [ array names $staff {*.9} ] != "" } {
	set sortTypes($staff) "-real"
    } else { set sortTypes($staff) "-integer" }
}

# From editor/src/Tags.h
set keyList {
    A Aflat B Bflat C Cflat Csharp D Dflat E Eflat F Fsharp G Gflat
} 

proc IsKeyChange { index } {
    return [ string match {*.9} $index ]
}

proc SetKey { key } {
    global defaultTonality
    global defaultScale
    global defaultMode
    global keyList

    set defaultMode 0

    # Deal with a numerical argument first
    if [ regexp {[0-9]+} $key ] { 
	set tonalityName [ format "tonality%s" [ lindex $keyList $key ] ] } \
	else {
	    if [ string match "*min*" $key ] { set defaultScale scaleMinor } \
		else { set defaultScale scaleMajor }
	    
	    regsub {(^[A-G])(b|\#|flat|sharp)?.*$} $key {tonality\1\2} tonalityName
	    regsub {(tonality.)(b$)} $tonalityName {\1flat} tonalityName
	    regsub {(tonality.)(\#$)} $tonalityName {\1sharp} tonalityName
	}
    global $tonalityName

    set defaultTonality [ set $tonalityName ]
}

proc NbOfPitches { item } {
    return [ expr [ llength $item ] - 1 ]
}
proc IsSinglePitch { item } {
    return [ expr [ llength $item ] == 2 ] 
}
proc GetPitches { item } { 
    return [ lrange $item 1 end ]
}
proc GetOnePitch { item pitchNb } {
    return [ lindex $item [ incr pitchNb ] ]
}
proc GetDuration { item } {
    return [ lindex $item 0 ]
}
proc AddPitches { item pitches } {
    return [ concat $item $pitches ]
}
proc SetDuration { item time } {
    return [ concat $time [ lrange $item 1 end ] ]
}

proc NewNoteItem { duration pitches } {
    return [ concat $duration $pitches ]
}

proc PetalGetItem { staffNb index } { 
    global staff$staffNb
    return [ set [ subst staff$staffNb ]($index) ]
}

proc PetalPutItem { item index } {
    global mainStaff
    set mainStaff($index) $item
}

proc ApplyPitchFilter { filter } {
    global mainStaff
    global mainNames

    # Note that this will duplicate all single pitches, but we don't
    # care 'cause NewChord() will filter 'em out

    foreach idx $mainNames {
	set el $mainStaff($idx)

	if [ IsKeyChange $el ] { SetKey $el } \
	    elseif [ NbOfPitches $el ] {
		PetalPutItem [ AddPitches $el [ $filter $el ] ] $idx
	    }
    }
}

proc PetalExit { { exitCode 0 } } {
    global mainStaff
    global mainTrack
    global staff$mainTrack

    # Is this really necessary ? Do we need the flexibility to dump
    # any staff on exit... The main one should be enough.

    unset staff$mainTrack
    array set staff$mainTrack [ array get mainStaff ]

    DumpStaff $mainTrack
    exit $exitCode
}

# Confused ? Yeah, you can be. Ok, here it is.

# mainStaff, refStaff and all the staff<some_number> variable are
# internal structure (Tcl assoc. arrays) that Petal keeps the stuff
# in.

# mainTrack and refTrack are just indexes among those 'staves' :
# mainStaff == staff$mainTrack, and refStaff == staff$refTrack
# This is to keep consistency with MIDI mode.


proc PetalEditorInit { {sNb 0} {duplicate 1} {refSNb -1} } {
    global outputFile
    global mainTrack
    global refTrack
    global mainStaff
    global nbOfStaves
    global sortTypes
    global petalItemIndexes

    set mainTrack $sNb

    # Read staff: very ugly, very insecure, very shoot-in-the-foot-ish
    uplevel \#0 { while { ! [eof stdin] } { eval [ gets stdin ] } }

    if { ! [ info exist nbOfStaves ] } { set nbOfStaves 1 }

    uplevel \#0 { 

	for { set i 0 } { $i < $nbOfStaves } { incr i } {
	    FindStaffSortType staff$i
	    set petalItemIndexes($i) [ lsort $sortTypes(staff$i) [ array names staff$i ] ]
	}

	array set mainStaff [ array get staff$mainTrack ]

	set sortTypes(mainStaff) $sortTypes(staff$mainTrack)

	# Will be used by PetalGetNextItem
	set mainNames [ lsort $sortTypes(mainStaff) [ array names mainStaff ] ]
    }

    if { $refSNb > 0 } {
	set refTrack $refSNb
	uplevel \#0 { 
	    array set refStaff [ array get staff$refTrack ]
	    set sortTypes(refStaff) $sortTypes(staff$refTrack)
	}
    }
}


package provide "PetalEditor" "0.2"
