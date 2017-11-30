#include <stdio.h>
#include <stdlib.h>

enum {CHAR, INT, DOUBLE};

/*declaration of list node*/
typedef struct Node *PtrToNode;
struct Node
{
    void *pData;
    int dataType;
    PtrToNode pNext;
};

/*create a new list*/
PtrToNode CreateList();

/*get the length of the list*/
int GetLength(PtrToNode l);

/*insert the new node into the head of the list*/
PtrToNode InsertHead(void *pData, int dataType, PtrToNode l);

/*insert the new node into the Kth place*/
PtrToNode InsertKth(void *pData, int k, int dataType, PtrToNode l);

/*insert the new node into the rear of the list*/
PtrToNode InsertRear(void *pData, int dataType, PtrToNode l);

/*find the Kth node*/
PtrToNode FindKth(int k, PtrToNode l);

/*remove the head node of the list*/
PtrToNode RemoveHead(PtrToNode l);

/*remove the Kth node of the list*/
PtrToNode RemoveKth(int k, PtrToNode l);

/*remove the rear node of the list*/
PtrToNode RemoveRear(PtrToNode l);

/*print the whole list*/
void Print(PtrToNode l);

/*remove all the nodes in the list*/
PtrToNode MakeEmpty(PtrToNode l);

/*merge the two lists*/
PtrToNode Merge(PtrToNode lhs, PtrToNode rhs);

/*reverse the list*/
PtrToNode Reverse(PtrToNode l);

int main()
{
    PtrToNode listA, listB;
    void *val;
    
    printf("This is a test program\n");
    printf("Create a list named A\n");
    listA = CreateList();
    printf("Insert 1 into the head of the list A and print list A\n");
    val = (int*)malloc(sizeof(int));
    *(int*)val = 1;
    listA = InsertHead(val, INT, listA);
    Print(listA);
    printf("Insert 'M' into the 3th place of the list A and print list A\n");
    val = (char*)malloc(sizeof(char));
    *(char*)val = 'M';
    listA = InsertKth(val, 3, CHAR, listA);
    Print(listA);
    printf("Insert 0.1 into the rear of the list A and print list A\n");
    val = (double*)malloc(sizeof(double));
    *(double*)val = 0.1;
    listA = InsertRear(val, DOUBLE, listA);
    Print(listA);
    printf("Insert -5.2 into the 2th place of the list A and print list A\n");
    val = (double*)malloc(sizeof(double));
    *(double*)val = -5.2;
    listA = InsertKth(val, 2, DOUBLE, listA);
    Print(listA);
    printf("Insert 'A' into the 3th place of the list A and print list A\n");
    val = (char*)malloc(sizeof(char));
    *(char*)val = 'A';
    listA = InsertKth(val, 3, CHAR, listA);
    Print(listA);
    printf("Insert 10000 into the 5th place of the list A and print list A\n");
    val = (int*)malloc(sizeof(int));
    *(int*)val = 10000;
    listA = InsertKth(val, 5, INT, listA);
    Print(listA);
    printf("Insert 'z' into the head of the list A and print list A\n");
    val = (char*)malloc(sizeof(char));
    *(char*)val = 'z';
    listA = InsertHead(val, CHAR, listA);
    Print(listA);
    printf("Reverse list A and print list A\n");
    listA = Reverse(listA);
    Print(listA);
    printf("Remove the head of list A and print list A\n");
    listA = RemoveHead(listA);
    Print(listA);
    printf("Remove the rear of list A and print list A\n");
    listA = RemoveRear(listA);
    Print(listA);
    printf("Remove the 5th node of list A and print list A\n");
    listA = RemoveKth(5, listA);
    Print(listA);
    printf("Remove the 2th node of list A and print list A\n");
    listA = RemoveKth(2, listA);
    Print(listA);
    printf("Create another list named B\n");
    listB = CreateList();
    printf("Insert -5 into the head of the list B and print list B\n");
    val = (int*)malloc(sizeof(int));
    *(int*)val = -5;
    listB = InsertHead(val, INT, listB);
    Print(listB);
    printf("Insert 9.9 into the rear of the list B and print list B\n");
    val = (double*)malloc(sizeof(double));
    *(double*)val = 9.9;
    listB = InsertRear(val, DOUBLE, listB);
    Print(listB);
    printf("Insert 't' into the 2th place of the list B and print list B\n");
    val = (char*)malloc(sizeof(char));
    *(char*)val = 't';
    listB = InsertKth(val, 2, CHAR, listB);
    Print(listB);
    printf("Merge list A and list B into list A and print the new list A\n");
    listA = Merge(listA, listB);
    Print(listA);
    printf("Clear list A and check the result\n");
    listA = MakeEmpty(listA);
    Print(listA);
}

/**
 * @brief: create a new list, set the new head pointing to NULL, and return the new head
 */
PtrToNode CreateList()
{
    PtrToNode l = NULL;
    return l;
}

/**
 * @brief: get the length of the list
 */
int GetLength(PtrToNode l)
{
    int length = 0;
    while (NULL != l)
    {
        l = l->pNext;
        length++;
    }
    return length;
}

/**
 * @brief: insert the new node into the head of the list,
 *         return the new head after the insertion
 */
