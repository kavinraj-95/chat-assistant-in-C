#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_COMMAND_LENGTH 255
#define MAX_PATH_LENGTH 1024
#define HISTORY_FILE ".command_history"

typedef struct {
    int brightness;
    int volume;
    char last_command[MAX_COMMAND_LENGTH];
} SystemState;

// Function prototypes
void to_lowercase(char *str);
int starts_with(const char *restrict string, const char *restrict prefix);
void get_password(char *password, size_t max_length);
char* slice_string(char *str, int start, int end);
void save_command_history(const char *command);
void show_command_history(void);
void speak_text(const char *text);
void adjust_volume(SystemState *state, int delta);
void adjust_brightness(SystemState *state, int delta);
void show_system_info(void);
void create_reminder(const char *reminder_text);
void show_weather(void);
void play_music(const char *filename);
void list_directory(void);

// Original utility functions
void to_lowercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

int starts_with(const char *restrict string, const char *restrict prefix) {
    while (*prefix) {
        if (*prefix++ != *string++)
            return 0;
    }
    return 1;
}

void get_password(char *password, size_t max_length) {
    struct termios old_term, new_term;
    int c;
    size_t position = 0;

    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    while (position < max_length - 1 && (c = getchar()) != '\n' && c != EOF) {
        if (c == 127 || c == 8) {
            if (position > 0) {
                printf("\b \b");
                position--;
            }
        } else {
            password[position++] = c;
            printf("*");
        }
    }
    password[position] = '\0';
    printf("\n");
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
}

char* slice_string(char *str, int start, int end) {
    int i;
    char* output = malloc((end - start + 1) * sizeof(char));
    for (i = start; i < end && str[i] != '\0'; i++) {
        output[i - start] = str[i];
    }
    output[i - start] = '\0';
    return output;
}

// New utility functions
void speak_text(const char *text) {
    char execute[MAX_COMMAND_LENGTH] = "espeak-ng \"";
    strcat(execute, text);
    strcat(execute, "\"");
    system(execute);
}

void save_command_history(const char *command) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file) {
        time_t now = time(NULL);
        fprintf(file, "[%s] %s\n", ctime(&now), command);
        fclose(file);
    }
}

void show_command_history(void) {
    system("cat " HISTORY_FILE);
    speak_text("Displaying command history");
}

void adjust_volume(SystemState *state, int delta) {
    char command[MAX_COMMAND_LENGTH];
    state->volume += delta;
    if (state->volume > 100) state->volume = 100;
    if (state->volume < 0) state->volume = 0;

    sprintf(command, "amixer set Master %d%%", state->volume);
    system(command);

    char feedback[100];
    sprintf(feedback, "Volume set to %d percent", state->volume);
    speak_text(feedback);
}

void adjust_brightness(SystemState *state, int delta) {
    char command[MAX_COMMAND_LENGTH];
    state->brightness += delta;
    if (state->brightness > 100) state->brightness = 100;
    if (state->brightness < 10) state->brightness = 10;

    sprintf(command, "xrandr --output $(xrandr | grep primary | cut -d' ' -f1) --brightness %.2f",
            state->brightness / 100.0);
    system(command);

    char feedback[100];
    sprintf(feedback, "Brightness set to %d percent", state->brightness);
    speak_text(feedback);
}

void show_system_info(void) {
    system("echo '=== System Information ==='");
    system("echo 'CPU Usage:'");
    system("top -bn1 | grep 'Cpu(s)' | awk '{print $2}'");
    system("echo 'Memory Usage:'");
    system("free -h");
    system("echo 'Disk Usage:'");
    system("df -h /");
    speak_text("Displaying system information");
}

void create_reminder(const char *reminder_text) {
    char command[MAX_COMMAND_LENGTH] = "echo 'notify-send \"Reminder\" \"";
    system("notify-send \"Reminder Set for 1 hour from now\"");
    strcat(command, reminder_text);
    strcat(command, "\" | at now + 1 hour");
    system(command);
    speak_text("Reminder set for one hour from now");
}

void show_weather(void) {
    system("curl wttr.in/?0");
    speak_text("Displaying current weather information");
}

void play_music(const char* filename) {
    
    system("ffplay rickroll.mp3");
    speak_text("Playing music");
}

