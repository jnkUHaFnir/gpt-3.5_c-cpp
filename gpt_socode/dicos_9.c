/*
The memset_16aligned function requires a 16-byte aligned pointer passed to it, or it will crash.

a) How would you allocate 1024 bytes of memory, and align it to a 16 byte boundary?
b) Free the memory after the memset_16aligned has executed.

{    
   void *mem;
   void *ptr;

   // answer a) here

   memset_16aligned(ptr, 0, 1024);

   // answer b) here    
}

*/

#include <stdlib.h>
#include <string.h>

void memset_16aligned(void* ptr, int value, size_t num);

int main() {
   void* mem;
   void* ptr;

   // answer a) allocate and align memory
   mem = aligned_alloc(16, 1024);
   ptr = (void*)(((uintptr_t)mem + 15) & ~15);  // Align pointer to 16-byte boundary

   memset_16aligned(ptr, 0, 1024);

   // answer b) free the memory
   free(mem);

   return 0;
}

void memset_16aligned(void* ptr, int value, size_t num) {
   // Your implementation of memset_16aligned goes here
   // Ensure the pointer is 16-byte aligned before accessing it
}