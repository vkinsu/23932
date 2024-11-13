 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <termios.h>
 #include <signal.h>
 #include <string.h>

#define IN_WHITESPASES 1
#define IN_WORD 2

 /* переменная для сохранения исходных атрибутов терминала */
 struct termios saved_attributes;

 void reset_input_mode(void)
 {
    tcsetattr(0, TCSANOW, &saved_attributes);
 }

static int count = 0;
void sighnd(int signo)
{
    if (count == 3) exit(0);
    count ++;
   
    printf("There is no exit AHHAHAHAHHA\n");
}

void set_input_mode(void)
{
    struct termios tattr;
 /* проверяем, что вводcим с терминала */
    if (!isatty(0))
    {
        fprintf(stderr, "Not a terminal.\n");
        exit(1);
    }
    /* считываем текущий режим работы терминала */
    tcgetattr(0, &saved_attributes);
    memcpy(&tattr, &saved_attributes, sizeof(tattr));
    /* включаем неканонический режим без эха */
    tattr.c_lflag &= ~(ICANON|ECHO);
    //tattr.c_iflag |= ECHOE;
    /* считываем минимум по одному символу */
    tattr.c_cc[VMIN] = 1;

    /* без ограничения времени ожидания */
    tattr.c_cc[VTIME] = 0;
    /* устанавливаем новый режим работы терминала */
    tcsetattr(0, TCSAFLUSH, &tattr);
 }

 int main(void)
 {
    char current_char;
    char backspace[4] = {'\b', ' ', '\b', '\0'};
    char new_line = '\n';
    char bell = 0x07;
    int bound = 40;
    char buffer[128];
    char* line = &buffer[0];
    char* tmp_line = &buffer[64];
    int line_index = 0;
    int letters = 0;

    setbuf(stdout, NULL);
    set_input_mode();
    atexit(reset_input_mode);

    /* устанавливаем обработку сигналов завершения */
    signal(SIGINT, sighnd);
    signal(SIGTERM, sighnd);

    while (1)
    {
        read(0, &current_char, 1);
    
        if (current_char == 127) // Backspace
        {
            write(1, backspace, 4); // 0 stdin, 1 stdout, 2 stderr
            if (line_index > 0) line_index -= 1;
        }
        else if (current_char == 0x18) // kill Ctrl + X
        {
            for (; line_index != 0; line_index--)
            {
                write(1, backspace, 4); // 0 stdin, 1 stdout, 2 stderr
            }
            line_index = 0;
        }
        else if (current_char == 0x04) // Ctrl + D
        {
            if (line_index == 0)
            {
                exit(1);
            }
        }
        else if (current_char == 0x17) // Ctrl + W 
        {
            int to_delete = 0;
            int i = 1;
            while (isspace(line[line_index-i]) > 1) {to_delete++; i++;};
            while (isalnum(line[line_index-i]) > 1) {to_delete++; i++;};

            for (int i = 0; i < to_delete; i++)
            {
                write(1, backspace, 4); 
            }
            line_index -= to_delete;
        }
        else if ( (isalnum(current_char) > 1) || (isspace(current_char) > 1) )
        {
            if (line_index == bound)
            {
                line[line_index] = current_char;
                int i = 0;
                while (isalnum(line[line_index - i]) > 1) 
                {
                    i++;
                    write(1, backspace, 4); 
                }
                if (i > bound) exit(-1);

                write(1, &new_line, 1);
             
                for (int j = 0; j < i; j++)
                {
                    tmp_line[j] = line[line_index - i + j + 1];
                }

                line_index = 0;
                for (int j = 0; j < i; j++)
                {
                    write(1, &tmp_line[line_index++], 1); 
                }

                char* tmp = line;
                line = tmp_line;
                tmp_line = tmp;
            }

            write(1, &current_char, 1);
            line[line_index++] = current_char;
            if (current_char == '\n') line_index = 0;
        }
        else
        {
            write(1, &bell, 1);
        }
    }

    return 0;
}
