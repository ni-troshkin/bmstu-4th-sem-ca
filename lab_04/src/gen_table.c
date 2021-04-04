#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
    printf("Please enter the name of file: ");
    char name[256];
    fgets(name, 255, stdin);
    
    name[strlen(name) - 1] = '\0';
    
    FILE *f = fopen(name, "w");
    if (!f)
        return EXIT_FAILURE;
    
    printf("Please enter number of points: ");
    int a;
    if (scanf("%d", &a) != 1)
        return EXIT_FAILURE;
    
    unsigned seed = time(NULL);
    srand(seed);

    for (int i = 0; i < a; i++)
        fprintf(f, "%d %d 1\n", i, rand() % 10);

    fclose(f);

    return EXIT_SUCCESS;
}