##
## EPITECH PROJECT, 2024
## NOP
## File description:
## void;
##

NAME = Chip8TYD

compile:
	@ gcc -g -Wall -Wextra src/*.c -l csfml-graphics -l csfml-window \
		-o $(NAME)

compile_Ofast:
	@ gcc -Ofast -Wall -Wextra src/*.c -l csfml-graphics -l csfml-window \
		-o $(NAME)

run: compile
	@ ./$(NAME)
	@ rm $(NAME)

valgrind: compile
	@valgrind -s --show-leak-kinds=none \
		--track-origins=yes \
		--leak-check=full \
		--error-limit=no \
	./$(NAME)
	@ rm $(NAME)

clean:
	@ rm -fr *.o

fclean: clean
	@ rm -f $(NAME)

re: fclean compile

kronk:
	@ echo "Oh ouais."
