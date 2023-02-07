all: gen_world

gen_map: mapgenerator.c
	gcc -Wall -Werror -ggdb mapgenerator.c -o map_generator
	./map_generator

gen_world: worldgenerator.c 
	gcc -Wall -Werror -ggdb worldgenerator.c -o world_generator
	./world_generator

clean:
	rm -f map_generator *~ core

package:
	make clean
	cd ..; cp -rv Pokemon greg-carter_assignment-1.02; tar cvfz greg-carter_assignment-1.02.tar.gz greg-carter_assignment-1.01;
