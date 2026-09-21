#include <stdio.h>

int main() {
    // Define the dimensions of the rectangle
    float length = 12.5;
    float width = 8.3;
    
    // Calculate the area
    float area = length * width;
    
    // Display the result with an appropriate message
    printf("The area of a rectangle with length %.1f and width %.1f is %.2f.\n", length, width, area);
    
    return 0;
}