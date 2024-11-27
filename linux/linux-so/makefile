TARGET := libxxx.so
DIRS := ./ 
SOURCES := $(foreach dir,$(DIRS),$(wildcard $(dir)*.c))

SOURCES := $(filter-out ./client.c ./server.c, $(SOURCES))

OBJS := $(notdir $(SOURCES:.c=.o))
OBJ_PATH := ./objs/
OBJS_O := $(addprefix $(OBJ_PATH), $(OBJS))




CC := gcc -g -Wall

LIBPATH :=  -L./  

LIBS :=   -ldl -lpthread 

INCLUDE:= -I./ 

CFLAGS :=  $(INCLUDE)  -shared -fPIC 

all: $(OBJ_PATH) $(TARGET)

$(OBJ_PATH):
	mkdir -p ./$@


$(TARGET): $(OBJS_O)	
	@echo 'Linking......'	
	$(CC) -o $@ $^ $(LIBPATH) $(LIBS) $(CFLAGS) -Wl,--version-script=1.ver -Wl,--soname='libxxx.so'
	@echo 'Finished Linking: $@'	
	@echo 'success!'

$(OBJS_O): $(SOURCES)	
	@echo 'Building file: $(SOURCES)'	
	$(CC) $(CFLAGS) -o $@ -c $(filter %$(*F).c,$(SOURCES))	
	@echo 'Finished building: $@'	
	@echo ''
clean:
	rm -rf $(OBJ_PATH)/* $(TARGET) 

c: client.o
	gcc -o client $^ -L. -lxxx