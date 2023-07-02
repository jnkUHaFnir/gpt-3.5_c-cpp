/*
I wrote a function for inserting a node at the end of a linked list. When I run the program, and when I call this function, the program just stops working.
Here is the function:

void insert(node **head, int x) {
    node *new = malloc(sizeof(node));
    new->data = x;
    new->next = NULL;

    node *temp = *head;

    while (temp != NULL) {
        temp = temp->next;
    }
    temp->next = new;
}

Can you tell me where did it go wrong?
*/

void insert(node **head, int x) {
    node *new = malloc(sizeof(node));
    new->data = x;
    new->next = NULL;

    if (*head == NULL) {
        // Special case: if the linked list is empty, make the new node the head
        *head = new;
    } else {
        node *temp = *head;

        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
    }
}

/*
Personal Comments:

line 26 is a direct countermeasure to the 'Null terminated string issue'
in the original Stack Overflow post.
*/