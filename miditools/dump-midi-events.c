/* dump-midi-events.c - Utility to examine MIDI file structure
 * 
 * Shows division and first events from each track to diagnose
 * why some files only play drums (program changes at wrong timestamps)
 */

#include <stdio.h>
#include <stdlib.h>
#include <MidiFile.h>
#include <MidiEvent.h>

int main(int argc, char *argv[])
{
    MIDIFileHandle mf;
    MIDIHeaderChunk header;
    int i;
    int fileIdx;
    int numTracks;
    unsigned long absTime;
    int eventCount;
    EventList track;
    EventList ev;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <midifile.mid> [midifile2.mid ...]\n", argv[0]);
        return 1;
    }
    
    /* Process each file */
    for (fileIdx = 1; fileIdx < argc; fileIdx++) {
        /* Open MIDI file */
        mf = Midi_FileOpen(argv[fileIdx], &header, MIDI_READ);
        if (!mf) {
            fprintf(stderr, "Error: Could not read MIDI file: %s\n", argv[fileIdx]);
            continue;
        }
        
        /* Print file info */
        printf("\n=== %s ===\n", argv[fileIdx]);
        printf("Format: %d\n", header.Format);
        printf("Division: %d ticks/quarter note\n", header.Timing.Division);
        printf("Tracks: %d\n\n", header.NumTracks);
        
        numTracks = header.NumTracks;
    
    /* Process each track */
    for (i = 0; i < numTracks; i++) {
        printf("--- Track %d ---\n", i);
        
        /* Skip to track chunk */
        Midi_FileSkipToNextChunk(mf, MIDI_TRACK_HEADER);
        
        /* Read track */
        track = Midi_FileReadTrack(mf);
        if (!track) {
            printf("  (empty track)\n\n");
            continue;
        }
        
        absTime = 0;
        eventCount = 0;
        
        /* Iterate through first 20 events - note: files are read with
         * Midi_TrackConvertToOnePointRepresentation() applied in sequencer,
         * but we're reading raw file format here (two-point notes) */
        for (ev = (EventList)First(track); ev && eventCount < 20; ev = (EventList)Next(ev)) {
            byte status;
            byte channel;
            
            absTime += ev->Event.DeltaTime;
            
            printf("  Tick %5lu: ", absTime);
            
            /* Extract status (upper 4 bits) and channel (lower 4 bits) */
            status = ev->Event.EventCode & 0xF0;
            channel = ev->Event.EventCode & 0x0F;
            
            /* Decode event type */
            if (status == MIDI_NOTE_ON) {
                printf("Note On     Ch%2d Note %3d Vel %3d\n",
                       channel + 1,
                       ev->Event.EventData.NoteOn.Note,
                       ev->Event.EventData.NoteOn.Velocity);
            }
            else if (status == MIDI_NOTE_OFF) {
                printf("Note Off    Ch%2d Note %3d Vel %3d\n",
                       channel + 1,
                       ev->Event.EventData.NoteOff.Note,
                       ev->Event.EventData.NoteOff.Velocity);
            }
            else if (status == MIDI_PROG_CHANGE) {
                printf("Program Chg Ch%2d Patch %3d *** INSTRUMENT ***\n",
                       channel + 1,
                       ev->Event.EventData.ProgramChange.Program);
            }
            else if (status == MIDI_CTRL_CHANGE) {
                printf("Control     Ch%2d Ctrl %3d Val %3d\n",
                       channel + 1,
                       ev->Event.EventData.ControlChange.Controller,
                       ev->Event.EventData.ControlChange.Value);
            }
            else if (status == MIDI_PITCH_BEND) {
                printf("Pitch Bend  Ch%2d LSB %3d MSB %3d\n",
                       channel + 1,
                       ev->Event.EventData.PitchWheel.LSB,
                       ev->Event.EventData.PitchWheel.MSB);
            }
            else if (status == MIDI_CHNL_AFTERTOUCH) {
                printf("Chan Press  Ch%2d\n",
                       channel + 1);
            }
            else if (status == MIDI_POLY_AFTERTOUCH) {
                printf("Key Press   Ch%2d Note %3d Vel %3d\n",
                       channel + 1,
                       ev->Event.EventData.PolyAftertouch.Note,
                       ev->Event.EventData.PolyAftertouch.Velocity);
            }
            else if (ev->Event.EventCode == MIDI_FILE_META_EVENT) {
                /* Meta event */
                switch (ev->Event.EventData.MetaEvent.MetaEventCode) {
                    case MIDI_SET_TEMPO:
                        printf("Meta: Tempo\n");
                        break;
                    case MIDI_TIME_SIGNATURE:
                        printf("Meta: Time Signature\n");
                        break;
                    case MIDI_TEXT_EVENT:
                        printf("Meta: Text/Track Name\n");
                        break;
                    case MIDI_END_OF_TRACK:
                        printf("Meta: End of Track\n");
                        goto next_track;
                    default:
                        printf("Meta: Type 0x%02X\n", ev->Event.EventData.MetaEvent.MetaEventCode);
                        break;
                }
            }
            else if (ev->Event.EventCode == MIDI_SYSTEM_EXCLUSIVE ||
                     ev->Event.EventCode == MIDI_EOX) {
                printf("SysEx\n");
            }
            else {
                printf("Unknown: EventCode 0x%02X\n", ev->Event.EventCode);
            }
            
            eventCount++;
        }
        
    next_track:
        printf("\n");
    }
    
    Midi_FileClose(mf);
    
    } /* End of file loop */
    
    return 0;
}
