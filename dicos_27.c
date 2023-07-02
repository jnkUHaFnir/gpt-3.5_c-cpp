/*
I am really trying to learn if someone wouldn't mind to educate me in the principles I may be missing out on here. I thought I had everything covered but it seems I am doing something incorrectly.
The following code gives me a segmentation fault, and I cannot figure out why? I am adding the & in front of the arguments name being passed in to fscanf.

int word_size = 0;
#define HASH_SIZE 65536
#define LENGTH = 45
node* global_hash[HASH_SIZE] = {NULL};

typedef struct node {
  char word[LENGTH + 1];
  struct node* next;
} node;

int hash_func(char* hash_val){
    int h = 0;
    for (int i = 0, j = strlen(hash_val); i < j; i++){
        h = (h << 2) ^ hash_val[i];
    }
    return h % HASH_SIZE;
}

bool load(const char *dictionary)
{
    char* string;
    FILE* dic = fopen(dictionary, "r");
    if(dic == NULL){
        fprintf(stdout, "Error: File is NULL.");
        return false;
    }
    while(fscanf(dic, "%ms", &string) != EOF){
        node* new_node = malloc(sizeof(node));
        if(new_node == NULL){
            return false;
        }
        strcpy(new_node->word, string);
        new_node->next = NULL;
        int hash_indx = hash_func(new_node->word);
        node* first = global_hash[hash_indx];
        if(first == NULL){
            global_hash[hash_indx] = new_node;
        } else {
            new_node->next = global_hash[hash_indx];
            global_hash[hash_indx] = new_node;
        }
        word_size++;
        free(new_node);
    }
    fclose(dic);
    return true;
}

dictionary.c:25:16: runtime error: left shift of 2127912344 by 2 places cannot be represented in type 'int'
dictionary.c:71:23: runtime error: index -10167 out of bounds for type 'node *[65536]'
dictionary.c:73:13: runtime error: index -10167 out of bounds for type 'node *[65536]'
dictionary.c:75:30: runtime error: index -22161 out of bounds for type 'node *[65536]'
dictionary.c:76:13: runtime error: index -22161 out of bounds for type 'node *[65536]'
Segmentation fault
*/

