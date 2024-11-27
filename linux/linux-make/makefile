
TARGET := main
DIRS := ./
SOURCES := $(foreach dir,$(DIRS),$(wildcard $(dir)*.c))

OBJS := $(notdir $(SOURCES:.c=.o))
OBJ_PATH := ./objs/
OBJS_O := $(addprefix $(OBJ_PATH), $(OBJS))




CC := gcc

LIBPATH := -L./

LIBS := -lpthread 

INCLUDE:= -I./

CFLAGS :=  $(INCLUDE) 

all: $(OBJ_PATH) $(TARGET)

$(OBJ_PATH):
	mkdir -p ./$@


$(TARGET): $(OBJS_O)	
	@echo 'Linking......'	
	$(CC) -o $@ $^ $(LIBS) $(LIBPATH)	
	@echo 'Finished Linking: $@'	
	@echo ' '

$(OBJS_O): $(SOURCES)	
	@echo 'Building file: $(filter %$(*F).c,$(SOURCES))'	
	$(CC) $(CFLAGS) -o $@ -c $(filter %$(*F).c,$(SOURCES))	
	@echo 'Finished building: $@'	
	@echo ' '


clean:
	rm -f $(OBJS_O)
