#########################################  
# Title : Makefile for socks5 proxy test
# Author: TianjueLi
###########################################  
INC=  ./config 
LIB= -lconfig
LIBPATH = -L./config  


CC=gcc
CFLAGS= -g  
  
PRG= Cudp
OBJ= Cudp.o

all:$(PRG)
$(PRG):$(OBJ)
	@$(CC) -o $(PRG) $(OBJ) $(LIB) $(CFLAGS) $(LIBPATH)
$(OBJ):Cudp.c
	@$(CC) -c Cudp.c $(CFLAGS) $(LIB) -I $(INC) -o $(OBJ)
      
  
.PRONY:clean  
clean:
	rm -rf $(PRG) $(OBJ)

