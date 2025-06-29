/* inish.c */

#include "commands.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/video/video.h"
#include "inish.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "vintix.h"

// ALL OF THIS IS HARD-CODED FOR NOW!

// I WILL CHANGE TO USER PROGRAM AFTER I GET A BETTER UNDERSTANDING OF THE
// KERNEL EXECUTION PROGRAM.

// 16384, 32767 AND 65535 CAUSED THE SYSTEM TO CRASH, IS RECOMMEND TO USE 512 OR
// 256
#define CMD_BUFFER_SIZE 512
#define MAX_HISTORY 128
static char command_history[MAX_HISTORY][CMD_BUFFER_SIZE];
static int  history_count  = 0;
int         input_overflow = 0;

const char* version    = "pre-beta-002";
const char* build_date = "28/06/2025"; // using non-american date format

// Command handler function type
typedef void (*command_func_t)(const char* args);

// Forward declarations for handler functions
static void cmd_help (const char* args);
static void cmd_clear (const char* args);
static void cmd_echo (const char* args);
static void cmd_poweroff (const char* args);
static void cmd_reboot (const char* args);
static void cmd_vfetch (const char* args);
static void cmd_display_info (const char* args);
static void cmd_ls (const char* args);
static void cmd_test_circle (const char* args);
static void cmd_test_square (const char* args);
static void cmd_sleep (const char* args);
static void cmd_test_caps (const char* args);
static void cmd_history (const char* args);
static void cmd_test_graphics (const char* args);

