/*
I tried to delete a file and then rename the temporary file in the name of the deleted file from a function that did not work. Please help me.

boolean delete_user(char user_name[256])   //only for Admin
{
    boolean status = FALSE;//what does the function return
    User_Data *ud = NULL;
    boolean found_user = FALSE;
    FILE *new_file = NULL;
    FILE *fp = NULL;
    char user_name_to_copy[256];
    char password_to_copy[256];
    char old_file_name[256];

    ud = find_user(user_name);
    if (ud == NULL) {
        printf("The username wasn't found!\n");
        return FALSE;
    }
    if (!strcmp(ud->permission_type, "Admin")) {
        printf("Cant delete an admin.");
        return FALSE;
    } else {
        // the user to delete was found 
        new_file = fopen("duplicate.txt", "wt");
        strcpy(old_file_name, ud->permission_type);
        strcat(old_file_name, "s.txt"); //the name of the file is in plural and ends with .txt
        fp = fopen(old_file_name, "rt");
        while (!feof(fp)) {
            //copy all the users except the user to delete the new file 
            fscanf(fp, "%s %s\n", user_name_to_copy, password_to_copy);
            if (strcmp(user_name_to_copy, user_name)) {
                fprintf(new_file, "%s %s\n", user_name_to_copy, password_to_copy);
            }
        }
        fclose(fp);
        fclose(new_file);
        printf(" %d ", remove(old_file_name));
        rename("duplicate.txt", old_file_name);
        remove("duplicate.txt");
        return TRUE;
    }
}

This function does not work when I call it from another function, but from the main the function works just fine.
*/

/*******************Generated code starts here*******************/

#include <stdio.h>
#include <string.h>

#define BASE_DIR "/path/to/base/directory/"  // Replace with the actual base directory path

typedef enum { FALSE, TRUE } boolean;

typedef struct {
    char permission_type[256];
} User_Data;

User_Data* find_user(const char* user_name) {
    // Implementation of find_user function
    return NULL;
}

boolean delete_user(char user_name[256]) {
    boolean status = FALSE;
    User_Data* ud = NULL;
    boolean found_user = FALSE;
    FILE* new_file = NULL;
    FILE* fp = NULL;
    char user_name_to_copy[256];
    char password_to_copy[256];
    char old_file_name[256];
    char base_path[256];
    
    strcpy(base_path, BASE_DIR);

    ud = find_user(user_name);
    if (ud == NULL) {
        printf("The username wasn't found!\n");
        return FALSE;
    }

    if (!strcmp(ud->permission_type, "Admin")) {
        printf("Can't delete an admin.\n");
        return FALSE;
    } else {
        // the user to delete was found 
        char temp_file[256];
        strcpy(temp_file, base_path);
        strcat(temp_file, "duplicate.txt");
        new_file = fopen(temp_file, "wt");

        strcpy(old_file_name, base_path);
        strcat(old_file_name, ud->permission_type);
        strcat(old_file_name, "s.txt");

/************************************************************************/
        fp = fopen(old_file_name, "rt");
        if (fp == NULL) {
            printf("Failed to open the file.\n");
            fclose(new_file);
            return FALSE;
        }

        while (!feof(fp)) {
            // copy all the users except the user to delete to the new file 
            fscanf(fp, "%s %s\n", user_name_to_copy, password_to_copy);
            if (strcmp(user_name_to_copy, user_name)) {
                fprintf(new_file, "%s %s\n", user_name_to_copy, password_to_copy);
            }
        }
        fclose(fp);
/************************************************************************/

        fclose(new_file);

        // Remove the original file
        printf("%d\n", remove(old_file_name));

        // Rename the temporary file to the original file name
        strcpy(temp_file, base_path);
        strcat(temp_file, "duplicate.txt");
        strcpy(old_file_name, base_path);
        strcat(old_file_name, ud->permission_type);
        strcat(old_file_name, "s.txt");

        printf("%d\n", rename(temp_file, old_file_name));

        return TRUE;
    }
}

int main() {
    char user_name[256];
    printf("Enter the username to delete: ");
    scanf("%s", user_name);

    delete_user(user_name);

    return 0;
}
