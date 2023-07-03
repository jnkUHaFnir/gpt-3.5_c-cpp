/*

I'm trying to create a function to center text on a 16*8 screen (8 lines and 16 letters in one line), the part with a length < 16 is working fine but when i have more than one line i can't find how to solve the problem, here is my code :


char* align_text(char* text){
    int i=0, j=0, k;
    int modulo, diff;
    int size = strlen(text);
    char space[16];
    char tmp[16];
    char tmp2[16];
    char* tmp3;
    char median[150];
    char* final;

    if(size==16)
        return text;
    else if(size<16)
    {
        diff = 16 - size;
        modulo = diff%2;
        if(modulo==0)
        {
            for(j=0;j<diff/2;j++)
                space[j] = ' ';
            space[j] = '\0';
            strcat(median, space);
            strcat(median, text);
            strcat(median, space);
        }
        else
        {
            for(j=0;j<(int)(diff/2);j++)
                space[j] = ' ';
            space[j] = '\0';
            strcat(median, space);
            strcat(median, text);
            space[j] = ' ';
            space[j+1] = '\0';
            strcat(median, space);
        }
        final = (char*)malloc(strlen(median)*sizeof(char));
        strcpy(final, median);
        return final;
    }
    else
    {
        while(text[i] != '\0')
        {
            if(text[i] == ' ')
            {
                if(strlen(tmp2)>16 && strlen(tmp)<=16)
                {
                    tmp3 = align_text(tmp);
                    if(strlen(median) == 0)
                        strcpy(median, tmp3);
                    else
                        strcat(median, tmp3);
                    free(tmp3);
                    j=0;
                    tmp2[0] = '\0';
                    i = k;
                }
                strcpy(tmp, tmp2);
                tmp2[j++]=text[i++];
                tmp2[j]='\0';
                k = i;
            }
            else
            {
                tmp2[j++]=text[i++];
                tmp2[j]='\0';
            }
        }

        if(strlen(tmp2)>16 && strlen(tmp)<=16)
        {
            tmp3 = align_text(tmp);
            if(strlen(median) == 0)
                strcpy(median, tmp3);
            else
                strcat(median, tmp3);
            free(tmp3);
        }

        j=0;
        tmp2[0] = '\0';
        i = k;
        
        while(text[i] != '\0' )
        {
            tmp2[j] = text[i];
            tmp3 = align_text(tmp2);
            strcat(median, tmp3);
            free(tmp3);
        }

        final = (char*)malloc(strlen(median)*sizeof(char));
        strcpy(final, median);
        return final;
    }
}

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void align_text(char* text, char* aligned_text) {
    int num_lines = 0;
    char* lines[8]; // Array to store individual lines
    char* line;
    char* word;
    char space[16] = "                "; // 16 spaces

    // Split text into lines
    line = strtok(text, "\n");
    while (line != NULL && num_lines < 8) {
        lines[num_lines++] = line;
        line = strtok(NULL, "\n");
    }

    aligned_text[0] = '\0'; // Initialize aligned_text as an empty string

    // Process each line
    for (int i = 0; i < num_lines; i++) {
        line = lines[i];
        int line_length = strlen(line);

        if (line_length >= 16) {
            // Line is longer than or equal to 16 characters, no need for alignment
            strcat(aligned_text, line);
        } else {
            int diff = 16 - line_length;
            int left_spaces = diff / 2;
            int right_spaces = diff - left_spaces;

            strcat(aligned_text, space); // Add left spaces
            strncat(aligned_text, line, line_length); // Add the line
            strncat(aligned_text, space, right_spaces); // Add right spaces
        }

        if (i < num_lines - 1) {
            // Add a newline character if it's not the last line
            strcat(aligned_text, "\n");
        }
    }
}

int main() {
    char text[] = "This is a multi-line text.\n"
                  "It should be aligned in the center of a 16x8 screen.";

    char aligned_text[150];
    align_text(text, aligned_text);

    printf("%s\n", aligned_text);

    return 0;
}

/*
Personal Comments:

Above code has completely different syntax from original answer code.
*/

