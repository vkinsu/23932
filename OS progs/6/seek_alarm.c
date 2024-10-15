// void handle_alarm(int sig) 
// {
//     char c;
//     lseek(file, 0, SEEK_SET);
//     printf("\n");
//     while (read(file, &c, 1) > 0) 
//     {
//         write(1, &c, 1);
//     }
//     close(file);
//     exit(0);
// }


//         signal(SIGALRM, handle_alarm);
//         alarm(5);
