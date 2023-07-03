/*
I want to malloc an integer pointer present inside a structure.
If in future I need to expand the array, I use realloc.
something like this:

typedef struct {
   int *temp,count
}foo;

main()
{
    foo *fake = (foo*)malloc(1*sizeof(foo));
    fake.count = 0;

    ++(fake.count);
    fake.temp = (int*)malloc((fake.count)*sizeof(int));

    // I do something

    // now i want to realloc "temp" to ++(fake.count)


Is this way of doing that correct?

    ++(fake.count);
    fake.temp = (int*)realloc(fake.temp,(fake.count)*sizeof(int));
*/

#include <stdlib.h>

typedef struct {
   int *temp;
   int count;
} foo;

int main() {
    foo *fake = (foo*)malloc(10 * sizeof(foo)); // Allocate memory for an array of 10 foo structures
    fake->count = 0;

    ++(fake->count);
    fake->temp = (int*)malloc((fake->count) * sizeof(int));

    /* Do something */

    /* Now I want to realloc "temp" to ++(fake->count) */
    ++(fake->count);
    fake->temp = (int*)realloc(fake->temp, (fake->count) * sizeof(int));

    /* Clean up memory */
    free(fake->temp);
    free(fake);

    return 0;
}

// NOTE: room for self-patch regarding realloc result check