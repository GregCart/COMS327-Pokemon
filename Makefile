all: gen_map

gen_map: mapgenerator.c
	gcc -Wall -Werror -ggdb mapgenerator.c -o map_generator
	./map_generator

clean:
	rm -f map_generator *~ core