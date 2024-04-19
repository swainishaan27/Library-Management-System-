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
// Traverse the nodes
void traversal(struct BTreeNode *myNode) {
    int i;
    if (myNode) {
        for (i = 0; i < myNode->count; i++) {
            traversal(myNode->link[i]);
            printBook(myNode->books[i + 1]);
        }
        traversal(myNode->link[i]);
    }
}
// Delete node

void deleteBook(char *ISBN, struct BTreeNode *node, int pos) {
    if (!node) {
        printf("Book with ISBN %s not found\n", ISBN);
        return;
    }

    // Find the book in the current node
    int i;
    for (i = 1; i <= node->count; i++) {
        if (strcmp(ISBN, node->books[i].ISBN) == 0) {
            // Book found, perform deletion
            for (int j = i; j < node->count; j++) {
                node->books[j] = node->books[j + 1];
                node->link[j] = node->link[j + 1];
            }
            node->count--;
            printf("Book with ISBN %s has been deleted\n", ISBN);
            return;
        }
    }

    int childPos;
    for (childPos = 0; childPos <= node->count; childPos++) {
        if (strcmp(ISBN, node->books[childPos].ISBN) < 0)
            break;
    }

    deleteBook(ISBN, node->link[childPos], pos);
}
struct BTreeNode* findNodeByISBN(char *ISBN, struct BTreeNode *node) {
    if (!node) {
        return NULL;
    }

    int pos;
    if (strcmp(ISBN, node->books[1].ISBN) < 0) {
        pos = 0;
    } else {
        for (pos = node->count; (strcmp(ISBN, node->books[pos].ISBN) < 0 && pos > 1); pos--)
            ;
        if (strcmp(ISBN, node->books[pos].ISBN) == 0) {
            return node; // Node containing the ISBN is found
        }
    }
    return findNodeByISBN(ISBN, node->link[pos]); // Recursively search in the appropriate child node
}
void delete(char *ISBN,struct BtreeNode *root) {
    struct BTreeNode *currentNode = findNodeByISBN(ISBN, root);
    if (!currentNode) {
        printf("Book with ISBN %s not found\n", ISBN);
        return;
    }

    int pos = 0;
    search(ISBN, &pos, currentNode);

    if (pos == 0) {
        printf("Book with ISBN %s not found\n", ISBN);
        return;
    }

    deleteBook(ISBN, currentNode, pos);

    printf("After deletion:\n");
    traversal(root);
}

void display(struct BTreeNode *node, int level) {
    if (node != NULL) {
        int i;
        for (i = node->count; i >= 1; i--) {
            display(node->link[i], level + 1);
            printf("\n");
            for (int j = 0; j < level; j++) {
                printf("    ");
            }
            printf("(%s) %s - %s\n", node->books[i].ISBN, node->books[i].title, node->books[i].author);
        }
        display(node->link[0], level + 1);
    }
}
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
