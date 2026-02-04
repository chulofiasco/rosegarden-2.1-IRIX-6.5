######### Functions common to both Petal modes ####################

proc HarmonizePitch { pitch under over } {
        # First the pitches that are below the one we're given
        for { set i 1 } { $i <= $under } { incr i } \
            {
                lappend chord [ ith $pitch [ expr -($i * 2) ] ]
            }

        # Then the pitch itself
        lappend chord $pitch

        # Then those over it
        for { set i 1 } { $i <= $over } { incr i } \
            {
                lappend chord [ ith $pitch [ expr $i * 2 ] ]
            }
        return $chord
    }

proc Pitch2Triad { pitch } {
    return [ HarmonizePitch $pitch 0 2 ]
}

proc TransposePitch { pitch tonality } {
    return [ expr $pitch + ($defaultTonality - $tonality) ]
}

proc PetalInit {} {
    global petalMode
    global auto_path
    global argv

    petalInitInternals
    
    # Add '.../petal/editor' and '.../petal/midi' to auto_path
    set petalPath [ lindex $auto_path [ lsearch -regexp $auto_path "/petal$" ] ]
    foreach pck { editor midi } {
	set p1 [ file join $petalPath "petal$pck" ]
	lappend auto_path $p1
    }

    set petalMode [ lindex $argv 0 ]
    switch $petalMode {
	"EDITOR" { 
	    package require PetalEditor
	    set INIT "PetalEditorInit"
	}
	"MIDI"   {
	    package require PetalMidi
	    set INIT "PetalMidiInit"
	}
	default { error "wrong mode $petalMode : must be 'EDITOR' or 'MIDI'" }
    }
    # puts stderr "args : $argv"
    eval [ concat $INIT [ lrange $argv 1 end ] ]
}

package provide "Petal" "0.2"
