PWD=$(shell pwd)
VPATH=$(PWD)
OBJDIR=obj


SRC_C=$(foreach dir,$(subst :,,$(VPATH)),$(wildcard $(dir)/*.c))
OBJ_C=$(addsuffix .o,$(basename $(SRC_C)))


CCFLAG= -Wall -Os
# -O1 -02 -O3
LDFLAGS= -lpthread

#CC=gcc -Wall -Werror
CC=gcc


TARGET=mqtt_client

src:
	@echo $(SRC_C)

pc:$(TARGET)
$(TARGET):$(OBJ_C)
	$(CC) -o $@ $^ $(LDFLAGS)
$(OBJ_C):$(SRC_C)
	$(CC) -c $(SRC_C) $(CCFLAG)

freertos:
	@echo "hello world!!"

clean:
	rm -rf $(OBJ_C) $(TARGET)
