#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// ==========================================
// Part A: Musical Data Structure Design
// ==========================================

// Define scale types
enum Scale { MAJOR, MINOR, PENTATONIC };

// Define Note struct and create an alias using typedef
typedef struct {
    int pitch;
    float duration;
    int velocity;
} Note;

// Static scale arrays (intervals relative to the root note)
static const int major_scale[] = {0, 2, 4, 5, 7, 9, 11};
static const int minor_scale[] = {0, 2, 3, 5, 7, 8, 10};
static const int penta_scale[] = {0, 2, 4, 7, 9};

// MIDI to frequency conversion function from previous assignments
float midi_to_frequency(int midi_pitch) {
    return 440.0 * pow(2.0, (midi_pitch - 69) / 12.0);
}

// ==========================================
// Part B: Melody Generation Engine
// ==========================================
void generate_melody(Note* melody, int length, enum Scale scale_type, int root_note) {
    const int* current_scale;
    int scale_len;

    // Set pointer and length based on selected scale
    switch(scale_type) {
        case MAJOR: 
            current_scale = major_scale; 
            scale_len = 7; 
            break;
        case MINOR: 
            current_scale = minor_scale; 
            scale_len = 7; 
            break;
        case PENTATONIC: 
            current_scale = penta_scale; 
            scale_len = 5; 
            break;
        default:
            current_scale = major_scale;
            scale_len = 7;
    }

    // Available note durations (in beats)
    float durations[] = {0.25, 0.5, 1.0, 2.0};

    // Write to array using pointer arithmetic
    for (int i = 0; i < length; i++) {
        int scale_degree = rand() % scale_len;
        int octave_offset = (rand() % 2) * 12; // 0 or 1 octave offset
        
        // Assignment requirement: Use (melody + i)->member
        (melody + i)->pitch = root_note + current_scale[scale_degree] + octave_offset;
        (melody + i)->duration = durations[rand() % 4];
        (melody + i)->velocity = 64 + (rand() % 64); // Range 64-127
    }
}

// ==========================================
// Part C: Interactive CLI and Output
// ==========================================
int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    int length, root_note, scale_choice;

    printf("=== AI-Assisted Random Melody Generator ===\n");
    
    // 1. Input melody length
    printf("Enter melody length (4-16): ");
    if (scanf("%d", &length) != 1) length = 8;
    // Bounds checking
    if (length < 4) length = 4;
    if (length > 16) length = 16;

    // 2. Choose scale
    printf("Choose scale (0=MAJOR, 1=MINOR, 2=PENTATONIC): ");
    if (scanf("%d", &scale_choice) != 1) scale_choice = 0;
    if (scale_choice < 0 || scale_choice > 2) scale_choice = 0;

    // 3. Select root note
    printf("Enter root note MIDI value (e.g., C4 = 60): ");
    if (scanf("%d", &root_note) != 1) root_note = 60;

    // Allocate struct array
    Note my_melody[16]; 

    // Pass array pointer for generation
    generate_melody(my_melody, length, (enum Scale)scale_choice, root_note);

    const char* scale_names[] = {"MAJOR", "MINOR", "PENTATONIC"};
    printf("\nGenerated Melody in %s (Root: %d, %d notes):\n", scale_names[scale_choice], root_note, length);
    printf("--------------------------------------------------\n");

    // 4. Output results and ASCII rhythm
    for (int i = 0; i < length; i++) {
        int p = (my_melody + i)->pitch;
        float d = (my_melody + i)->duration;
        int v = (my_melody + i)->velocity;
        float freq = midi_to_frequency(p);

        printf("Note %2d: MIDI %3d (%6.2f Hz) - Duration: %.2f beats [", i + 1, p, freq, d);
        
        // Draw ASCII rhythm (one '-' per 0.25 beats)
        int dashes = (int)(d / 0.25);
        for (int j = 0; j < dashes; j++) {
            printf("-");
        }
        printf("] (Vel: %3d)\n", v);
    }

    return 0;
}