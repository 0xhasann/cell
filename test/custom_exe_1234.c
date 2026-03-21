#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Program was passed %d args (including program name).\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Arg #%d: %s\n", i, argv[i]);
    }

    return 0;
}
