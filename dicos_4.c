/*
I need to count how many strings there is in a "string list". Each string ends as usual with a NUL char ('\0'), and the list ends with two NUL chars in succession.

I wrote a function but I keep getting a segmentation fault:

int numStrsInList(const char* strList) {
    int count = 0;
    int flag = 0;
    if(!(*strList))
        return -1;

    while (flag != 2) {
        if (!(*strList)) {
            count++;
            flag++;
        }
        else
            flag = 0;
        strList++;
    }
    return count;
}

*/

#include <stdio.h>

int numStrsInList(const char* strList) {
    int count = 0;
    int flag = 0;

    if (strList == NULL || !(*strList))
        return -1;

    while (*strList != '\0' || flag < 2) {
        if (*strList == '\0') {
            flag++;
        } else {
            flag = 0;
        }
        strList++;
    }

    return count;
}

int main() {
    const char strList[] = "string1\0string2\0string3\0\0";
    int numStrings = numStrsInList(strList);

    printf("Number of strings in the list: %d\n", numStrings);

    return 0;
}


/*
Personal comments:

- Reduces risk of out-of-bound error by not using strlen
- Seems to have syntactical error (why is 'count' not incremented?)
*/