#include <stdio.h>

int main() {
    int option;

    printf("Choose an option (1-3):\n");
    printf("1. Say Hello\n");
    printf("2. Say Goodbye\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &option);

    switch (option) {
        case 1:
            printf("Hello!\n");
            break;
        case 2:
            printf("Goodbye!\n");
            break;
        case 3:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid option.\n");
            break;
    }
label:
    ;
    return 0;
}
