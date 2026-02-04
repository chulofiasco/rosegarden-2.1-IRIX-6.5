#include <stdio.h>
#include <dmedia/midi.h>

int main() {
    int i, num = mdInit();
    printf("Found %d MIDI devices:\n", num);
    for (i = 0; i < num; i++) {
        char *name = mdGetName(i);
        printf("  %d: %s\n", i, name ? name : "NULL");
    }
    return 0;
}
