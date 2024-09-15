#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bplusTree.h"




// create functions for node and tree

BPTree *createBPTree()
{
    BPTree *bPTree = (BPTree *)malloc(sizeof(BPTree));
    bPTree->maxChildInt = MAX_CHILD;
    bPTree->maxNodeLeaf = MAX_NODE;
    bPTree->root = NULL;
    return bPTree;
}
Node *createNode(int isLeaf)
{

    Node *node = (Node *)malloc(sizeof(Node));

    node->numKeys = 0;

    node->keys = (int *)malloc((MAX_CHILD - 1) * sizeof(int));

    if (isLeaf)
    {
        node->ptrs.dataPtr = (FILE **)malloc(MAX_NODE * sizeof(FILE *));
        // Allocate memory for the array of data pointers in the leaf node
    }
    else
    {
        node->ptrs.childNodes = (Node **)malloc(MAX_CHILD * sizeof(Node *));
        // Allocate memory for the array of child nodes in the internal node
    }

    node->parent = NULL;

    node->isLeaf = isLeaf;

    return node;
}







//--------------------------------------------- insert operations---------------------------------------------


void insert(BPTree *tree, int key, FILE *filePtr)
{
    if (tree->root == NULL)
    {
        tree->root = (Node *)malloc(sizeof(Node));
        tree->root->isLeaf = 1;
        tree->root->keys = (int *)malloc(MAX_NODE * sizeof(int));
        tree->root->keys[0] = key;
        tree->root->ptrs.dataPtr = (FILE **)malloc(MAX_NODE * sizeof(FILE *));
        tree->root->ptrs.dataPtr[0] = filePtr;
        tree->root->numKeys = 1;

        printf("%d: I AM ROOT!!\n", key);
        return;
    }
    else
    {
        Node *cursor = tree->root;
        Node *parent = NULL;
        while (cursor->isLeaf == 0)
        {
            parent = cursor;
            for (int i = 0; i < cursor->numKeys; i++)
            {
                if (key < cursor->keys[i])
                {
                    cursor = cursor->ptrs.childNodes[i];
                    break;
                }
                if (i == cursor->numKeys - 1)
                {
                    cursor = cursor->ptrs.childNodes[i + 1];
                    break;
                }
            }
        }

        // Insert the key in the leaf node
        if (cursor->numKeys < MAX_NODE)
        {
            // Insert the key in the correct position while maintaining sorted order
            int i = cursor->numKeys - 1;
            while (i >= 0 && key < cursor->keys[i])
            {
                cursor->keys[i + 1] = cursor->keys[i];
                cursor->ptrs.dataPtr[i + 1] = cursor->ptrs.dataPtr[i];
                i--;
            }
            cursor->keys[i + 1] = key;
            cursor->ptrs.dataPtr[i + 1] = filePtr;
            cursor->numKeys++;

            printf("%d: INSERTED INTO LEAF NODE\n", key);
        }
        else
        {
            // Leaf node is full, split it into two nodes
            Node *newLeaf = createNode(1);
            int tempKeys[MAX_NODE + 1];
            FILE *tempPtrs[MAX_NODE + 1];
            for (int i = 0; i < MAX_NODE; i++)
            {
                tempKeys[i] = cursor->keys[i];
                tempPtrs[i] = cursor->ptrs.dataPtr[i];
            }
            int i = MAX_NODE - 1;
            while (i >= 0 && key < tempKeys[i])
            {
                tempKeys[i + 1] = tempKeys[i];
                tempPtrs[i + 1] = tempPtrs[i];
                i--;
            }
            tempKeys[i + 1] = key;
            tempPtrs[i + 1] = filePtr;

            cursor->numKeys = (MAX_NODE + 1) / 2;
            newLeaf->numKeys = MAX_NODE + 1 - (MAX_NODE + 1) / 2;

            for (int j = 0; j < cursor->numKeys; j++)
            {
                cursor->keys[j] = tempKeys[j];
                cursor->ptrs.dataPtr[j] = tempPtrs[j];
            }
            for (int j = 0, k = cursor->numKeys; j < newLeaf->numKeys; j++, k++)
            {
                newLeaf->keys[j] = tempKeys[k];
                newLeaf->ptrs.dataPtr[j] = tempPtrs[k];
            }

            newLeaf->ptrs.dataPtr[newLeaf->numKeys] = cursor->ptrs.dataPtr[MAX_NODE];
            cursor->ptrs.dataPtr[MAX_NODE] = (FILE *)newLeaf;

            if (cursor == tree->root)
            {
                // If the split occurs at the root, create a new root
                Node *newRoot = createNode(0);
                newRoot->keys[0] = newLeaf->keys[0];
                newRoot->ptrs.childNodes[0] = cursor;
                newRoot->ptrs.childNodes[1] = newLeaf;
                newRoot->numKeys = 1;
                cursor->parent = newRoot;
                newLeaf->parent = newRoot;
                tree->root = newRoot;
            }
            else
            {
                // If the split occurs below the root, update the parent node
                insertInternal(tree, parent, newLeaf->keys[0], newLeaf);
            }

            printf("%d: INSERTED INTO LEAF NODE, SPLIT OCCURRED\n", key);
        }
    }
}


