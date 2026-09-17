# Week 1 Process Journal

**Name:** Mike Tang  
**Email:** matng2@berklee.edu  
**Date:** 9/16/2026  

## Gemini Prompt & Code
    So, I used Google Gemini and asked it to write a C program that calculates and displays the area of a rectangle with length 12.5 and width 8.3, then prints the result and an appropriate message.

    It said: 
    
Here is the C program:


#include <stdio.h>

int main() {
    // Define dimensions
    float length = 12.5;
    float width = 8.3;
    
    // Calculate area
    float area = length * width;
    
    printf("The area is %.2f.\n", area);
    
    return 0;
}

## Output 
When you compile and run this program, it will output:
The area of a rectangle with length 12.5 and width 8.3 is 103.75.


 	And when my partner at that time use chatgpt to ask same question, it use “double” instead if “float”, so I ask Gemini: whats float for, and it response:

 In C programming, float is a data type used to store numbers that have a decimal point (also known as "floating-point numbers").
Here is why we used it in your program:
The dimensions you provided were 12.5 and 8.3.
If we had used the int (integer) data type instead, the computer would have chopped off the decimal parts. It would only see 12 and 8, which would give you the wrong calculated area (96 instead of 103.75).
	

	and it gives me a chart and the following :


Formatting Tip: In the code I wrote for you, I used %.1f and %.2f inside the printf statement. The .1 and .2 tell the program exactly how many decimal places to print on the screen so it looks clean!

    So I understand that using “float” allows to more accurately calculate and measure the area of a rectangle with length 12.5 and width 8.3, although using double is great, but the data to storage or the number to print are different intern of having a decimal point or not.

Whole conversation link : https://share.gemini.google/J6Nm0LWxzpEK    	

