
all:
	gcc -Wall -Wextra *.c -l csfml-graphics -l csfml-window -o damien

run:
	gcc -Wall -Wextra *.c -l csfml-graphics -l csfml-window -o damien
	./damien
	rm damien

valgrind:
	gcc -g  -Wall -Wextra *.c -l csfml-graphics -l csfml-window -o damien
	valgrind -s --show-leak-kinds=none --track-origins=yes --leak-check=full --error-limit=no ./damien
	rm damien
