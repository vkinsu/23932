touch file.txt
chmod 600 file.txt
gcc -o main main.c
chmod g+rx main
chmod u+s main
./main
