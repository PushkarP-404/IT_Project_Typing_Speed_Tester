#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include "timer.h"   // timer.h must be in the same folder

int main() {
    char prompt[] = "the quick brown fox jumps over the lazy dog";
    char typed[256] = "";
    int totalKeystrokes = 0, correctKeystrokes = 0, backspaces = 0;
    int started = 0;
    double elapsed = 0.0;
    clock_t startTime, endTime;

    printf("Expected prompt:\n%s\n\n", prompt);
    printf("Start typing. Timer will start on first key and stop on Enter.\n");
    printf("[Timer] System initialized.\n\n");

    int i = 0;
    char ch;
    while (1) {
        ch = _getch();

        // Start timer on first key
        if (!started) {
            startTimer(0);
            startTime = clock();
            printf("[Timer] Started timer for ID: 0\n");
            started = 1;
        }

        // Enter ends typing
        if (ch == '\r' || ch == '\n') {
            typed[i] = '\0';
            printf("\n");
            break;
        }

        // Handle backspace
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b"); // erase from screen
                backspaces++;
                totalKeystrokes++;
            }
            continue;
        }

        // Normal character
        typed[i++] = ch;
        totalKeystrokes++;
        printf("%c", ch);
    }

    stopTimer(0);
    endTime = clock();

    // Calculate elapsed time in seconds
    elapsed = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
    if (elapsed == 0) elapsed = 1.0; // prevent divide by zero

    // Compare to prompt for accuracy
    int lenPrompt = strlen(prompt);
    int lenTyped = strlen(typed);
    int minLen = lenPrompt < lenTyped ? lenPrompt : lenTyped;
    for (int j = 0; j < minLen; j++) {
        if (typed[j] == prompt[j]) correctKeystrokes++;
    }

    double accuracy = ((double)correctKeystrokes / totalKeystrokes) * 100.0;
    double words = (double)lenTyped / 5.0; // standard WPM = 5 chars = 1 word
    double wpm = (words / elapsed) * 60.0;

    printf("\nYou typed: %s\n\n", typed);
    printf("Results:\n");
    printf("Elapsed time: %.3f seconds\n", elapsed);
    printf("Total keystrokes: %d\n", totalKeystrokes);
    printf("Correct keystrokes: %d\n", correctKeystrokes);
    printf("Incorrect keystrokes: %d\n", totalKeystrokes - correctKeystrokes);
    printf("Backspaces: %d\n", backspaces);
    printf("WPM: %.2f\n", wpm);
    printf("Accuracy: %.2f%%\n", accuracy);

    saveMetricsToFile(0, elapsed);
    printf("\n[Timer] Metrics saved to metrics.txt successfully.\n");

    return 0;
}
