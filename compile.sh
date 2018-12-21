cc source/main.c source/log.c source/glLoader.c -o main -fsanitize=address -fsanitize=undefined -lGL -lX11
