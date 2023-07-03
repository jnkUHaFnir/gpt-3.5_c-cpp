/*
What is the simplest way to read a full line in a C console program? The text entered might have a variable length and we can't make any assumption about its content.
*/

#include <stdio.h>
#include <string.h>

int main() {
    char input[100];  // Assuming a maximum line length of 99 characters
    
    printf("Enter a line of text: ");
    fgets(input, sizeof(input), stdin);
    
    // Remove the trailing newline character, if present
    input[strcspn(input, "\n")] = '\0';
    
    printf("You entered: %s", input);
    
    return 0;
}