void insertInternal(BPTree *tree, Node *parent, int key, Node *right)
{
    if (parent->numKeys < MAX_NODE)
    {
        // Insert the key in the correct position while maintaining sorted order
        int i = parent->numKeys - 1;
        while (i >= 0 && key < parent->keys[i])
        {
            parent->keys[i + 1] = parent->keys[i];
            parent->ptrs.childNodes[i + 2] = parent->ptrs.childNodes[i + 1];
            i--;
        }
        parent->keys[i + 1] = key;
        parent->ptrs.childNodes[i + 2] = right;
        parent->numKeys++;
    }
    else
    {
        // Internal node is full, split it into two nodes
        Node *newInternal = createNode(0);
        int tempKeys[MAX_NODE + 1];
        Node *tempPtrs[MAX_NODE + 2];
        for (int i = 0; i < MAX_NODE; i++)
        {
            tempKeys[i] = parent->keys[i];
            tempPtrs[i + 1] = parent->ptrs.childNodes[i + 1];
        }
        int i = MAX_NODE - 1;
        while (i >= 0 && key < tempKeys[i])
        {
            tempKeys[i + 1] = tempKeys[i];
            tempPtrs[i + 2] = tempPtrs[i + 1];
            i--;
        }
        tempKeys[i + 1] = key;
        tempPtrs[i + 2] = right;

        parent->numKeys = (MAX_NODE + 1) / 2;
        newInternal->numKeys = MAX_NODE - (MAX_NODE + 1) / 2;

        for (int j = 0; j < parent->numKeys; j++)
        {
            parent->keys[j] = tempKeys[j];
            parent->ptrs.childNodes[j + 1] = tempPtrs[j + 1];
        }
        for (int j = 0, k = parent->numKeys + 1; j < newInternal->numKeys; j++, k++)
        {
            newInternal->keys[j] = tempKeys[k];
            newInternal->ptrs.childNodes[j + 1] = tempPtrs[k];
        }

        if (parent == tree->root)
        {
            // If the split occurs at the root, create a new root
            Node *newRoot = createNode(0);
            newRoot->keys[0] = parent->keys[parent->numKeys];
            newRoot->ptrs.childNodes[0] = parent;
            newRoot->ptrs.childNodes[1] = newInternal;
            newRoot->numKeys = 1;
            parent->parent = newRoot;
            newInternal->parent = newRoot;
            tree->root = newRoot;
        }
        else
        {
            // If the split occurs below the root, update the parent node
            insertInternal(tree, parent->parent, parent->keys[parent->numKeys], newInternal);
        }
    }
}










// -------------------------------searching function--------------------------------


// Get the root node of the B+ tree
Node *BPTree_getRoot(struct BPTree *tree)
{
    return tree->root;
}


void search(BPTree *bPTree, int key)
{
    Node *cursor = BPTree_getRoot(bPTree); // Start from the root node

    // Traverse the tree until reaching a leaf node
    while (!cursor->isLeaf)
    {
        int i;
        for (i = 0; i < cursor->numKeys; i++)
        {
            if (key < cursor->keys[i])
            {
                break;
            }
        }
        // Move to the appropriate child node
        cursor = cursor->ptrs.childNodes[i];
    }

    // Search for the key in the leaf node
    int i;
    for (i = 0; i < cursor->numKeys; i++)
    {
        if (key == cursor->keys[i])
        {
            printf("Key %d found!\n", key);
            return;
        }
    }

    printf("Key %d not found.\n", key);
}








// -------------------------------deleting function--------------------------------


// Find the parent node of the given cursor node and child node
Node **BPTree_findParent(struct BPTree *tree, Node *cursor, Node *child)
{
    if (cursor == NULL || cursor->isLeaf)
        return NULL;

    int i;
    Node **parentPtr = NULL;

    for (i = 0; i < cursor->numKeys; i++)
    {
        if (child == cursor->ptrs.childNodes[i])
        {
            parentPtr = &cursor->ptrs.childNodes[i];
            break;
        }
        else if (child->keys[0] < cursor->keys[i])
        {
            parentPtr = BPTree_findParent(tree, cursor->ptrs.childNodes[i], child);
            break;
        }
    }

    if (parentPtr == NULL)
    {
        parentPtr = BPTree_findParent(tree, cursor->ptrs.childNodes[cursor->numKeys], child);
    }

    return parentPtr;
}

// Delete the key from the given node and rearrange its elements


Node *findParent(struct Node *root, struct Node *cursor)
{
    if (root == cursor)
        return NULL;

    Node *parent = NULL;
    for (int i = 0; i < root->numKeys; i++)
    {
        if (cursor->keys[0] < root->keys[i])
        {
            if (root->isLeaf)
                return NULL;
            else
            {
                parent = root;
                root = root->ptrs.childNodes[i];
                i = -1; // Resetting the index to 0
            }
        }
        else if (i == root->numKeys - 1)
        {
            if (root->isLeaf)
                return NULL;
            else
            {
                parent = root;
                root = root->ptrs.childNodes[i + 1];
                i = -1; // Resetting the index to 0
            }
        }
    }
    return parent;
}







//------------------------------------ display function -----------------------------



void display(Node *ptr, int blanks)
{
    if(ptr)
    {
        int i;

        if (ptr->isLeaf)
        {
            // Print the required number of blanks/spaces
            for (i = 1; i <= blanks; i++)
                printf(" ");

            // Print the keys in the leaf node
            for (i = 0; i < ptr->numKeys; i++)
                printf("%d ", ptr->keys[i]);
            printf("\n");
        }
        else
        {
            // Recursively display child nodes with increased blanks
            display(ptr->ptrs.childNodes[0], blanks + 10);

            // Print the required number of blanks/spaces
            for (i = 1; i <= blanks; i++)
                printf(" ");

            // Print the keys in the internal node
            for (i = 0; i < ptr->numKeys; i++)
                printf("%d ", ptr->keys[i]);
            printf("\n");

            for (i = 0; i <= ptr->numKeys; i++)
                display(ptr->ptrs.childNodes[i + 1], blanks + 10);
        }
    }
}


