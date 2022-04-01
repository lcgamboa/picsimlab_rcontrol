
CC = g++


RM= rm -f
CP= cp
MKDIR = mkdir -p

FLAGS =  -Wall -O0 -ggdb

LIBS = 

OBJS = PLoader.o 

exp: all

all: $(OBJS)
	@echo "Linking PLoader"
	@$(CC) $(FLAGS) $(OBJS) -oPLoader $(LIBS)

%.o: %.cc
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./PLoader


clean:
	rm -f PLoader *.o core
