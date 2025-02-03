gcc -Wall -fsanitize=address,undefined -std=c99 -g src/*.c include/*.c -I include/ -lm -o main.out
