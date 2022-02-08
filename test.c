#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int value;
    struct node *next;
} node_t;

void printList(node_t *head)
{
    node_t *temporary = head;

    while (temporary != NULL)
    {
        printf("%d - ", temporary->value);
        temporary = temporary->next;
    }
    printf("\n");
}

node_t *create_new_node(int value)
{
    node_t *result = malloc(sizeof(node_t));
    result->value = value;
    result->next = NULL;
    return result;
}

void *insert_at_head(node_t **head, node_t *node_to_insert)
{
    node_to_insert->next = *head;
    *head = node_to_insert;
}

node_t *find_node(node_t *head, int value)
{
    node_t *tmp = head;
    while (tmp != NULL)
    {
        if (tmp->value == value)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void insert_after_mode(node_t *node_to_insert_after, node_t *newnode)
{
    newnode->next = node_to_insert_after->next;
    node_to_insert_after->next = newnode;
}

void delete_node(node_t *head, int value)
{
    node_t *tmp = head;
    node_t *delnode, *next_node;

    delnode = find_node(head, value);

    if (delnode == NULL)
    {
        printf("Node does not exist!\n");
    }
    else
    {
        while (tmp != NULL)
        {
            next_node = tmp->next;
            if (next_node->value == value)
            {
                tmp->next = next_node->next;
                printf("Node deleted!\n");
                break;
            }
            tmp = tmp->next;
        }
        free(delnode);
    }
}

int main(int argc, char *argv[])
{
    node_t *head = NULL;
    node_t *tmp;
    int i;

    for (i = 0; i < 25; i++)
    {
        tmp = create_new_node(i);
        insert_at_head(&head, tmp);
    }

    printList(head);

    delete_node(head, 18);

    printList(head);

    free(tmp);
    return 0;
}