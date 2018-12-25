cc source/main.c source/log.c source/graphics.c -o main -fsanitize=address -fsanitize=undefined -lGL -lX11 -O2
