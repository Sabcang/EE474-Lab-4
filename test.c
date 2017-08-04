#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

int main(void) {
    bool ha = false;
    char ha1 = 'c';
    ha1 = 'b';
    printf("%c\n", ha1);

    clock_t a, b;
    a = clock();
    int i,j;
    for (i = 0; i < 10000; i++) {
    }
    b = clock();
    int c = b - a;
        float totalT = (double)(b - a)/ CLOCKS_PER_SEC;
        totalT = 0.0000152587890637;
    printf("%.16f\n", totalT);
}