PtrToNode InsertHead(void *pData, int dataType, PtrToNode l)
{
    PtrToNode newNode;
    newNode = (PtrToNode)malloc(sizeof(struct Node));
    newNode->pData = pData;
    newNode->dataType = dataType;
    newNode->pNext = l;
    return newNode;
}

/**
 * @brief: insert the new node into the Kth place,
 *         return the new head after the insertion
 */
PtrToNode InsertKth(void *pData, int k, int dataType, PtrToNode l)
{
    PtrToNode precursor, newNode;
    
    if (k < 1)
    {
        printf("Illegal input for k\n");
        return l;
    }
    else if (1 == k)
    {
        return InsertHead(pData, dataType, l);
    }
    else
    {
        precursor = FindKth(k - 1, l);
        /* 
         * examine whether the local variable "precursor" is a NULL pointer,
         * if it is a NULL pointer, the parameter "k" is out of the range of the list 
         */
        if (NULL == precursor)
        {
            printf("Out of range\n");
            return l;
        }
        newNode = (PtrToNode)malloc(sizeof(struct Node));
        newNode->pData = pData;
        newNode->dataType = dataType;
        newNode->pNext = precursor->pNext;
        precursor->pNext = newNode;
        return l;
    }
}

/**
 * @brief: insert the new node into the rear of the list,
 *         return the new head after the insertion
 */
PtrToNode InsertRear(void *pData, int dataType, PtrToNode l)
{
    int length = GetLength(l);
    return InsertKth(pData, length + 1, dataType, l);
}

/**
 * @brief: find the Kth node in the list,
 *         if the Kth node exists, return a pointer to the node.
 *         if the Kth node does not exist, return a NULL pointer
 */
PtrToNode FindKth(int k, PtrToNode l)
{
    int i;
    
    if (k < 1)
    {
        printf("Illegal input\n");
        return NULL;
    }
    for (i = 0; i < k - 1; i++)
    {
        if (NULL == l)
        {
            break;
        }
        l = l->pNext;
    }
    if (i < k - 1)
    {
        printf("Out of range\n");
        return NULL;
    }
    return l;
}

/**
 * @brief: remove the head node of the list and return the new head after removing.
 *         If the list has no element, output "The list has no element to be removed"
 */
PtrToNode RemoveHead(PtrToNode l)
{
    PtrToNode oldNode;
    /* it is an error to remove the head node from an empty list */
    if (NULL == l)
    {
        printf("The list has no element to be removed\n");
        return NULL;
    }
    oldNode = l;
    l = l->pNext;
    free(oldNode);
    return l;
}

/**
 * @brief: remove the Kth node of the list and return the new head after removing.
 *         If the Kth node does not exist, output "Out of range"
 */
PtrToNode RemoveKth(int k, PtrToNode l)
{
    PtrToNode precursor, oldNode;
    
    if (k < 1)
    {
        printf("Illegal intput for k\n");
        return l;
    }
    else if (1 == k)
    {
        return RemoveHead(l);
    }
    else
    {
        precursor = FindKth(k - 1, l);
        /* examine whether the list has the Kth node to be removed */
        if (NULL == precursor || NULL == precursor->pNext){
            printf("Out of range\n");
            return l;
        }
        oldNode = precursor->pNext;
        precursor->pNext = oldNode->pNext;
        free(oldNode);
        return l;
    }
}

/**
 * @brief: remove the rear node of the list and return the new head after removing.
 */
PtrToNode RemoveRear(PtrToNode l)
{
    int length = GetLength(l);
    /* it is an error to remove the head node from an empty list */
    if (0 == length)
    {
        printf("The list has no element to be removed\n");
        return NULL;
    }
    return RemoveKth(length, l);
}

/**
 * @brief: print the whole list
 */
void Print(PtrToNode l)
{
    while (NULL != l)
    {
        if (CHAR == l->dataType)
        {
            printf("%c ", *(char*)l->pData);
        }
        else if (INT == l->dataType)
        {
            printf("%d ", *(int*)l->pData);
        }
        else if (DOUBLE == l->dataType)
        {
            printf("%lf ", *(double*)l->pData);
        }
        l = l->pNext;
    }
    putchar('\n');
}

/**
 * @brief: remove all the nodes in the list
 */
PtrToNode MakeEmpty(PtrToNode l)
{
    while (NULL != l)
    {
        l = RemoveHead(l);
    }
    return l;
}

/**
 * @brief: merge the two lists and return the new head after merging
 */
PtrToNode Merge(PtrToNode lhs, PtrToNode rhs)
{
    PtrToNode rear;
    int length;
    if (NULL == lhs)
    {
        return rhs;
    }
    else if (NULL == rhs)
    {
        return lhs;
    }
    else
    {
        length = GetLength(lhs);
        rear = FindKth(length, lhs);
        rear->pNext = rhs;
        return lhs;
    }
}

/**
 * @brief: reverse the list and return the new head after reversion
 */
PtrToNode Reverse(PtrToNode l)
{
    PtrToNode newList = CreateList();
    PtrToNode tempNode;
    while (NULL != l)
    {
        tempNode = l;
        l = l->pNext;
        tempNode->pNext = newList;
        newList = tempNode;
    }
    return newList;
}
