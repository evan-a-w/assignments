#include <stdio.h>
#include <stdbool.h>

void test(bool *bo){
    *bo = false;
    if (!(*bo)) {
        printf("hi\n");
    }
}

int main(void) {
    bool bo = true;
    test(&bo);
}
