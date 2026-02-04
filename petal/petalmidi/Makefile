all : pkgIndex.tcl

pkgIndex.tcl : PetalMidi.tcl
	@echo "Creating package index for PetalMidi"
	@echo 'package ifneeded PetalMidi 1.0 [list source [file join $$dir PetalMidi.tcl]]' > pkgIndex.tcl

