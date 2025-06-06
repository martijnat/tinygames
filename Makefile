all:
	gcc snek/snek_nocomment.c -o snek.bin  -lSDL2
	cat snek/snek_nocomment.c | qrencode -o snek_qr.png

