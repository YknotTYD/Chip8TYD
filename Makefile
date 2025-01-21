##
## EPITECH PROJECT, 2024
## Chip8TYD
## File description:
## A Chip8 TYD Emulator;
##

SRC  = src/*.c
LIBS = -l SDL2
NAME = Chip8TYD
DEFAULTROM  = "files/roms/brix.ch8"

compile:
	@ gcc -g -Wall -Wextra $(SRC) $(LIBS) \
		-o $(NAME)

compile_Ofast:
	@ gcc -Ofast -Wall -Wextra $(SRC) $(LIBS) \
		-o $(NAME)

run: compile
	@ ./$(NAME) $(DEFAULTROM)
	@ rm $(NAME)

valgrind: compile
	@valgrind -s --show-leak-kinds=none \
		--track-origins=yes \
		--leak-check=full \
		--error-limit=no \
	./$(NAME) $(DEFAULTROM)
	@ rm $(NAME)

clean:
	@ rm -fr *.o

fclean: clean
	@ rm -f $(NAME)

re: fclean compile

kronk:
	@ echo "Oh ouais."
