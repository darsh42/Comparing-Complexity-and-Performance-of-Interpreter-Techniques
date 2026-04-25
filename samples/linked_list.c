#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};

int main() {
    int i;
    int count = 10000; // Adjust for your emulator's speed
    struct node *head = NULL;
    struct node *curr;

    // Build list
    for (i = 0; i < count; i++) {
        struct node *new_node = malloc(sizeof(struct node));
        new_node->data = i;
        new_node->next = head;
        head = new_node;
    }

    // Traverse multiple times to simulate workload
    long long sum = 0;
    for (int r = 0; r < 100; r++) {
        curr = head;
        while (curr) {
            sum += curr->data;
            curr = curr->next;
        }
    }

    printf("LL sum: %lld\n", sum);
    return 0;
}
