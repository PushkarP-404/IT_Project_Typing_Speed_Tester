// timer.c
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "timer.h"

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h> // POSIX sleep
#endif

#define MAX_FUNCTIONS 10

/* Use time_t for wall-clock seconds */
time_t startTimes[MAX_FUNCTIONS];
double elapsedTimes[MAX_FUNCTIONS];
int isRunning[MAX_FUNCTIONS];

/* helper function to write metrics to file */
void saveMetricsToFile(void)
{
    FILE *file = fopen("metrics.txt", "a"); // append mode
    if (file == NULL) {
        printf("[Timer] Error opening metrics.txt for writing!\n");
        return;
    }

    time_t now = time(NULL);
    fprintf(file, "\n===== Metrics Log (%s) =====\n", ctime(&now));
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        if (elapsedTimes[i] > 0.0) {
            fprintf(file, "Function ID %d: %.6f seconds\n", i, elapsedTimes[i]);
        }
    }
    fprintf(file, "=====================================\n");
    fclose(file);
}

void initTimerSystem(void)
{
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        startTimes[i] = 0;
        elapsedTimes[i] = 0.0;
        isRunning[i] = 0;
    }
    printf("[Timer] System initialized.\n");
}

void startTimer(int id)
{
    if (id < 0 || id >= MAX_FUNCTIONS) {
        printf("[Timer] Invalid ID: %d\n", id);
        return;
    }
    if (isRunning[id]) {
        printf("[Timer] Timer %d already running!\n", id);
        return;
    }
    startTimes[id] = time(NULL); /* wall-clock start */
    isRunning[id] = 1;
    printf("[Timer] Started timer for ID: %d\n", id);
}

void stopTimer(int id)
{
    if (id < 0 || id >= MAX_FUNCTIONS) {
        printf("[Timer] Invalid ID: %d\n", id);
        return;
    }
    if (!isRunning[id]) {
        printf("[Timer] Timer %d was not running!\n", id);
        return;
    }
    time_t endTime = time(NULL); /* wall-clock end */
    /* difftime returns double seconds (end - start) */
    elapsedTimes[id] += difftime(endTime, startTimes[id]);
    isRunning[id] = 0;
    printf("[Timer] Stopped timer for ID: %d\n", id);
}

void printMetrics(void)
{
    printf("\n======= Timer Metrics Summary =======\n");
    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        if (elapsedTimes[i] > 0.0) {
            printf("Function ID %d: %.6f seconds\n", i, elapsedTimes[i]);
        }
    }
    printf("=====================================\n");

    /* Save metrics to file */
    saveMetricsToFile();
    printf("[Timer] Metrics saved to metrics.txt successfully.\n");
}

/* Live timer that updates each second (uses start/stop above) */
void runLiveTimer(int id, int durationSeconds)
{
    if (id < 0 || id >= MAX_FUNCTIONS) {
        printf("[Timer] Invalid ID: %d\n", id);
        return;
    }

    printf("\n[Timer] Starting live timer for ID %d (%d seconds)...\n", id, durationSeconds);
    startTimer(id);

    for (int i = 1; i <= durationSeconds; i++) {
        sleep(1); /* cross-platform macro above */
        printf("Elapsed: %d second(s)\r", i);
        fflush(stdout);
    }

    stopTimer(id);
    printf("\n[Timer] Live timer for ID %d stopped.\n", id);
}

/* New: return current elapsed seconds for id */
double getElapsedForId(int id)
{
    if (id < 0 || id >= MAX_FUNCTIONS) return -1.0;
    return elapsedTimes[id];
}
