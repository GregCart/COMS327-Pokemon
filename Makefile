all: gen_map_structs

gen_map: mapgenerator.c
	gcc -Wall -Werror -ggdb mapgenerator.c -o map_generator
	./map_generator

gen_map_structs: mapgen_structs.c
	gcc -Wall -Werror -ggdb mapgen_structs.c -o map_generator_structs
	./map_generator_structs

gen_world: worldgenerator.c 
	gcc -Wall -Werror -ggdb worldgenerator.c -o world_generator
	./world_generator

clean:
	rm -f map_generator *~ core

package:
	make clean
	cd ..; cp -rv Pokemon greg-carter_assignment-1_01; tar cvfz greg-carter_assignment-1_01.tar.gz greg-carter_assignment-1_01;
