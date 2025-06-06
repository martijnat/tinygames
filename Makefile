games: snek.bin

snek.bin: snek/snek_commented.c
	gcc snek/snek_commented.c -o snek.bin  -lSDL2

readme:
	cat README.md.header > README.md
