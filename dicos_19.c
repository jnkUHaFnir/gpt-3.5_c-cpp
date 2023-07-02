/*
I have a struct

typedef struct
{
int size; //size of array
int array*
} 

How do I allocate memory for the int array using the size variable and malloc?
*/


#include <stdlib.h>

typedef struct {
    int size;   // size of array
    int* array; // pointer to int array
} MyStruct;

int main (void) {

    MyStruct myStruct;
    myStruct.size = 10; // set the size of the array

    myStruct.array = (int*)malloc(myStruct.size * sizeof(int));

    if (myStruct.array == NULL) {
        // Memory allocation failed
        // Handle the error
    } else {
        // Memory allocation successful
        // Access and manipulate the array
    }

    // Accessing and manipulating the array
    for (int i = 0; i < myStruct.size; i++) {
        myStruct.array[i] = i + 1; // example: storing values 1 to 10 in the array
    }

    free(myStruct.array);
}

