.DEFAULT_GOAL := all

# constants

MKDIR = mkdir -p
COPY = cp -r

IFLAGS = -I. 
CFLAGS = ${IFLAGS} 
LFLAGS = 


TARGET = draw_fb 

# objects
objs = draw_fb_rect.o

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@



main_object: ${objs}
	$(CC) -o ${TARGET} $^ $(LFLAGS)

.PHONY: install clean


clean:
	$(RM) ${objs} 
	$(RM) ${TARGET}


debug: CFLAGS += -g -DDEBUG
debug: LFLAGS += -g -DDEBUG
debug: main_object 


all: main_object
