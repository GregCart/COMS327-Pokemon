CFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=Fall2023 -lm
OBJS = worldgenerator.c lib/heap.o lib/queue.o

all: gen_world

gen_map: mapgenerator.c queue.o
	gcc $(CFLAGS) mapgenerator.c -o map_generator
	./map_generator

gen_world: worldgenerator.c heap.o queue.o
	gcc $(CFLAGS) $(OBJS) -o world_generator 
	./world_generator

heap.o: lib/heap.c
	cd lib; gcc $(CFLAGS) heap.c -c;

queue.o: lib/queue.c
	cd lib; gcc $(CFLAGS) queue.c -c;

clean:
	rm -f map_generator world_generator */*.o *~ core *.exe *.stackdump

package:
	make clean
	\cp -r changes.txt CHANGELOG
	git --no-pager log --reverse > hist.txt
	cat hist.txt >> CHANGELOG
	cd ..; rm -fr greg-carter_assignment-1.02 greg-carter_assignment-1.02.tar.gz;
	cd ..; rsync -av --exclude=Pokemon/.git --exclude=Pokemon/.vscode Pokemon greg-carter_assignment-1.02; tar cvfz greg-carter_assignment-1.02.tar.gz greg-carter_assignment-1.02;
