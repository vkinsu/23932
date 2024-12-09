#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void setup_terminal(struct termios* saved_settings) {
  struct termios new_settings;
  tcgetattr(STDIN_FILENO, saved_settings);
  new_settings = *saved_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
  new_settings.c_cc[VMIN] = 1;
  new_settings.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

void remove_char(char* buff, int* i) {
  if (*i == 0) write(STDOUT_FILENO, "\a", 1);
  else {
    (*i)--;
    buff[*i] = '\0';
    write(STDOUT_FILENO, "\b \b", 3);
  }
}

void remove_word(char *buff, int* i) {
  while (*i > 0 && buff[*i - 1] == ' ')
    remove_char(buff, i);
  while (*i > 0 && buff[*i - 1] != ' ')
    remove_char(buff, i);
}

void remove_line(char* buff, int* i) {
  while (*i > 0)
    remove_char(buff, i);
}

void copy_last_word(char* from, char* to) {
  int i = strlen(from) - 1;
  int j = 0;
  while (from[i-1] != ' ' && i > 0) i--;
  for (j = 0; from[i] != '\0'; j++) {
    to[j] = from[i];
    i++;
  }
  to[j] = '\0';
}

int single_word(char* buff) {
  for (int i = 0; buff[i] != '\0'; i++)
     if (buff[i] == ' ') return 0;
  return 1;
}

void null_buff(char* buff, int* index) {
  for (int i = 0; i < 41; i++) buff[i] = '\0';
  *index = 0;
}

void fix_index(char* buff, int* i) {
  *i = 0;
  while(buff[*i] != '\0') (*i)++;
}

int main() {
  struct termios settings;
  setup_terminal(&settings);

  char buff[41] = { 0 };
  char temp_buff[41] = { 0 };
  int index = 0;

  while (1) {
    fix_index(buff, &index);
    char c;
    if (read(STDIN_FILENO, &c, 1) <= 0) {
       perror("Failed to read char");
       exit(13);
    }

    // Ctrl + D
    if (c == 4 && index == 0) break;
    // Backspace
    else if (c == 127) remove_char(buff, &index);
    // Ctrl + U
    else if (c == 21)  remove_line(buff, &index);
    // Ctrl + W
    else if (c == 23)  remove_word(buff, &index);
    // Letters
    else if (c == '\n') {
      write(STDOUT_FILENO, &c, 1);
      null_buff(buff, &index);
    }
    else if (c >= 32 && c <= 126) {
      if (index < 40) {
        buff[index++] = c;
        buff[index+1] = '\0';
        write(STDOUT_FILENO, &c, 1);
      }
      else if (single_word(buff) == 1) {
        write(STDOUT_FILENO, "\n\a", 2);
        null_buff(buff, &index);
        buff[index++] = c;
	buff[index+1] = '\0';
        write(STDOUT_FILENO, &c, 1);
      }
      else {
        copy_last_word(buff, temp_buff);
        remove_word(buff, &index);
	write(STDOUT_FILENO, "\n", 1);
        strcpy(buff, temp_buff);
	fix_index(buff, &index);
        buff[index++] = c;
        buff[index+1] = '\0';
	write(STDOUT_FILENO, buff, strlen(buff));
        remove_char(buff, &index);
      }
    }
    else write(STDOUT_FILENO, "\a", 1);
  }



  write(STDOUT_FILENO, "\n", 1);
  tcsetattr(STDIN_FILENO, TCSANOW, &settings);
  return 0;
}
