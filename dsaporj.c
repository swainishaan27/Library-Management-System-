#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 3
//#define MIN 2

struct Book {
    char title[100];
    char author[50];
    char ISBN[15];
    int availability; // 1 for available, 0 for checked out
};

struct BTreeNode {
    struct Book books[MAX + 1];
    int count;
    struct BTreeNode *link[MAX + 1];
};

struct BTreeNode *root;

// Create a node
struct BTreeNode *createNode(struct Book book, struct BTreeNode *child) {
    struct BTreeNode *newNode;
    newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode));
    newNode->books[1] = book;
    newNode->count = 1;
    newNode->link[0] = root;
    newNode->link[1] = child;
    return newNode;
}

// Insert node
void insertNode(struct Book book, int pos, struct BTreeNode *node, struct BTreeNode *child) {
    int j = node->count;
    while (j > pos) {
        node->books[j + 1] = node->books[j];
        node->link[j + 1] = node->link[j];
        j--;
    }
    node->books[j + 1] = book;
    node->link[j + 1] = child;
    node->count++;
}

// Split node
void splitNode(struct Book book, struct Book *pbook, int pos, struct BTreeNode *node,
               struct BTreeNode *child, struct BTreeNode **newNode) {
    int median, j;

    median = (MAX + 1) / 2; // Calculate the median

    *newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode));
    j = median + 1;
    while (j <= MAX) {
        (*newNode)->books[j - median] = node->books[j];
        (*newNode)->link[j - median] = node->link[j];
        j++;
    }
    node->count = median;
    (*newNode)->count = MAX - median;

    if (pos <= median) {
        insertNode(book, pos, node, child);
    } else {
        insertNode(book, pos - median, *newNode, child);
    }
    *pbook = node->books[node->count];
    (*newNode)->link[0] = node->link[node->count];
    node->count--;
}

// Set the book
int setBook(struct Book book, struct Book *pbook, struct BTreeNode *node, struct BTreeNode **child) {
    int pos;
    if (!node) {
        *pbook = book;
        *child = NULL;
        return 1;
    }

    if (strcmp(book.ISBN, node->books[1].ISBN) < 0) {
        pos = 0;
    } else {
        for (pos = node->count; (strcmp(book.ISBN, node->books[pos].ISBN) < 0 && pos > 1); pos--)
            ;
        if (strcmp(book.ISBN, node->books[pos].ISBN) == 0) {
            printf("Book with ISBN %s already exists\n", book.ISBN);
            return 0;
        }
    }
    if (setBook(book, pbook, node->link[pos], child)) {
        if (node->count < MAX) {
            insertNode(*pbook, pos, node, *child);
        } else {
            splitNode(*pbook, pbook, pos, node, *child, child);
            return 1;
        }
    }
    return 0;
}

// Insert the book
void insert(struct Book book) {
    struct Book pbook;
    int flag;
    struct BTreeNode *child;

    flag = setBook(book, &pbook, root, &child);
    if (flag)
        root = createNode(pbook, child);
}

// Search for a book by ISBN
void search(char *ISBN, int *pos, struct BTreeNode *myNode) {
    if (!myNode) {
        return;
    }

    if (strcmp(ISBN, myNode->books[1].ISBN) < 0) {
        *pos = 0;
    } else {
        for (*pos = myNode->count; (*pos > 1 && strcmp(ISBN, myNode->books[*pos].ISBN) < 0); (*pos)--)
            ;
        if (strcmp(ISBN, myNode->books[*pos].ISBN) == 0) {
            printf("Book with ISBN %s is found\n", ISBN);
            return; // Exit the function as the book is found
        }
    }
    search(ISBN, pos, myNode->link[*pos]);
}

// Print details of a book
void printBook(struct Book book) {
    printf("Title: %s\n", book.title);
    printf("Author: %s\n", book.author);
    printf("ISBN: %s\n", book.ISBN);
    if (book.availability == 1) {
        printf("Status: Available\n");
    } else {
        printf("Status: Checked Out\n");
    }
    printf("\n");
}

// Traverse the nodes

int main() {
    struct Book book1 = {"Book1", "Author1", "111111111", 1};
    struct Book book2 = {"Book2", "Author2", "222222222", 1};
    struct Book book3 = {"Book3", "Author3", "333333333", 1};
    struct Book book4 = {"Book4","Author4","444444444",1};
    struct Book book5 = {"Book5", "Author5", "555555555", 1};
    struct Book book6 = {"Book6", "Author6", "666666666", 1};
    struct Book book7 = {"Book7", "Author7", "777777777", 1};
    struct Book book8 = {"Book8", "Author8", "888888888", 1};
    struct Book book9 = {"Book9", "Author9", "999999999", 1};

    insert(book1);
    insert(book2);
    insert(book3);
    insert(book4);
    insert(book5);
    insert(book6);
    insert(book7);
    insert(book8);
    insert(book9);

    printf("\n");
    int pos;
    int delete_pos;
    char ISBN[10];
    //traversal(root);
    display(root,0);
    printf("Enter the ISBN to be deleted\n");
    scanf("%s",&ISBN);
    delete(ISBN,root);
    display(root,0);
    traversal(root);

    return 0;
}