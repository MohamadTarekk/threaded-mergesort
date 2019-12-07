#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/// Structs
typedef struct ARRAY
{
    int* data;
    int start;
    int end;
    int size;
} ARRAY;

/// Functions prototype
ARRAY* readArray();
void writeOutput(ARRAY array);
void printArray(ARRAY array, char name);
void mainmenu();
void* threadedMergeSort(void *ptr);
void merge(int *array, int leftStart, int middle, int rightEnd);

int main()
{
    mainmenu();
    return 0;
}

ARRAY* readArray()
{
    ARRAY *array = (ARRAY *) malloc(sizeof(ARRAY));
    FILE *f = fopen("input.txt", "r");
    fscanf(f, "%d\n", &(*array).size);
    (*array).data = (int*) malloc(((*array).size) * sizeof(int));
    int i;
    for (i = 0; i < (*array).size; i++)
    {
        fscanf(f, "%d ", &(*array).data[i]);
    }
    fclose(f);
    (*array).start = 0;
    (*array).end = (*array).size;
    printArray(*array, 'A');

    return array;
}

void writeOutput(ARRAY array)
{
    FILE *f = fopen("output.txt", "w");
    fprintf(f, "%d\n", array.size);
    int i;
    for (i = 0; i < array.size; i++)
        fprintf(f, "%d ", array.data[i]);
    fclose(f);
}

void printArray(ARRAY array, char name)
{
    printf("Array %c:  ", name);
    int i;
    for (i = 0; i < array.size; i++)
        printf("%d   ", array.data[i]);
    printf("\n");

    return;
}

void* threadedMergeSort(void *ptr)
{
    ARRAY *array = (ARRAY *) ptr;
    if((*array).start < (*array).end)
    {
        int m = ((*array).start + (*array).end) /2;
        //recursive merge sort of left half of the array
        pthread_t lower;
        ARRAY *left = (ARRAY *) malloc(sizeof(ARRAY));
        (*left).data = (*array).data;   (*left).start = (*array).start; (*left).end = m;
        pthread_create(&lower, NULL, threadedMergeSort, (void *) left);
        //recursive merge sort of left half of the array
        pthread_t higher;
        ARRAY *right = (ARRAY *) malloc(sizeof(ARRAY));
        (*right).data = (*array).data;  (*right).start = m + 1;   (*right).end = (*array).end;
        pthread_create(&higher, NULL, threadedMergeSort, (void *) right);
        // wait for the 2 treads to finish then merge the results
        pthread_join(lower, NULL);
        pthread_join(higher, NULL);
        merge((*array).data, (*left).start, m, ((*right).end));
    }

    return NULL;
}

void merge(int *array, int leftStart, int middle, int rightEnd)
{
    int leftCount = (middle + 1) - leftStart;
    int rightCount = rightEnd - middle;

    int leftArray[leftCount];
    int rightArray[rightCount];

    int x;
    for (x = 0; x < leftCount; x++)
    {
        leftArray[x] = array[leftStart + x];
    }

    int y;
    for (y = 0; y < rightCount ; y++)
    {
        rightArray[y] = array[middle + 1 + y];
    }

    x = y = 0;
    int z = leftStart;
    while ((x < leftCount) && (y < rightCount))
    {
        if(leftArray[x] < rightArray[y])
        {
            array[z] = leftArray[x];
            x++;
        }
        else
        {
            array[z] = rightArray[y];
            y++;
        }
        z++;
    }

    while (x < leftCount)
    {
        array[z] = leftArray[x];
        x++;    z++;
    }
    while (y < rightCount)
    {
        array[z] = rightArray[y];
        y++;    z++;
    }

    return;
}

void mainmenu()
{
    ARRAY *array = readArray();

    // main thread creation
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, threadedMergeSort, (void*) array);
    pthread_join(mainThread, NULL);
    // print results
    printArray(*array, 'M');
    writeOutput(*array);
    return;
}
