/*
This program creates random numbers using rand(). The user inputs how many random numbers will be created as an integer. The program also finds the largest number.

This is my code:


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

  int main(void)
{
  srand(time(NULL));
  int size;
  int i;
  int array[0];

  printf("\nSize of random array: ");
  scanf("%d", &size);

  for (i = 0; i <  size; i++){
    array[i] = rand() % 100 + 1;;
  }

  for (i=0; i < size; i++){
       printf("%d ", array[i]);
      }

 int largest =0;

for (i = 1; i < size; i++)
{
      if (largest < array[i])
              largest = array[i];
      }
printf("\n largest element present in the given array is : %d\n", largest);

    return 0;
}


I'm using an online C compiler. (I was using Atom editor but my code doesn't do anything in there). The output should be something like this:

Size of random array: 6
0 6 10 21
largest element present in the given array is : 21

But I'm getting this:

Size of random array: 6
0 6 0 0 -433525051 32757
largest element present in the given array is : 32757

Why do I'm getting such a big numbers? how can I fix this?
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  srand(time(NULL));
  int size;
  int i;

  printf("Size of random array: ");
  scanf("%d", &size);

  int* array = (int*)malloc(size * sizeof(int));  // Dynamically allocate memory

  for (i = 0; i < size; i++) {
    array[i] = rand() % 100 + 1;
  }

  for (i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }

  int largest = array[0];  // Initialize largest with the first element

  for (i = 1; i < size; i++) {
    if (largest < array[i])
      largest = array[i];
  }

  printf("\nLargest element present in the given array is: %d\n", largest);

  free(array);  // Free the dynamically allocated memory

  return 0;
}

/*
Personal Comments:

The writer of the original code did not consider accuracy / error checks regarding their code.
The resulting code is mostly similar to OP's code, resulting in several inaccuracies and vulneraiblities.
*/