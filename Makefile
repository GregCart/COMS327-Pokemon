all: gen_world

gen_map: mapgenerator.c
	gcc -Wall -Werror -ggdb mapgenerator.c -o map_generator
	./map_generator

gen_world: worldgenerator.c 
	gcc -Wall -Werror -ggdb worldgenerator.c -o world_generator -lm
	./world_generator

clean:
	rm -f map_generator world_generator *~ core *.exe *.stackdump

package:
	make clean
	\cp -r changes.txt CHANGELOG
	git --no-pager log > hist.txt
	cat hist.txt >> CHANGELOG
	cd ..; cp -rv Pokemon greg-carter_assignment-1.02; tar cvfz greg-carter_assignment-1.02.tar.gz greg-carter_assignment-1.02;
