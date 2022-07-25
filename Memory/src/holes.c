#include "holes.h"

/*
Driver
*/

int main(int argc, char **argv)
{
    Process *totalProcesses[BUFFER], *totalMemory[MAX_MEMORY], *nextProcess;
    char line[BUFFER], buffer[BUFFER], strategy;
    double cumulativeMem = 0;
    int size = -1, order = 0, numProcess = 0, totalLoads = 0, avrgNumProcesses = 0, avgHoles = 0, nextMemSpot = 0, indexOrder, tempIndex, i;

    if (argc != 3)
    {
        fprintf(stderr, "Format must be in the form: ./holes fileName <first | best | next | worst>\n");
        exit(-1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp)
    {
        fprintf(stderr, "Could not open file\n");
        exit(-1);
    }

    strcpy(buffer, argv[2]);

    if (strcasecmp(buffer, "first") == 0)
    {
        strategy = 'f';
    }
    else if (strcasecmp(buffer, "best") == 0)
    {
        strategy = 'b';
    }
    else if (strcasecmp(buffer, "next") == 0)
    {
        strategy = 'n';
    }
    else if (strcasecmp(buffer, "worst") == 0 || strcasecmp(buffer, "worse") == 0)
    {
        strategy = 'w';
    }
    else
    {
        fprintf(stderr, "Must select a valid strategy\n");
        exit(-1);
    }

    for (i = 0; i < MAX_MEMORY; i++)
    {
        totalMemory[i] = NULL;
    }

    while (fgets(line, 100, fp))
    {
        Process *temp = (Process *)malloc(sizeof(Process));
        sscanf(line, "%d %d", &(temp->id), &(temp->memRequired));

        if (temp->memRequired > 512 || temp->memRequired < 1)
        {
            fprintf(stderr, "Test file process memory size is invalid\n");
            exit(-1);
        }

        tempIndex = order++;
        temp->numSwaps = 0;
        size = size + 1;
        totalProcesses[size] = temp;
        totalProcesses[size]->key = tempIndex;
        backShift(totalProcesses, size);
    }

    fclose(fp);

    while (size != -1)
    {
        nextProcess = totalProcesses[0];
        totalProcesses[0] = totalProcesses[size];
        size--;
        forwardShift(totalProcesses, size, 0);

        nextMemSpot = executeAlgorithm(totalMemory, nextProcess->memRequired, nextMemSpot, strategy);

        while (nextMemSpot == -1)
        {
            Process *temp = swapProcess(totalMemory);
            numProcess--;
            if (!temp)
            {
                fprintf(stderr, "Cannot require this amount of space\n");
                exit(-1);
            }

            temp->numSwaps++;

            if (temp->numSwaps == MAX_SWAPS)
            {
                free(temp);
            }

            else
            {
                indexOrder = order++;
                size = size + 1;
                totalProcesses[size] = temp;
                totalProcesses[size]->key = indexOrder;
                backShift(totalProcesses, size);
            }

            nextMemSpot = executeAlgorithm(totalMemory, nextProcess->memRequired, nextMemSpot, strategy);
        }

        numProcess++;
        totalLoads++;
        avrgNumProcesses += numProcess;

        for (i = 0; i < nextProcess->memRequired; i++)
        {
            totalMemory[nextMemSpot + i] = nextProcess;
        }

        avgHoles += getTotalHoles(totalMemory);
        cumulativeMem += getUsedMemory(totalMemory);

        printf("pid loaded, #processes = %d, #holes = %d, %%memusage = %0.0f, cumulative %%mem = %0.0f\n",
               numProcess, getTotalHoles(totalMemory), (float)floor(getUsedMemory(totalMemory)), (float)floor(cumulativeMem / totalLoads));
    }

    printf("Total Loads = %d, average processes = %0.1f, average #holes = %0.1f, cumulative %%mem = %0.0f\n", totalLoads, ((float)avrgNumProcesses / (float)totalLoads), ((float)avgHoles / (float)totalLoads), floor((double)cumulativeMem / (double)totalLoads));

    freeMemory(totalMemory);

    return 0;
}

/*
Min Heap Functions
*/

void backShift(Process **totalProcesses, int index)
{
    Process *tempProcess;
    int temp;
    while (index > 0 && totalProcesses[(index - 1) / 2]->key > totalProcesses[index]->key)
    {
        temp = (index - 1) / 2;
        tempProcess = totalProcesses[temp];
        totalProcesses[temp] = totalProcesses[index];
        totalProcesses[index] = tempProcess;
        index = temp;
    }
}

void forwardShift(Process **totalProcesses, int size, int index)
{
    int maxIndex = index;
    int leftIndex = (2 * index) + 1;

    if (leftIndex <= size && totalProcesses[leftIndex]->key < totalProcesses[maxIndex]->key)
    {
        maxIndex = leftIndex;
    }

    int rightIndex = (2 * index) + 2;
    if (rightIndex <= size && totalProcesses[rightIndex]->key < totalProcesses[maxIndex]->key)
    {
        maxIndex = rightIndex;
    }

    if (index != maxIndex)
    {
        Process *tempProcess = totalProcesses[index];
        totalProcesses[index] = totalProcesses[maxIndex];
        totalProcesses[maxIndex] = tempProcess;
        forwardShift(totalProcesses, size, maxIndex);
    }
}

/*
Algorithm Execution Functions
*/

int firstFitAlgorithm(Process **totalMemory, int size)
{
    int spotIndex, spotSize, i, j;
    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i] == NULL)
        {
            spotIndex = i;
            spotSize = 0;

            for (j = i; j < MAX_MEMORY && totalMemory[j] == NULL; j++)
            {
                spotSize++;
            }

            if (spotSize >= size)
            {
                return spotIndex;
            }
        }
    }
    return -1;
}

