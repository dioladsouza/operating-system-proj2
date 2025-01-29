#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef struct node
{
    int data;
    int priority; // Larger number means higher priority.
    struct node *nextptr;
} Node;

// Create node function
Node* createNode(int data, int priority)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = data;
    temp->priority = priority;
    temp->nextptr = NULL;
    return temp;
}

//Function to push element into PQ based on priority
Node* push(Node* head, int data, int priority) 
{ 
    Node* start = head; 

    // Create new Node 
    Node* temp = createNode(data, priority); 
 
    if (head->priority < priority) { 

        // Insert New Node before head 
        temp->nextptr = head; 
        head = temp; 
    } 
    else { 
        while (start->nextptr != NULL && start->nextptr->priority > p) { 
            start = start->next; 
        } 

        temp->nextptr = start->nextptr; 
        start->nextptr = temp; 
    } 
    return head;
} 

Node* pop(Node* head) 
{ 
    Node* temp = head; 
    head = head->nextptr; 
    free(temp); 
    return head;
} 

int isEmpty(Node* head) 
{ 
    return (head == NULL); 
} 



