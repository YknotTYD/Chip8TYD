##
## EPITECH PROJECT, 2024
## NOP
## File description:
## void;
##

SRC  = src/*.c
LIBS = -l csfml-graphics -l csfml-window
NAME = Chip8TYD
ROM  = "files/roms/tetris.ch8"

compile:
	@ gcc -g -Wall -Wextra $(SRC) $(LIBS) \
		-o $(NAME)

compile_Ofast:
	@ gcc -Ofast -Wall -Wextra $(SRC) $(LIBS) \
		-o $(NAME)

run: compile
	@ ./$(NAME) $(ROM)
	@ rm $(NAME)

valgrind: compile
	@valgrind -s --show-leak-kinds=none \
		--track-origins=yes \
		--leak-check=full \
		--error-limit=no \
	./$(NAME) $(ROM)
	@ rm $(NAME)

clean:
	@ rm -fr *.o

fclean: clean
	@ rm -f $(NAME)

re: fclean compile

kronk:
	@ echo "Oh ouais."
