#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplusTree.h"

int main()
{
    // Initialize the B+ tree
    BPTree *bPTree = createBPTree();

	
    printf("\n\n**************************************************************************************\n\n");
    printf("\t\tDatabase Indexing and Management using B+ Tree ");
    printf("\n\n**************************************************************************************\n\n");
    int option;
    do
    {
        printf("\nPlease provide the queries with respective keys :\n\n");
        printf("Press 1 for : Insertion\n");
        printf("Press 2 for : Search\n");
        printf("Press 3 for : Print Tree\n");
        printf("Press 4 for : Delete Key In Tree\n");
        printf("Press 5 for : taking input from given txt file\n");
        printf("Press 6 for : EXIT.!!\n");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
        {
            int rollNo;
            int age, marks;
            char name[100],dpt[100];

            printf("Please provide the rollNo: ");
            scanf("%d", &rollNo);

            printf("\nWhat's the Name : ");
            scanf("%s",name);
            printf("\nWhat's the Department : ");
            scanf("%s",dpt);
            printf("\nWhat's the Age : ");
            scanf("%d",&age );
            printf("\nWhat are the obtained marks : ");
            scanf("%d", &marks);

            char fileName[100];
            sprintf(fileName, "DBFiles/%d.txt", rollNo);
            FILE *filePtr = fopen(fileName, "w");
            fprintf(filePtr, "%s %s %d %d\n", name,dpt, age, marks);
            fclose(filePtr);

            insert(bPTree, rollNo, filePtr);

            printf("Insertion of roll No: %d Successful\n", rollNo);
            break;
        }
        case 2:
        {
            int rollNo;
            printf("What's the RollNo to Search? ");
            scanf("%d", &rollNo);

            search(bPTree, rollNo);
            break;
        }
        case 3:
        {

            printf("\nHere is your File Structure:\n");
            if(!bPTree->root){
		    printf("Tree is empty!\n");
	    }
	    else{
		    display(bPTree->root, 0);
	    }
            printf("\n");
            break;
        }
        case 4:
        {
            printf("Showing you the Tree, Choose a key from here:\n");
            display(bPTree->root, 0);

            int tmp;
            printf("Enter a key to delete: ");
            scanf("%d", &tmp);

            removeKey(bPTree->root, tmp);

            // Displaying
            // display(bPTree->root, 0);
            break;
        }
        case 5:
        {
            printf("Taking input from given input.txt file\n");
            // taking input from txt file
            FILE *inputFile = fopen("input.txt", "r");
            if (inputFile == NULL)
            {
                printf("Failed to open input file.\n");
                return 1;
            }

            char line[100];
            while (fgets(line, sizeof(line), inputFile) != NULL)
            {
                int rollNo, age, marks;
                char name[100],dpt[100];

                // Parse the line to extract the data
                sscanf(line, "%d,%[^,],%[^,],%d,%d", &rollNo, name,dpt, &age, &marks);

                // Create a file name based on the rollNo
                char fileName[100];
                sprintf(fileName, "DBFiles/%d.txt", rollNo);

                // Open the file for writing
                FILE *filePtr = fopen(fileName, "w");
                if (filePtr == NULL)
                {
                    printf("Failed to open file for rollNo %d.\n", rollNo);
                    continue;
                }

                // Write the data to the file
                fprintf(filePtr, "%s %s %d %d\n", name,dpt, age, marks);

                // Insert the key into the B+ tree
                insert(bPTree, rollNo, filePtr);

                // Close the file
                fclose(filePtr);
            }

            // Close the input file
            fclose(inputFile);
            printf("input.txt file is successfully read\n");
            break;
        }
        default:
            option = 6;
            break;
        }
    } while (option != 6);

    return 0;
}
