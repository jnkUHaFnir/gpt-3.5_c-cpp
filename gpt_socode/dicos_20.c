/*
I'm looking for a way to convert a char* (coming from argv) to a uint16_t. The command line argument is a port number, and so, can't be > to 65535, nor negative.
Currently, I did this (compiling with -std=gnu99):

#include <stdbool.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

// Converts a string to an unsigned int and stores the result in "res".
bool str_to_uint(const char* str, unsigned long int* res) {
    if (str[0] == '-')
        return false;
    char* first_wrong_character;
    uintmax_t result = strtoumax(str, &first_wrong_character, 10);
    if ((result == UINTMAX_MAX) && (errno == ERANGE))
        return false; // Overflow)
    if ((*str != '\0') && (*first_wrong_character != '\0'))
        return false; // Not everything has been converted
    if ((result == 0) && (str == first_wrong_character))
        return false; // Nothing to convert
    *res = result;
    return true;
}

//Converts a string to an uint16_t and stores the result in "res".
bool str_to_uint16(const char* str, uint16_t* res) {
    unsigned long uint;
    if (!str_to_uint(str, &uint))
        return false;
    if (uint > UINT16_MAX)
        return false;
    *res = (uint16_t)uint;
    return true;
}

I'm not sure it's the best way to do it, so if you could tell me what is the good way?
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool str_to_uint16(const char* str, uint16_t* res) {
    int value;
    if (sscanf(str, "%d", &value) != 1) {
        return false;  // Failed to parse as integer
    }
    if (value < 0 || value > UINT16_MAX) {
        return false;  // Out of range
    }
    *res = (uint16_t)value;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: program_name <port_number>\n");
        return 1;
    }

    uint16_t port;
    if (str_to_uint16(argv[1], &port)) {
        printf("Valid port number: %u\n", port);
    } else {
        printf("Invalid port number\n");
    }

    return 0;
}


// NOTE: Self-patch regarding sscanf

/*
bool str_to_uint16(const char* str, uint16_t* res) {
    char* endptr;
    unsigned long int value = strtoul(str, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || value > UINT16_MAX) {
        return false;  // Error or out of range
    }

    *res = (uint16_t)value;
    return true;
}
*/

// Patch result is almost identical to final version of code in answer post