void list_directory(void) {
    system("ls -lah --color=auto");
    speak_text("Displaying directory contents");
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    SystemState state = {50, 50, ""}; // Initialize with default values

    system("clear");
    speak_text("Assistant initialized and ready for commands");

    while (1) {
        system("echo  \"Please Enter your command Master\"");
        system("espeak-ng -g 5 \"Please Enter your command Master\"");
        fgets(command, sizeof(command), stdin);
        system("bash loading.sh");

        command[strcspn(command, "\n")] = '\0';

        // Save command to history
        save_command_history(command);

        // Convert to lowercase for comparison
        to_lowercase(command);

        // Store last command
        strncpy(state.last_command, command, MAX_COMMAND_LENGTH);

        // Original commands
        if (starts_with(command, "open file")) {
            char* file = slice_string(command, 9, strlen(command));
            char execute[255] = "zed ";
            strcat(execute, file);
            free(file);
            system(execute);
        }
        else if (starts_with(command, "open ")) {
            system(slice_string(command, 4, strlen(command)));
        }
        else if(starts_with(command, "turn on wifi")) {
            char ssid[225];
            char psd[225];
            printf("Please Enter Your SSID: ");
            scanf("%s", ssid);
            getchar();

            printf("Please Enter Your Password: ");
            get_password(psd, 255);

            char execute[225] = "nmcli dev wifi connect ";
            system("echo \"Connecting to WiFi...\"");
            system("espeak-ng \"Connecting to WiFi\"");
            system("nmcli radio wifi on");
            system("sleep 5");
            strcat(execute, ssid);
            strcat(execute, " password ");
            strcat(execute, psd);
            system(execute);
        }
        else if (starts_with(command, "turn off wifi")) {
            system("echo \"Turning Off Wifi...\"");
            system("espeak-ng \"Turning off WiFi...\"");
            system("nmcli radio wifi off");
        }
        else if (starts_with(command, "what is the time now")) {
            system("date");
        }
        else if (starts_with(command, "quit") || starts_with(command, "exit") || starts_with(command, "bye")) {
            system("echo \"Bye bye master...\"");
            speak_text("Bye bye master...");
            quick_exit(1);
        }
        else if (starts_with(command, "shut down")) {
            printf("Do you want to shut down your system?(y/n) ");
            char c;
            scanf(" %c", &c);
            getchar(); // Consume newline
            if(c == 'y' || c == 'Y'){
                system("echo \"Shutting Down...\"");
                system("sleep 2");
                system("shutdown -h now");
            }
        }
        else if(starts_with(command, "search in youtube")){
            printf("Search Query: ");
            char query[255];
            fgets(query, sizeof(query), stdin);
            char execute[255] = "brave https://www.youtube.com/results?search_query=";
            strcat(execute, query);
            system(execute);
        }
        // New commands
        else if (starts_with(command, "help")) {
            printf("\nAvailable commands:\n"
                   "- open file [filename]: Open a file in Zed editor\n"
                   "- open [program]: Open a program\n"
                   "- turn on wifi: Connect to WiFi network\n"
                   "- turn off wifi: Disconnect from WiFi\n"
                   "- volume up/down: Adjust system volume\n"
                   "- brightness up/down: Adjust screen brightness\n"
                   "- show system info: Display system resources\n"
                   "- show weather: Show current weather\n"
                   "- create reminder [text]: Set a reminder\n"
                   "- play music [filename]: Play audio file\n"
                   "- list files: Show directory contents\n"
                   "- show history: Display command history\n"
                   "- what is the time now: Show current time\n"
                   "- shut down: Power off the system\n"
                   "- search in youtube: Search your query in youtube\n"
                   "- quit/exit/bye: Exit the program\n");
            speak_text("Displaying help information");
        }
        else if (starts_with(command, "volume up")) {
            adjust_volume(&state, 10);
        }
        else if (starts_with(command, "volume down")) {
            adjust_volume(&state, -10);
        }
        else if (starts_with(command, "brightness up")) {
            adjust_brightness(&state, 10);
        }
        else if (starts_with(command, "brightness down")) {
            adjust_brightness(&state, -10);
        }
        else if (starts_with(command, "show system info")) {
            show_system_info();
        }
        else if (starts_with(command, "show weather")) {
            show_weather();
        }
        else if (starts_with(command, "create reminder")) {
            create_reminder(command + 15);
        }
        else if (starts_with(command, "play music")) {
            play_music(command + 11);
        }
        else if (starts_with(command, "list files")) {
            list_directory();
        }
        else if (starts_with(command, "show history")) {
            show_command_history();
        }
        else {
            char execute[255] = "tgpt \"";
            strcat(execute, command);
            strcat(execute, "\"");
            system(execute);
        }
    }

    return 0;
}
