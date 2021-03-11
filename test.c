#include <stdio.h>

void print_val(int val[3]) {
    printf("%d\n", val[1]);
    val[1] = 8;

}

int main(void) {
    int val1[3] = {1, 2, 3};
    int val2[3] = {4, 5, 6};

    int (*placeholder)[3];

    placeholder = &val1;
    printf("%d\n", (*placeholder)[1]);

    placeholder = &val2;
    printf("%d\n", (*placeholder)[1]);

    print_val(*placeholder);
    printf("%d\n", val2[1]);


    return 0;
}
