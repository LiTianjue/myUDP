#########################################  
# Title : Makefile for socks5 proxy test
# Author: TianjueLi
###########################################  
INC=  ./config 
LIB= -lconfig
LIBPATH = -L./config  


CC=gcc
CFLAGS= -g -lpthread 
  
PRG= Sudp
OBJ= Sudp.o

all:$(PRG)
$(PRG):$(OBJ)
	@$(CC) -o $(PRG) $(OBJ) $(LIB) $(CFLAGS) $(LIBPATH)
$(OBJ):Sudp.c
	@$(CC) -c Sudp.c $(CFLAGS) $(LIB) -I $(INC) -o $(OBJ)
      
  
.PRONY:clean  
clean:
	rm -rf $(PRG) $(OBJ)

