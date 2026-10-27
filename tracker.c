#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_HABITS 5
#define STR_LEN 100
#define DATA_FILE "habits_c_data.txt"

struct Habit {
    char name[STR_LEN];
    int streak;
    int last_status;            // 1 = Success, 0 = Failed
    char last_trigger[STR_LEN]; // Reason for failure
    char last_date[12];         // DD-MM-YYYY
};

int loadData(struct Habit habits[]);
void saveData(struct Habit habits[], int count);
void updateStreak(struct Habit *h, int status, const char *trigger, const char *date);
void getTodayDate(char *date_str);

int main() {
    struct Habit habits[MAX_HABITS];
    int habit_count = loadData(habits);
    int choice;

    do {
        printf("\n==================================\n");
        printf("    DISCIPLINE & HABIT TRACKER    \n");
        printf("==================================\n");
        printf("1. View Active Habits & Streaks\n");
        printf("2. Add a New Habit (Max %d)\n", MAX_HABITS);
        printf("3. Log Daily Status (Success/Failure)\n");
        printf("4. Save & Exit\n");

        int valid_input;
        do {
            printf("Enter your choice (1-4): ");
            valid_input = scanf("%d", &choice);

            if (valid_input != 1) {
                while (getchar() != '\n'); 
                printf("Mistake: Letters are not allowed. Please type a number from 1 to 4.\n\n");
                choice = 0;
            } else if (choice < 1 || choice > 4) {
                printf("Mistake: %d is not an option. Please choose between 1 and 4.\n\n", choice);
            }
        } while (choice < 1 || choice > 4);

        switch (choice) {
            case 1: {
                if (habit_count == 0) {
                    printf("\nNo habits tracked yet. Choose Option 2 to add one first!\n");
                } else {
                    printf("\n--- CURRENT STREAKS ---\n");
                    for (int i = 0; i < habit_count; i++) {
                        printf("[%d] %s | Streak: %d days | Last Logged: %s\n", 
                               i + 1, habits[i].name, habits[i].streak, habits[i].last_date);
                        if (habits[i].last_status == 0 && strcmp(habits[i].last_trigger, "None") != 0) {
                            printf("    Last Relapse Trigger: %s\n", habits[i].last_trigger);
                        }
                    }
                }
                break;
            }

            case 2: {
                if (habit_count >= MAX_HABITS) {
                    printf("\nLimit reached! Focus on mastering your 5 core habits.\n");
                } else {
                    char temp_name[STR_LEN];
                    printf("\nEnter habit name (use_underscores, e.g., Study_C): ");
                    scanf("%s", temp_name);

                    // Duplicate prevention check
                    int exists = 0;
                    for (int i = 0; i < habit_count; i++) {
                        if (strcmp(habits[i].name, temp_name) == 0) {
                            exists = 1;
                            break;
                        }
                    }

                    if (exists) {
                        printf("\nError: A habit named '%s' already exists!\n", temp_name);
                    } else {
                        strcpy(habits[habit_count].name, temp_name);
                        habits[habit_count].streak = 0;
                        habits[habit_count].last_status = 1;
                        strcpy(habits[habit_count].last_trigger, "None");
                        strcpy(habits[habit_count].last_date, "Not_Started");
                        habit_count++;
                        printf("Habit registered successfully!\n");
                    }
                }
                break;
            }

            case 3: {
                if (habit_count == 0) {
                    printf("\nNo habits available to log. Add one in Option 2 first.\n");
                    break;
                }
                printf("\nSelect habit to log:\n");
                for (int i = 0; i < habit_count; i++) {
                    printf("%d. %s\n", i + 1, habits[i].name);
                }

                int idx;
                do {
                    printf("Select habit number (1-%d): ", habit_count);
                    if (scanf("%d", &idx) != 1) {
                        while (getchar() != '\n');
                        printf("Mistake: Please type a number, not a letter.\n");
                        idx = 0;
                    } else if (idx < 1 || idx > habit_count) {
                        printf("Mistake: Please pick a number between 1 and %d.\n", habit_count);
                    }
                } while (idx < 1 || idx > habit_count);

                int status;
                char date[12];
                char trigger[STR_LEN] = "None";

                getTodayDate(date);

                if (strcmp(habits[idx - 1].last_date, date) == 0) {
                    printf("\nNotice: You already logged '%s' today (%s). Updating today's entry...\n", 
                           habits[idx - 1].name, date);
                }

                do {
                    printf("Did you succeed today [%s]? (1 = Yes, 0 = No): ", date);
                    if (scanf("%d", &status) != 1) {
                        while (getchar() != '\n');
                        printf("Mistake: Type 1 for Yes or 0 for No.\n");
                        status = -1;
                    } else if (status != 0 && status != 1) {
                        printf("Mistake: Only 1 (Yes) or 0 (No) is allowed.\n");
                    }
                } while (status != 0 && status != 1);

                if (status == 0) {
                    printf("What was the trigger/distraction? (use_underscores): ");
                    scanf("%s", trigger);
                }

                updateStreak(&habits[idx - 1], status, trigger, date);
                printf("Status updated automatically for %s!\n", date);
                break;
            }

            case 4:
                saveData(habits, habit_count);
                printf("\nProgress saved to %s. Stay disciplined!\n", DATA_FILE);
                break;
        }
    } while (choice != 4);

    return 0;
}

void getTodayDate(char *date_str) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(date_str, 12, "%d-%m-%Y", tm_info);
}

int loadData(struct Habit habits[]) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
    int count = 0;
    while (count < MAX_HABITS && fscanf(fp, "%s %d %d %s %s", 
           habits[count].name, 
           &habits[count].streak, 
           &habits[count].last_status, 
           habits[count].last_trigger, 
           habits[count].last_date) == 5) {
        count++;
    }
    fclose(fp);
    return count;
}

void saveData(struct Habit habits[], int count) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        printf("Error opening file for saving.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %d %d %s %s\n", 
                habits[i].name, 
                habits[i].streak, 
                habits[i].last_status, 
                habits[i].last_trigger, 
                habits[i].last_date);
    }
    fclose(fp);
}

void updateStreak(struct Habit *h, int status, const char *trigger, const char *date) {
    if (status == 1) {
        if (strcmp(h->last_date, date) != 0) {
            h->streak += 1;
        }
    } else {
        h->streak = 0;
    }
    h->last_status = status;
    strcpy(h->last_trigger, trigger);
    strcpy(h->last_date, date);
}
