/*
I have two equal strings, I need to delete a portion of one of them, and store it in another.
My code is not working:

int main(int argc, char *argv[])
{
    char *imagetmp = argv[1];
    char *imagefile = imagetmp;
    char *unpackdir = imagetmp;

    // Remove substring from char imagefile
    char * pch;
    pch = strstr (imagefile,".img");
    strncpy (pch,"",6);

    // Print strings
    puts (imagefile);
    puts (unpackdir);
    return 0;
}

Here is the expected output:

./imgtools mysuperimage.img
mysuperimage.img
mysuperimage

Here is the actual output:

./imgtools mysuperimage.img
mysuperimage
mysuperimage

How can I fix this?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *imagetmp = argv[1];

    // Determine the length of the original string
    int length = strlen(imagetmp);

    // Allocate memory for imagefile and unpackdir
    char *imagefile = (char *)malloc(length + 1); // +1 for null terminator
    char *unpackdir = (char *)malloc(length + 1);

    // Copy the original string into imagefile and unpackdir
    strcpy(imagefile, imagetmp);
    strcpy(unpackdir, imagetmp);

    // Remove substring ".img" from imagefile
    char *pch = strstr(imagefile, ".img");
    if (pch != NULL) {
        *pch = '\0'; // Set the end of the string at the position of ".img"
    }

    // Print strings
    puts(imagefile);
    puts(unpackdir);

    // Free the allocated memory
    free(imagefile);
    free(unpackdir);

    return 0;
}

// Uses string with unfixed length to prevent buffer overflow
// Uses strcpy, but no danger of overflow due to malloc using string length