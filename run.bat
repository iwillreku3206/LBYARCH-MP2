nasm -f win64 kernel.asm
gcc -c main.c -o main.obj -m64
gcc main.obj kernel.obj -o main.exe -m64
main
