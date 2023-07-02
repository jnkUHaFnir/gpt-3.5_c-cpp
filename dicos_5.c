/*
I was trying to implement Viterbi decoder in C.
So I thought of using a 2-dimensional array which i wanted to create dynamically. Here is the sample code for 2D array created dynamically:


place_table = (int **)malloc((no_places+1)*sizeof(int *));
for(i = 1; i <= no_places; i++)
    place_table[i] = (int *)malloc((no_places+1)*sizeof(int));


The size of this 2-dimensional array keeps on varying in my decoder, i.e., no_places keeps on varying, so I wanted to know how to reallocate memory for the 2-dimensional array.
Any suggestion or help would be greatly appreciated.
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    int **place_table = NULL;
    int no_places = 3; // Example size of the array
    int i, j;

    // Allocate memory for the initial array
    place_table = (int **)malloc((no_places + 1) * sizeof(int *));
    for (i = 1; i <= no_places; i++)
        place_table[i] = (int *)malloc((no_places + 1) * sizeof(int));

    // Print the initial array
    for (i = 1; i <= no_places; i++) {
        for (j = 1; j <= no_places; j++) {
            place_table[i][j] = i + j;
            printf("%d ", place_table[i][j]);
        }
        printf("\n");
    }

    // Reallocate memory for a larger array
    int new_no_places = 5; // New size of the array

    // Allocate a new block of memory
    int **new_place_table = (int **)malloc((new_no_places + 1) * sizeof(int *));
    for (i = 1; i <= new_no_places; i++)
        new_place_table[i] = (int *)malloc((new_no_places + 1) * sizeof(int));

    // Copy elements from the old array to the new array
    for (i = 1; i <= no_places; i++) {
        for (j = 1; j <= no_places; j++) {
            new_place_table[i][j] = place_table[i][j];
        }
        free(place_table[i]); // Free memory occupied by inner arrays of the old array
    }
    free(place_table); // Free memory occupied by the old array

    place_table = new_place_table; // Update the pointer to the new array
    no_places = new_no_places; // Update the size variable

    // Print the new array
    for (i = 1; i <= no_places; i++) {
        for (j = 1; j <= no_places; j++) {
            printf("%d ", place_table[i][j]);
        }
        printf("\n");
    }

    // Free memory occupied by the new array
    for (i = 1; i <= no_places; i++)
        free(place_table[i]);
    free(place_table);

    return 0;
}

/*
Personal Comments:

No realloc, no problem?
*/