int bestFitAlgorithm(Process **totalMemory, int size)
{
    int spotIndex = -1, spotSize = 9999, tempIndex, tempSize, i, j;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i] == NULL)
        {
            tempIndex = i;
            tempSize = 0;
            for (j = i; j < MAX_MEMORY && totalMemory[j] == NULL; j++)
            {
                tempSize++;
            }
            if (tempSize >= size && tempSize <= spotSize)
            {
                spotSize = tempSize;
                spotIndex = tempIndex;
            }
        }
    }
    return spotIndex;
}

int nextFitAlgorithm(Process **totalMemory, int size, int index)
{
    int spotIndex, spotSize, i, j;

    if (index == -1)
    {
        index = 0;
    }

    i = index;
    if (totalMemory[i] == NULL)
    {
        spotIndex = i;
        spotSize = 0;
        for (j = i; j < MAX_MEMORY && totalMemory[j] == NULL; j++)
        {
            spotSize++;
        }
        if (spotSize >= size)
        {
            return spotIndex;
        }
    }

    if (i >= (MAX_MEMORY - 1))
    {
        i = 0;
    }
    else
    {
        i++;
    }

    for (; i != index;)
    {
        if (totalMemory[i] == NULL)
        {
            spotIndex = i;
            spotSize = 0;
            for (; i < MAX_MEMORY && totalMemory[i] == NULL && i != index; i++)
            {
                spotSize++;
            }
            if (spotSize >= size)
            {
                return spotIndex;
            }
        }

        if (i >= (MAX_MEMORY - 1) && i != index)
        {
            i = 0;
        }
        else if (i != index)
        {
            i++;
        }
    }

    return -1;
}

int worstFitAlgorithm(Process **totalMemory, int size)
{
    int spotIndex = -1, spotSize = 0, tempIndex, tempSize, i, j;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i] == NULL)
        {
            tempIndex = i;
            tempSize = 0;
            for (j = i; j < MAX_MEMORY && totalMemory[j] == NULL; j++)
            {
                tempSize++;
            }
            if (tempSize >= size && tempSize > spotSize)
            {
                spotSize = tempSize;
                spotIndex = tempIndex;
            }
        }
    }
    return spotIndex;
}

/*
Helpers
*/

Process *swapProcess(Process **totalMemory)
{
    Process *oldestProcess = NULL;
    int oldestTime = 99999, i;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if ((totalMemory[i] != NULL) && (totalMemory[i]->key < oldestTime))
        {
            oldestProcess = totalMemory[i];
            oldestTime = oldestProcess->key;
        }
    }

    if (!oldestProcess)
    {
        return NULL;
    }

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if ((totalMemory[i] != NULL) && (totalMemory[i]->id == oldestProcess->id))
        {
            totalMemory[i] = NULL;
        }
    }

    return oldestProcess;
}

void freeMemory(Process **totalMemory)
{
    int i, j;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i] != NULL)
        {
            Process *temp = totalMemory[i];
            for (j = 0; j < MAX_MEMORY; j++)
            {
                if (totalMemory[j] != NULL && totalMemory[j]->id == temp->id)
                {
                    totalMemory[j] = NULL;
                }
            }
            free(temp);
        }
    }
}

double getUsedMemory(Process **totalMemory)
{
    int memUsed = 0, i;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i])
        {
            memUsed++;
        }
    }
    return (double)(memUsed * 100 / MAX_MEMORY);
}

int getTotalHoles(Process **totalMemory)
{
    int noHoles = 0, i;

    for (i = 0; i < MAX_MEMORY; i++)
    {
        if (totalMemory[i] == NULL)
        {
            noHoles++;
            while ((i < MAX_MEMORY) && (totalMemory[i] == NULL))
            {
                i++;
            }
        }
    }
    return noHoles;
}

int executeAlgorithm(Process **totalMemory, int size, int nextFitIndex, char type)
{
    if (type == 'f')
    {
        return firstFitAlgorithm(totalMemory, size);
    }
    else if (type == 'b')
    {
        return bestFitAlgorithm(totalMemory, size);
    }
    else if (type == 'n')
    {
        return nextFitAlgorithm(totalMemory, size, nextFitIndex);
    }
    else if (type == 'w')
    {
        return worstFitAlgorithm(totalMemory, size);
    }

    return -1;
}
