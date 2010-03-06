# CONFIGURE

CC	= gcc
INCLUDE	= ./include
BIN	= ./bin
OBJ	= ./obj
DOC	= ./doc
LIB	= ./lib
SRC	= ./src

VERSION	= 0.7

L_OBJS	= $(OBJ)/l_net.o $(OBJ)/l_parse.o $(OBJ)/l_mem.o $(OBJ)/l_irc.o $(OBJ)/l_ctcp.o $(OBJ)/l_dcc.o $(OBJ)/l_commands.o 

H_OBJS	= $(OBJ)/h_irc.o $(OBJ)/h_commands.o $(OBJ)/h_dcc.o $(OBJ)/h_ctcp.o 
   
FLAGS	= -Wall -I$(INCLUDE) -ggdb


# LIB

$(LIB)/libsircll-$(VERSION).a: $(L_OBJS) $(H_OBJS)
	ld -m elf_i386 -r -o $(LIB)/libsircl-$(VERSION).a $(L_OBJS) $(H_OBJS)
   

# OBJECTS LOW-LEVEL

$(OBJ)/l_net.o: $(SRC)/lowlevel/l_net.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_net.c -o $(OBJ)/l_net.o

$(OBJ)/l_parse.o: $(SRC)/lowlevel/l_parse.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_parse.c -o $(OBJ)/l_parse.o

$(OBJ)/l_mem.o: $(SRC)/lowlevel/l_mem.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_mem.c -o $(OBJ)/l_mem.o

$(OBJ)/l_irc.o: $(SRC)/lowlevel/l_irc.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_irc.c -o $(OBJ)/l_irc.o

$(OBJ)/l_ctcp.o: $(SRC)/lowlevel/l_ctcp.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_ctcp.c -o $(OBJ)/l_ctcp.o

$(OBJ)/l_dcc.o: $(SRC)/lowlevel/l_dcc.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_dcc.c -o $(OBJ)/l_dcc.o

$(OBJ)/l_commands.o: $(SRC)/lowlevel/l_commands.c
	$(CC) $(FLAGS) -c $(SRC)/lowlevel/l_commands.c -o $(OBJ)/l_commands.o

	
# OBJECTS HIGH-LEVEL

$(OBJ)/h_ctcp.o: $(SRC)/highlevel/h_ctcp.c
	$(CC) $(FLAGS) -c $(SRC)/highlevel/h_ctcp.c -o $(OBJ)/h_ctcp.o

$(OBJ)/h_dcc.o: $(SRC)/highlevel/h_dcc.c
	$(CC) $(FLAGS) -c $(SRC)/highlevel/h_dcc.c -o $(OBJ)/h_dcc.o

$(OBJ)/h_irc.o: $(SRC)/highlevel/h_irc.c
	$(CC) $(FLAGS) -c $(SRC)/highlevel/h_irc.c -o $(OBJ)/h_irc.o

$(OBJ)/h_commands.o: $(SRC)/highlevel/h_commands.c
	$(CC) $(FLAGS) -c $(SRC)/highlevel/h_commands.c -o $(OBJ)/h_commands.o

	
# TESTING

testbot: $(SRC)/testing/testbot.c $(LIB)/libsircl-$(VERSION).a
	$(CC) $(FLAGS) $(SRC)/testing/testbot.c -o $(BIN)/testbot -L$(LIB) -lsircl-$(VERSION)


# INSTALLATION

clean:
	rm $(H_OBJS) $(L_OBJS)
	rm $(BIN)/testbot
	rm $(LIB)/libsircl-$(VERSION).a

install:
	cp $(LIB)/libsircl-$(VERSION).a /usr/local/lib/libsircl-$(VERSION).a
	cp $(INCLUDE)/sircl.h /usr/include/sircl.h

uninstall:
	rm /usr/local/lib/libsircl-$(VERSION).a
	rm /usr/include/sircl.h

