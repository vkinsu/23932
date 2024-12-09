#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ERASE write(1, "\b \b", 4)

int main() {
    struct termios settings;
    struct termios saved_settings;
    tcgetattr(0, &settings);
    memcpy(&saved_settings, &settings, sizeof(saved_settings));
    saved_settings = settings;
    settings.c_lflag &= ~(ICANON | ECHO);
    settings.c_cc[VMIN] = 1;
    settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &settings);

    char line[41];
    int pos = 0;
    while (read(0, &line[pos], 1) == 1) {
        if (pos > 0 && line[pos] == settings.c_cc[VERASE]) {
            ERASE;
            --pos;
        }
        else if (line[pos] == settings.c_cc[VKILL]) {
            while (pos > 0) {
                ERASE;
                --pos;
            }
        }
        else if (line[pos] == settings.c_cc[VWERASE]) {
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            while (pos > 0 && isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
        }
        else if (pos == 0 && line[pos] == settings.c_cc[VEOF]) {
            tcsetattr(0, TCSANOW, &saved_settings);
            return 0;
        }
        else if (!isprint(line[pos]))
            write(1, "\a", 1);
        else if (pos == 40) {
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            write(1, "\n", 1);
            int k = 0;
            int i;
            for (i = pos; i < 40; ++i) {
                write(1, &line[i], 1);
                ++k;
            }
            pos = k;
        }
        else {
            write(1, &line[pos], 1);
            ++pos;
        }
    }
    tcsetattr(0, TCSANOW, &saved_settings);
    return 0;
}