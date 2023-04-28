#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PAGES 10
#define FRAMES 4

typedef struct node {
    int value;
    struct node* next;
    struct node* prev;
} node;

node* head = NULL;
node* tail = NULL;

void insert(int value) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->value = value;
    new_node->next = NULL;
    if (head == NULL) {
        new_node->prev = NULL;
        head = new_node;
        tail = new_node;
    } else {
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
    }
}

void delete(node* node_to_delete) {
    if (node_to_delete == head) {
        head = node_to_delete->next;
        head->prev = NULL;
    } else if (node_to_delete == tail) {
        tail = node_to_delete->prev;
        tail->next = NULL;
    } else {
        node_to_delete->prev->next = node_to_delete->next;
        node_to_delete->next->prev = node_to_delete->prev;
    }
    free(node_to_delete);
}

node* search(int value) {
    node* temp = head;
    while (temp != NULL) {
        if (temp->value == value) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void print_list() {
    node* temp = head;
    while (temp != NULL) {
        printf("%d ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

int main() {
    FILE* fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("Could not open file\n");
        return 1;
    }
    
    int page_table[PAGES];
    for (int i = 0; i < PAGES; i++) {
        page_table[i] = -1;
    }
    
    int addr;
    int page_faults = 0;
    while (fscanf(fp, "%d", &addr) == 1) {
        int page_number = addr / 10;
        int offset = addr % 10;
        if (page_table[page_number] == -1) {
            page_faults++;
            if (tail == NULL || tail->value < FRAMES - 1) {
                page_table[page_number] = tail == NULL ? 0 : tail->value + 1;
                insert(page_table[page_number]);
            } else {
                page_table[head->value] = -1;
                delete(head);
                page_table[page_number] = tail->value;
                insert(page_table[page_number]);
            }
            printf("Page fault for address %d (page %d)\n", addr, page_number);
        } else {
            printf("Page hit for address %d (page %d)\n", addr, page_number);
            node* temp = search(page_table[page_number]);
            delete(temp);
            insert(page_table[page_number]);
        }
        print_list();
    }
    
    fclose(fp);
    printf("Page faults: %d
