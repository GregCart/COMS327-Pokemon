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
	git --no-pager log --reverse > hist.txt
	cat hist.txt >> CHANGELOG
	cd ..; rm -fr greg-carter_assignment-1.02 greg-carter_assignment-1.02.tar.gz;
	cd ..; rsync -av --exclude=Pokemon/.git --exclude=Pokemon/.vscode Pokemon greg-carter_assignment-1.02; tar cvfz greg-carter_assignment-1.02.tar.gz greg-carter_assignment-1.02;
