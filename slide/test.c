#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main(void) {
    int test = 1;

    int *test_p = &test;

    printf("%d", 0[test_p]);

}
