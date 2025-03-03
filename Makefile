
# clang -Wall -Wextra -Wpedantic -ggdb  -o input_output input_output.c
# -fsanitize=address
main:
	# clang -Wall -Wextra -ggdb -o main.o main.c
	clang -Wall -Wextra -ggdb -fsanitize=address -o main.o main.c

main_mac:
	clang -Wall -Wextra -glldb -fsanitize=address -o main.o main.c


test:
	clang -Wall -Wextra -ggdb -fsanitize=address -o test.o test.c

test_mac:
	clang -Wall -Wextra -glldb -fsanitize=address -o test.o test.c
