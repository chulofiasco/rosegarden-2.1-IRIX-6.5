/* test-midi-timing.c - Test SGI MIDI timing with different resolutions */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dmedia/midi.h>
#include <sys/time.h>
#include <errno.h>

#define TEST_NOTES 100
#define TEST_DIVISION_LOW 96
#define TEST_DIVISION_HIGH 480
#define TEST_TEMPO 500000  /* 120 BPM in microseconds per beat */

void test_midi_port(char *device_name, int division, int enable_sync) {
    MDport port;
    MDevent events[TEST_NOTES];
    int i, result;
    struct timeval start, end;
    long elapsed_usec;
    unsigned long long now;
    long long tell, sleep_usec;
    int ewouldblock_count = 0;
    
    printf("\n=== Testing Division=%d, Sync=%s ===\n", 
           division, enable_sync ? "ON" : "OFF");
    printf("Using device: %s\n", device_name);
    
    /* Open MIDI port */
    port = mdOpenOutPort(device_name);
    if (port == NULL) {
        fprintf(stderr, "Failed to open MIDI port %s\n", device_name);
        perror("mdOpenOutPort");
        return;
    }
    
    /* Configure port */
    mdSetStampMode(port, MD_RELATIVETICKS);
    mdSetDivision(port, division);
    mdSetTempo(port, TEST_TEMPO);
    mdSetTemposcale(port, 1.0);
    
    /* Set start point 2 seconds in future */
    dmGetUST(&now);
    mdSetStartPoint(port, (long long)now + 2000000, 0);
    
    printf("Port configured: division=%d, tempo=%d usec/beat\n", 
           division, TEST_TEMPO);
    printf("Ticks per second: %.2f\n", 
           (1000000.0 / TEST_TEMPO) * division);
    
    /* Prepare test events - notes every 10 ticks */
    for (i = 0; i < TEST_NOTES; i++) {
        /* Note ON */
        events[i].msg[0] = 0x90;  /* Note ON, channel 0 */
        events[i].msg[1] = 60;    /* Middle C */
        events[i].msg[2] = 64;    /* Velocity */
        events[i].msglen = 3;
        events[i].stamp = i * 10; /* Every 10 ticks */
        events[i].sysexmsg = NULL;
    }
    
    /* Start timing */
    gettimeofday(&start, NULL);
    printf("Sending %d events... ", TEST_NOTES);
    fflush(stdout);
    
    /* Send events with optional sync blocking */
    for (i = 0; i < TEST_NOTES; i++) {
        /* This is the MidiPortSync blocking code from Mapper_SGI.c */
        if (enable_sync) {
            tell = mdTellNow(port);
            if (tell < (long long)events[i].stamp) {
                sleep_usec = mdTicksToNanos(port, events[i].stamp - tell) / 1000;
                if (sleep_usec > 600000) {
                    printf("\n  Event %d: tell=%lld, stamp=%llu, sleeping %lld usec",
                           i, tell, (unsigned long long)events[i].stamp, sleep_usec - 500000);
                    usleep((long)(sleep_usec - 500000));
                }
            }
        }
        
        /* Send event (may hit EWOULDBLOCK) */
        result = mdSend(port, &events[i], 1);
        while (result <= 0) {
            if (errno == EWOULDBLOCK) {
                ewouldblock_count++;
                sginap(1);  /* This is the blocking call from Mapper_SGI.c */
                result = mdSend(port, &events[i], 1);
                continue;
            }
            perror("mdSend");
            break;
        }
    }
    
    /* End timing */
    gettimeofday(&end, NULL);
    elapsed_usec = (end.tv_sec - start.tv_sec) * 1000000 + 
                   (end.tv_usec - start.tv_usec);
    
    printf("\nDone\n");
    printf("Elapsed time: %.3f seconds\n", elapsed_usec / 1000000.0);
    printf("EWOULDBLOCK count: %d\n", ewouldblock_count);
    printf("Events per second: %.2f\n", TEST_NOTES / (elapsed_usec / 1000000.0));
    
    mdClosePort(port);
    printf("Test complete\n");
}

int main(int argc, char **argv) {
    int nports;
    char *device_name;
    
    printf("SGI MIDI Timing Test Utility\n");
    printf("============================\n");
    
    /* Initialize MIDI library */
    nports = mdInit();
    if (nports < 0) {
        fprintf(stderr, "Failed to initialize MIDI library\n");
        fprintf(stderr, "Is the MIDI daemon running? Try 'startmidi'\n");
        return 1;
    }
    if (nports == 0) {
        fprintf(stderr, "No MIDI devices available\n");
        return 1;
    }
    
    printf("Found %d MIDI device(s)\n", nports);
    
    /* Get name of first device */
    device_name = mdGetName(0);
    if (device_name == NULL) {
        fprintf(stderr, "Failed to get device name for device 0\n");
        return 1;
    }
    printf("Using device 0: %s\n", device_name);
    
    /* Test 1: Low resolution, no sync */
    test_midi_port(device_name, TEST_DIVISION_LOW, 0);
    
    /* Test 2: Low resolution, with sync */
    test_midi_port(device_name, TEST_DIVISION_LOW, 1);
    
    /* Test 3: High resolution, no sync */
    test_midi_port(device_name, TEST_DIVISION_HIGH, 0);
    
    /* Test 4: High resolution, with sync */
    test_midi_port(device_name, TEST_DIVISION_HIGH, 1);
    
    printf("\n=== Summary ===\n");
    printf("If low-resolution with sync took much longer than without sync,\n");
    printf("the MidiPortSync blocking is causing your UI freeze.\n");
    printf("\nIf you saw many EWOULDBLOCK messages, the sginap() retries\n");
    printf("are blocking the main loop.\n");
    
    return 0;
}
