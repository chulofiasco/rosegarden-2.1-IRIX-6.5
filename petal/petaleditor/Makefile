all : pkgIndex.tcl

pkgIndex.tcl : PetalEditor.tcl
	@echo "Creating package index for PetalEditor"
	@echo 'package ifneeded PetalEditor 1.0 [list source [file join $$dir PetalEditor.tcl]]' > pkgIndex.tcl

