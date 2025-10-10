#include <stdio.h>
#include "timer.h"

int main() {
    int choice, id, duration;

    initTimerSystem();

    while (1) {
        printf("\n========== TIMER MENU ==========\n");
        printf("1. Run new live timer\n");
        printf("2. View metrics summary\n");
        printf("3. Exit\n");
        printf("================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Enter Function ID (0-9): ");
            scanf("%d", &id);
            printf("Enter duration (in seconds): ");
            scanf("%d", &duration);

            runLiveTimer(id, duration);
        }
        else if (choice == 2) {
            printMetrics();
        }
        else if (choice == 3) {
            printf("\nExiting Timer Program. Goodbye!\n");
            break;
        }
        else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
