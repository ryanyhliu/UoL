/*
 * Written by Dr Maryam Abo-Tabik
*/

#include <stdio.h>


/* the function return number based on even/odd test*/
int test(unsigned int x)
{
    /* write your code here*/
    if (x == 1)
    {
        return 1;
    }
    else {
        return (x % 2 == 0) ? x / 2 : x * 3 + 1;
    }
}

/* the function return the length of the sequnce*/
int count_length(unsigned int x)
{
    /* write your code here*/
    int steps = 0;

    // Iteration
    while (x != 1)
    {
        x = test(x); 
        steps++;
    }
    
    return steps;
}

int main(int argc, char* argv[])
{
    unsigned int i, init = 0, count = 0; // init is input, count = steps

    // 1000000
    for (i = 1; i < 1000000; i++)
    {
        // TESTCODE
        //i = 2;

        unsigned int k = count_length(i);

        if (k > count)
        {
            count = k;
            init = i;
            printf("The sequence length for the number %u equals %u \n", init, count);
        }
    }

    return 0;
}

//