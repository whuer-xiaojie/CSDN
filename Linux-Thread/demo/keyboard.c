/*
 * keyboard.c
 */

#include "keyboard.h"

#include <termios.h>
#include <unistd.h>

static struct termios old_termios;
static struct termios cur_termios;
static int keyboard_initialized = 0;

int init_keyboard(void) {
    if (tcgetattr(STDIN_FILENO, &old_termios) != 0)
        return -1;

    cur_termios = old_termios; /* structure copy */

    cur_termios.c_lflag &= ~(ECHO | ICANON | ISIG);
    cur_termios.c_cc[VMIN] = 1;
    cur_termios.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_termios) != 0)
        return -1;

    keyboard_initialized = 1;
    return 0;
}

int close_keyboard(void) {
    if (keyboard_initialized) {
        keyboard_initialized = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &old_termios) != 0)
            return -1;
    }
    return 0;
}

int __getch(void) {
    ssize_t n;
    unsigned char c;

    if (!keyboard_initialized)
        return -1;
    cur_termios.c_cc[VMIN] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_termios) != 0)
        return -1;
    n = read(STDIN_FILENO, &c, 1);
    cur_termios.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_termios) != 0)
        return -1;
    return (n == 1) ? c : -1;
}
