#include <stdio.h>
#include <math.h> 

int main() {
    // Display welcome message for MTEC-353
    printf("Welcome to MTEC-353: Advanced Audio Programming!\n\n");
    printf("Sine Wave (Frequency: 1.0, Amplitude: 10.0):\n");

    int length = 40;    
    
    // Parameters for frequency and amplitude
    double frequency = 1.0; 
    int amplitude = 10; 

    // Generate and display an ASCII representation of a sine wave
    //*AI created section*
    for (int i = 0; i < length; i++) {
        // Calculate the sine value. We multiply `i` by a step size (e.g., 0.3) 
        // to make the wave visible within the 40-line length.
        //*AI created section*
        double time = i * frequency * 0.3;
        
        // Scale by amplitude and shift right by amplitude so we don't have negative spaces
        //*AI created section*
        int spaces = (int)(amplitude * sin(time)) + amplitude;

        // Print the spaces for the current line
        //*AI created section*
        for (int j = 0; j < spaces; j++) {
            printf(" ");
        }
        
        // Print the point of the wave
        printf("*\n");
    }

    return 0;
}
    
    