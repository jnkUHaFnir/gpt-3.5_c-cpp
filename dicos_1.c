/*
Why does malloc not work sometimes?
I'm porting a C project from Linux to Windows. On Linux it is completely stable. On Windows, it's working well most times, but sometimes I got a segmentation fault.
I'm using Microsoft Visual Studio 2010 to compile and debug and looks like sometimes my malloc calls simply doesn't allocate memory, returning NULL. The machine has free memory; it already passed through that code a thousand times, but it still happens in different locations.
Like I said, it doesn't happen all the time or in the same location; it looks like a random error.
Is there something I have to be more careful on Windows than on Linux? What can I be doing wrong?
*/

// General question

#include <stdio.h>
#include <stdlib.h>

void useAfterFree() {
  int* ptr = (int*)malloc(sizeof(int));
  free(ptr);
  *ptr = 10; // Using freed memory
}

int main() {
  for (int i = 0; i < 1000; i++) {
    useAfterFree();
  }

  return 0;
}

/*
Personal Comments:

Resulting code is completely different to original code
*/
