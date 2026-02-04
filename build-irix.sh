#!/bin/sh
#
# Rosegarden 2.1 build script for IRIX 6.5
# Builds TclMidi and Rosegarden with proper n32 ABI settings
#

echo "=========================================="
echo "Rosegarden 2.1 for IRIX 6.5 Build Script"
echo "=========================================="
echo ""

# Check we're on IRIX
if [ `uname -s` != "IRIX" -a `uname -s` != "IRIX64" ]; then
    echo "Error: This script is for IRIX systems only"
    exit 1
fi

# Check for required tools
if [ ! -x /usr/freeware/bin/tclsh ]; then
    echo "Error: Tcl not found. Please install Tcl 8.0 or later in /usr/freeware"
    exit 1
fi

if [ ! -x /usr/bin/gmake ]; then
    echo "Error: GNU make (gmake) not found. Please install it."
    exit 1
fi

# Set build environment for Rosegarden
CC=cc
CFLAGS="-n32 -woff 608,3970,1552,1174,1140,1116,1209,1185,1515,3968,1164,1199,1551"
CXX=CC
CXXFLAGS="-n32 -woff 608,3970,1552,1174,1140,1116,1209,1185,1515,3968,1164,1199,1551 -I/usr/include -I/usr/include/CC"
LDFLAGS="-n32 -L/usr/lib32 -L/usr/freeware/lib/tcl8.0/"
CPPFLAGS="-I/usr/freeware/include/tcl"
PATH="/usr/freeware/bin:$PATH"
TCLLIBPATH="/usr/local/lib/tclmidi /usr/local/lib/rosegarden/petal"
export CC CFLAGS CXX CXXFLAGS LDFLAGS CPPFLAGS PATH TCLLIBPATH

echo "Build environment configured:"
echo "  CC=$CC"
echo "  CFLAGS=$CFLAGS"
echo "  CXX=$CXX"
echo "  CXXFLAGS=$CXXFLAGS"
echo "  LDFLAGS=$LDFLAGS"
echo "  CPPFLAGS=$CPPFLAGS"
echo ""

# Build TclMidi
echo "=========================================="
echo "Building TclMidi 3.1f"
echo "=========================================="
cd tclmidi || exit 1

echo "Configuring TclMidi..."
./configure CC=cc \
    CFLAGS="-n32 -woff 608,1552,1047,3970 -I/usr/freeware/include" \
    CXX=CC \
    CXXFLAGS="-n32 -woff 608,1552,1047,3970 -I/usr/include -I/usr/include/CC -I/usr/freeware/include" \
    LDFLAGS="-n32 -L/usr/lib32" \
    --with-tclsh=/usr/freeware/bin/tclsh || exit 1

echo "Building TclMidi..."
gmake clean
gmake || exit 1

echo ""
echo "Installing TclMidi to /usr/local/lib/tclmidi..."
echo "You may be prompted for root password."
gmake install || exit 1

# Test TclMidi
echo ""
echo "Testing TclMidi..."
if echo "package require tclmidi" | /usr/freeware/bin/tclsh >/dev/null 2>&1; then
    echo "TclMidi installed successfully"
else
    echo "Warning: TclMidi test failed. Check installation."
fi

cd ..

# Build Rosegarden
echo ""
echo "=========================================="
echo "Building Rosegarden 2.1"
echo "=========================================="

echo "Configuring Rosegarden..."
./configure CC="$CC" CFLAGS="$CFLAGS" CXX="$CXX" CXXFLAGS="$CXXFLAGS" \
    LDFLAGS="$LDFLAGS" CPPFLAGS="$CPPFLAGS" || exit 1

echo "Building Rosegarden..."
gmake clean
gmake || exit 1

echo ""
echo "=========================================="
echo "Build Complete!"
echo "=========================================="
echo ""
echo "Binaries created:"
echo "  bin/rosegarden         - Main launcher"
echo "  bin/editor             - Score editor"
echo "  bin/sequencer          - MIDI sequencer"
echo "  bin/dump-midi-events   - MIDI file diagnostic tool"
echo "  bin/test-midi-devices  - MIDI device test utility"
echo "  bin/test-midi-timing   - MIDI timing test utility"
echo ""
echo "To install, run:"
echo "  ./do-install"
echo ""
echo "Or to create a distribution tarball:"
echo "  gmake env-scripts"
echo "  gmake dist"
echo ""
echo "To run from build directory (as non-root user):"
echo "  setenv TCLLIBPATH \"/usr/local/lib/tclmidi /usr/local/lib/rosegarden/petal\""
echo "  setenv XAPPLRESDIR `pwd`"
echo "  bin/rosegarden"
echo ""
