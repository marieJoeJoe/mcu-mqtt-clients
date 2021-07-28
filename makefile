PWD=$(shell pwd)
VPATH=$(PWD)
OBJDIR=obj

SRC_C=$(foreach dir,$(subst :,,$(VPATH)),$(wildcard $(dir)/*.c))
OBJ_C=$(addsuffix .o,$(basename $(SRC_C)))

CCFLAG= -Wall -Os
# -O1 -02 -O3
LDFLAGS= -lpthread

STRIP_SUFFIX=stripped

#CC=gcc -Wall -Werror
CC=gcc
STRIP=strip

TARGET=mqtt_client
TARGET_STRIP=mqtt_client-$(STRIP_SUFFIX)

src:
	@echo $(SRC_C)

pc:$(TARGET) $(TARGET_STRIP) 
$(TARGET):$(OBJ_C)
	$(CC) -o $@ $^ $(LDFLAGS)
	rm -rf $(OBJ_C)
$(OBJ_C):$(SRC_C)
	$(CC) -c $(SRC_C) $(CCFLAG)
$(TARGET_STRIP):$(TARGET)
	$(STRIP) $(TARGET) -o $(TARGET_STRIP)

freertos:
	@echo "hello world!!"

clean:
	rm -rf $(TARGET) $(TARGET_STRIP)
