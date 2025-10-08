#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>  // For select() - POSIX
#include <sys/time.h>
#include <unistd.h>      // For read()
#include <signal.h>      // For signal handling (backup)
#include <time.h>        // For time measurements

#define TARGET_SENTENCE "The quick brown fox jumps over the lazy dog."
#define MAX_INPUT_LEN 1024
#define TIME_LIMIT_SEC 60  // 1 minute
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Global flag for timeout (set by signal handler if needed)
volatile sig_atomic_t timed_out = 0;

// Signal handler for SIGALRM (backup if select fails)
void timeout_handler(int sig) {
    timed_out = 1;
    // Note: Keep handler simple and async-safe
}

int main() {
    char target[] = TARGET_SENTENCE;
    char user_input[MAX_INPUT_LEN] = {0};
    int input_len = 0;
    fd_set readfds;
    struct timeval timeout;
    int ret;
    time_t start_time;

   printf("%s%sTyping Speed Tester (with Backspace Support)%s\n", BOLD, CYAN, RESET);
    printf("%s============================================%s\n", BOLD, RESET);
    printf("\n");
    // Colored target sentence
    printf("%sTarget sentence:%s %s\n\n", BOLD, RESET, target);
    // Colored instructions
    printf("%sYou have %d seconds to type the sentence as accurately as possible.%s\n", YELLOW, TIME_LIMIT_SEC, RESET);
    printf("%sBackspace (or Delete) can be used to correct mistakes.%s\n", YELLOW, RESET);
    printf("%sPress Enter to finish early.%s\n", YELLOW, RESET);
    printf("\n%sStart typing...%s\n\n", GREEN, RESET);


    // Optional: Set up alarm as backup (in case select doesn't work as expected)
    signal(SIGALRM, timeout_handler);
    alarm(TIME_LIMIT_SEC);

    // Record start time
    start_time = time(NULL);

    // Loop to read input with timeout using select()
    while (input_len < MAX_INPUT_LEN - 1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // Set timeout to remaining time
        time_t elapsed = time(NULL) - start_time;
        int remaining = TIME_LIMIT_SEC - (int)elapsed;
        if (remaining <= 0) {
            timed_out = 1;
            break;
        }

        timeout.tv_sec = remaining;
        timeout.tv_usec = 0;

        ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (ret == -1) {
            perror("select failed");
            break;
        } else if (ret == 0) {
            // Timeout
            timed_out = 1;
            break;
        } else if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // Input available
            char ch;
            ssize_t bytes = read(STDIN_FILENO, &ch, 1);
            if (bytes > 0) {
                // Handle backspace (ASCII 8 or 127 for DEL)
                if ((ch == 8 || ch == 127) && input_len > 0) {
                    // Erase last character from buffer
                    input_len--;
                    user_input[input_len] = '\0';

                    // Echo backspace visually: move back, overwrite with space, move back again
                    printf("\b \b");
                    fflush(stdout);
                    continue;  // Don't append
                }

                // Handle newline/enter to stop early
                if (ch == '\n' || ch == '\r') {
                    break;
                }

                // Ignore other control characters (e.g., arrows) for simplicity
                if (ch < 32 && ch != 9) {  // Ignore most controls except tab (space-like)
                    continue;
                }

                // Append printable characters (including space and tab)
                if (input_len < MAX_INPUT_LEN - 1) {
                    user_input[input_len++] = ch;
                    user_input[input_len] = '\0';

                    // Echo the character (for user feedback)
                    putchar(ch);
                    fflush(stdout);
                }
            }
        }
    }

    // Cancel alarm
    alarm(0);

    // Flush any remaining input (optional, but helps on some systems)
    // Note: fflush(stdin) is non-standard; use a loop if needed, but select should handle it

    printf("\n\n%s%sTime's up! (or you finished early\n",RED,BOLD);

    // Calculate actual time taken (in minutes)
    double time_taken_min = (double)(time(NULL) - start_time) / 60.0;

    // Truncate user input to target length for comparison (standard for typing tests)
    int target_len = strlen(target);
    if (input_len > target_len) {
        input_len = target_len;
        user_input[input_len] = '\0';
    }

    // Accuracy: Percentage of matching characters (position-based on final input)
    // This reflects the final accuracy after any backspaces/corrections.
    // Backspaces allow improvements, so accuracy can increase with corrections.
    // If you want to "penalize" backspaces (e.g., decrease accuracy based on errors made),
    // see the note below for a modified calculation.
    int correct_chars = 0;
    for (int i = 0; i < input_len; i++) {
        if (user_input[i] == target[i]) {
            correct_chars++;
        }
    }
    double accuracy = (input_len > 0) ? ((double)correct_chars / input_len * 100.0) : 0.0;

    // Typing speed: Words per minute (based on final words, not keystrokes)
    // Count words in final user input (simple: split by spaces/tabs)
    int word_count = 0;
    char temp_input[MAX_INPUT_LEN];  // Copy for strtok (modifies string)
    strcpy(temp_input, user_input);
    char *word = strtok(temp_input, " \t\n");
    while (word != NULL) {
        word_count++;
        word = strtok(NULL, " \t\n");
    }
    // If no spaces, treat as 1 word if non-empty
    if (word_count == 0 && input_len > 0) {
        word_count = 1;
    }
    double wpm = (time_taken_min > 0) ? (word_count / time_taken_min) : 0.0;
    const char *accuracy_color = (accuracy >= 90) ? GREEN : (accuracy >= 70) ? YELLOW : RED;
    const char *wpm_color = (wpm >= 60) ? GREEN : (wpm >= 40) ? YELLOW : RED;


    // Output results
    printf("\n%sResults:%s\n", BOLD, RESET);
    printf("%s--------%s\n", BOLD, RESET);
    printf("Time taken: %.1f seconds (%.2f minutes)\n", time_taken_min * 60.0, time_taken_min);
    printf("Final characters typed: %d\n", input_len);
    printf("Final words typed: %d\n", word_count);
    printf("%sTyping speed: %.2f WPM (net, based on final correct words)%s\n", wpm_color, wpm, RESET);
    printf("%sAccuracy: %.2f%%%s (character matches in final input)\n", accuracy_color, accuracy, RESET);
    printf("\nWhat you typed (final): %s\"%s\"%s\n", MAGENTA, user_input, RESET);
    printf("Target: %s\"%s\"%s\n", GREEN, target, RESET);
    
    return 0;
}