// Command table with handler functions
static struct Command {
    const char*    name;
    const char*    description;
    command_func_t handler;
} commands[] = {
    {"help", "Show this help message", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"echo", "Echo a message", cmd_echo},
    {"poweroff", "Power off the OS", cmd_poweroff},
    {"reboot", "Reboot the OS", cmd_reboot},
    {"vfetch", "View system information", cmd_vfetch},
    {"display_info", "View monitor/display information", cmd_display_info},
    {"ls", "View current or other directory", cmd_ls},
    {"test_circle", "Test drawing a circle", cmd_test_circle},
    {"test_square", "Test drawing a square", cmd_test_square},
    {"sleep", "Test the HPET timer", cmd_sleep},
    {"test_CAPS", "Test the CAPS LOCK key", cmd_test_caps},
    {"history", "Show the history of commands", cmd_history},
    {"test_graphics", "Test the graphics driver", cmd_test_graphics},
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

static void handle_command (char* cmd) {
    // Split command and arguments
    char* space = cmd;
    while (*space && *space != ' ')
        ++space;
    char* args = NULL;
    if (*space == ' ') {
        *space = '\0';
        args   = space + 1;
        while (*args == ' ')
            ++args; // skip extra spaces
    }
    for (size_t i = 0; i < NUM_COMMANDS; ++i) {
        if (strcmp(cmd, commands[i].name) == 0) {
            commands[i].handler(args);
            return;
        }
    }
    printf("Unknown command: '%s'\n", cmd);
}

void shell (void) {
    char cmd_buffer[CMD_BUFFER_SIZE];
    int  cmd_ptr = 0;

    puts("Welcome to Vintix!");
    puts("Type 'help' for a list of commands.");

    while (true) {
        printf("\n$[user-not-implemented-yet] ");
        cmd_ptr        = 0;
        input_overflow = 0;

        while (true) {
            char c = (char) getchar();

            if (c == '\n') {
                putchar('\n');
                if (input_overflow) {
                    puts("Error: command too long.");
                } else {
                    cmd_buffer[cmd_ptr] = '\0';
                    handle_command(cmd_buffer);

                    if (history_count < MAX_HISTORY) {
                        size_t len = strlen(cmd_buffer);
                        if (len >= CMD_BUFFER_SIZE)
                            len = CMD_BUFFER_SIZE - 1;
                        memcpy(command_history[history_count], cmd_buffer, len);
                        command_history[history_count][len] = '\0';
                        history_count++;
                    }
                }
                break;
            }

            else if (c == '\b') {
                if (cmd_ptr > 0) {
                    cmd_ptr--;
                    putchar('\b');
                }
            }

            else {
                if (cmd_ptr < CMD_BUFFER_SIZE - 1) {
                    cmd_buffer[cmd_ptr++] = c;
                    putchar(c);
                } else {
                    input_overflow = 1;
                    putchar('\a');
                }
            }
        }
    }
}

static void cmd_clear (const char* args) {
    if (args && *args) {
        clear(args);
    } else {
        clear("#000000");
    }
}

static void cmd_help (const char* args) {
    (void) args;
    puts("Available commands:\n");
    for (size_t i = 0; i < NUM_COMMANDS; ++i) {
        printf("  %-8s - %s\n", commands[i].name, commands[i].description);
    }
}

static void cmd_echo (const char* args) {
    if (args && *args) {
        puts(args);
    } else {
        puts("Echo...");
        puts("Use: echo <your message>");
    }
}

static void cmd_poweroff (const char* args) {
    (void) args;
    poweroff();
}

static void cmd_reboot (const char* args) {
    (void) args;
    reboot();
}

static void cmd_vfetch (const char* args) {
    (void) args;
    putchar('\n');
    puts("____   ____.__        __  .__        ");
    puts("\\   \\ /   /|__| _____/  |_|__|__  ___");
    puts(" \\   Y   / |  |/    \\   __\\  \\  \\/  /");
    puts("  \\     /  |  |   |  \\  | |  |>    < ");
    puts("   \\___/   |__|___|  /__| |__/__/\\_ \\");
    puts("                   \\/              \\/");
    putchar('\n');
    puts("OS: VintixOS");
    puts("Kernel: Vintix");
    printf("Version: %s\n", version);
    printf("Build date: %s\n", build_date);
}

static void cmd_display_info (const char* args) {
    (void) args;
    putchar('\n');
    puts(":@%%%%%%%%%%%%@-");
    puts(":@           .@-");
    puts(":@           .@-");
    puts(":@           .@-");
    puts(":@............@-");
    puts(":#%%%%%@@%%%%%#:");
    puts("    =%%@@%%+    ");
    putchar('\n');
    printf("Screen width: %d\n", fb_width);
    printf("Screen height: %d\n", fb_height);
    printf("Screen pitch: %d\n", fb_pitch);
    printf("Screen BPP: %d\n", fb_bpp);
}

static void cmd_ls (const char* args) {
    (void) args;
    puts("Not implemented yet.");
    puts("Use: ls <directory>");
    puts("Or just use ls to list the current directory.");
}

static void cmd_test_circle (const char* args) {
    (void) args;

    uint32_t width_center  = fb_width / 2;
    uint32_t height_center = fb_height / 2;
    uint32_t color         = rand32() & 0xFFFFFF; // RGB 24-bit
    video_draw_circle((int) width_center, (int) (height_center), 100, color);
}

static void cmd_test_square (const char* args) {
    (void) args;

    uint32_t width_center  = fb_width / 2;
    uint32_t height_center = fb_height / 2;
    uint32_t color         = rand32() & 0xFFFFFF; // RGB 24-bit
    video_draw_square((int) width_center, (int) height_center, 100, color);
}

static void cmd_sleep (const char* args) {
    if (args && *args) {
        sleep(atoi(args));
    } else {
        puts("Sleep...");
        puts("Use: sleep <time in MS>");
    }
}

static void cmd_test_caps (const char* args) {
    (void) args;
    puts("You tested! Shell can do caps lock now!");
}

static void cmd_history (const char* args) {
    (void) args;

    if (history_count == 0) {
        puts("No commands in history.");
        return;
    }

    for (int i = 0; i < history_count; i++) {
        printf("%2d -> %s\n", i + 1, command_history[i]);
    }
}

static void cmd_test_graphics (const char* args) {
    (void) args;
    clear("#FFFFFF");

    uint8_t  circle_diff = rand32() & 0xFF;
    uint32_t circle_x    = fb_width / 2;
    uint32_t circle_y    = fb_height / 2;
    uint32_t color       = rand32() & 0xFFFFFF;
    video_draw_circle((int) circle_x, (int) circle_y, 100, color);

    // First square: left
    color              = rand32() & 0xFFFFFF;
    uint32_t square1_x = circle_x - circle_diff; // Randomly chosen value
    uint32_t square1_y = circle_y;
    video_draw_square((int) square1_x, (int) square1_y, 100, color);

    // Second square: right
    color              = rand32() & 0xFFFFFF;
    uint32_t square2_x = circle_x + circle_diff; // Randomly chosen value
    uint32_t square2_y = circle_y;
    video_draw_square((int) square2_x, (int) square2_y, 100, color);

    sleep(5000);
    clear(NULL);
}
