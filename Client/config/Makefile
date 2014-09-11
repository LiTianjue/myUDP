#############################################################  
# Title  : Makefile for config library.  
# Author : LiTianjue
# Data   : 2014/06/17
#############################################################  
#set environment option  
CC = gcc  
CC_FLAG = -D_NOMNG -D_FILELINE  
  
#static library use 'ar' command   
AR = ar  
  
#set your inc and lib  
INC =   
LIB = 
  
#make target lib and relevant obj   
PRG = libconfig.a  
OBJ = config.o  
  

#all target  
all:$(PRG)  
$(PRG):$(OBJ)
	${AR} rc ${PRG} $(OBJ)
$(OBJ):config.c
	$(CC) -c config.c
	
  
.PRONY:clean install
install:
	cp $(PRG) ../
clean:
	rm -f $(OBJ) $(PRG)  

