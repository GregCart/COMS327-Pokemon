CC = gcc
CXX = g++
CFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=Fall2023 
CXXFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=Fall2023 
INCLUDES = -I/lib
LFLAGS = -lm -lncurses -L/lib
SRCS = heap.c maps.c queue.c trainers.c utils-misc.c
OBJS = $(addprefix lib/, $(SRCS:.c=.o))
MAIN = Pokemon_GC2

.PHONY: depend clean

all: build start

build: $(OBJS)
	$(CXX) $(MAIN).c -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

start:
	./$(MAIN)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

depend: $(SRCS)
	makedepend $(INCLUDES) $^


test: build
	./$(MAIN) 

test-v: build
	valgrind --leak-check=full --log-file=valgrind-log.txt ./$(MAIN) 


clean:
	rm -f $(MAIN) */*.o *~ core *.exe *.stackdump vgcore.* valgrind-log.*

package: clean
	\cp -r changes.txt CHANGELOG
	git --no-pager log --reverse > hist.txt
	cat hist.txt >> CHANGELOG
	cd ..; rm -fr greg-carter_assignment-1.05 greg-carter_assignment-1.05.tar.gz;
	cd ..; rsync -av --exclude=Pokemon/.git --exclude=Pokemon/.vscode --exclude=Pokemon/pokemon-old Pokemon greg-carter_assignment-1.05; tar cvfz greg-carter_assignment-1.05.tar.gz greg-carter_assignment-1.05;
