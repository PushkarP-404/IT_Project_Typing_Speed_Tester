// timer.h
#ifndef TIMER_H
#define TIMER_H

/* Initialize the timer system (call once at start) */
void initTimerSystem(void);

/* Start timer for function/module identified by id (0..9) */
void startTimer(int id);

/* Stop timer for function/module identified by id */
void stopTimer(int id);

/* Print and save metrics summary to metrics.txt */
void printMetrics(void);

/* Runs a live timer that updates every second for durationSeconds */
void runLiveTimer(int id, int durationSeconds);

/* Returns accumulated elapsed seconds for given id, or -1.0 on error */
double getElapsedForId(int id);
void saveMetricsToFile();

#endif // TIMER_H
