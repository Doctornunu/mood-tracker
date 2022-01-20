#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MENU_ARG "-m"
#define USR_FILE "usr.conf"

#define WARNING_PREFIX "[WARNING]"
#define INFO_PREFIX "[INFO]"

enum command_entry
{
    IDLE,
    ADD_ENTRY,
    STATS,
    CUSTOMIZE,
    SETUP,
    VIEW_HOME,
};

FILE *usr_f;
char username[32];

/**
 * Put a warning message to output
 */
static void put_warning(char* msg)
{
    printf("%s %s", WARNING_PREFIX, msg);
}

/**
 * Put an info message to output
 */
static void put_info(char* msg)
{
    printf("%s %s", INFO_PREFIX, msg);
}

/**
 * Get the current time in local timezone
 */
char* get_current_time()
{
    time_t t = time(NULL);
    struct tm *timeinfo = localtime(&t);
    return asctime(timeinfo);
}

/**
 * Prompt the user to select a menu option.
 * Return `IDLE` if the user has not entered an option.
 */
static enum command_entry try_get_menu_selection()
{
    char option[3];
    printf("> ");
    scanf("%s", option);

    int menu_item = atoi(option);

    if (menu_item < 4 & menu_item >= 0)
    {
        return menu_item;
    }
    printf("* Invalid option selected, please select a number between 1-3");
    return 0;
}

/**
 * Print the main menu
 */
static void put_main_menu(char* uname)
{
    printf("* Welcome %s, please select a option:\n", uname);
    printf("*------------------------------------------------\n");
    printf("* 1 - Add log entry\n");
    printf("* 2 - Overview\n");
    printf("* 3 - Customize entry options\n");
    printf("*------------------------------------------------\n>\n");
}

/**
 * Add a new log entry
 */
static void add_log_entry()
{
    printf("* Log entry: %s\n", get_current_time());
}

/**
 * Prompt user to set a username
 */
static void set_user_name()
{
    char confirm[3];
    printf("- Enter a username: ");
    scanf("%s", username);
    printf("\n- Please confirm your username: %s [y/n]", username);
    scanf("%s", confirm);

    if (
        strcmp(confirm, "Y") == 0
        ||
        strcmp(confirm, "y") == 0
        )
    {
        usr_f = fopen(USR_FILE, "w+");
        fprintf(usr_f, "%s", username);
        fclose(usr_f);
    }
    else if (
            strcmp(confirm, "N") == 0
            ||
            strcmp(confirm, "n") == 0
            )
    {
        set_user_name();
    }
}

/**
 * Run the setup process to get user information
 */
static void run_user_setup()
{
    printf("* Hello and welcome to Mood-Tracker!\n");
    printf("*---------------------------------\n");
    printf("* Looks like this is your first use,\nplease provide the information below:\n");
    set_user_name();
}

/**
 * Execute a command from entry
 */
const void exec_command(enum command_entry *c_entry)
{
    int pid = fork();
    if (pid == -1)
    {
        printf("Error\n");
    }

    if (pid == 0)   // Child process (can only exec one command here)
    {
        execlp("clear", "clear", NULL);
    }
    else            // Parent process
    {
        wait(NULL);
        switch(*c_entry)
        {
            case IDLE:
                printf("[Error] Tried to exec_command when IDLE\n");
            break;

            case ADD_ENTRY:
                add_log_entry();
            break;

            case SETUP:
                run_user_setup();
            break;

            case VIEW_HOME:
                put_main_menu(username);
            break;

            default:
                printf("Not implemented yet!\n");
        }
    }
}

/**
 * Check if the setup process has been run before
 */
static bool is_first_use()
{
    return access(USR_FILE, F_OK) != 0;
}

/**
 * Try and load saved data from export file
 */
static bool try_load_saved_data()
{
    if (access(USR_FILE, R_OK) == 0)
    {
        usr_f = fopen(USR_FILE, "r");
        fgets(username, 32, usr_f);
        return true;
    }
    return false;
}

/**
 * Run the ui / input handler
 */
static void run_app(int argc, char* argv[])
{
    enum command_entry c_entry;// = IDLE;

    if (is_first_use())
    {
        c_entry = SETUP;
        exec_command(&c_entry);
    }
    else
    {
        if(!try_load_saved_data())
        {
            put_warning("Failed to load saved data, aborting...");
            exit(0);
        }
    }

    c_entry = IDLE;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(MENU_ARG, argv[i]) == 0)
        {
            if (i + 1 <= argc)
            {
                c_entry = atoi(argv[i+1]);
                printf("You have selected option: %d!\n", c_entry);
            }
            break;
        }
        continue;
    }

    if (c_entry == IDLE)
    {
        c_entry = VIEW_HOME;
        exec_command(&c_entry);
        c_entry = IDLE;

        while (c_entry == IDLE)
        {
            c_entry = try_get_menu_selection();
        }
    }
    exec_command(&c_entry);
}


/**
 * Pointers
 * --------
 *
 * # Declaration
 *
 * int a = 10;
 * int *p = &a;         // Declaring a pointer, initialize with adress to variable.
 * int* p = &a;         // This does exactly the same thing
 *
 * # Access
 *
 * printf("%d", p);     // This prints the adress that the pointer points to (a)
 * printf("%d", *p);    // This prints the value at the adress pointed by b (a)
 *
 *
 */
int main(int argc, char* argv[])
{
    run_app(argc, argv);
    return 0;
}
