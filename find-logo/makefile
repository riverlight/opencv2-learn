
CC=g++

SRC := main.cpp
OBJ := $(subst .cpp,.o,$(SRC))

target := find-logo

CFLAGS := -std=c++11 -O3 -I$(OPENCV2_INCLUDE) -L$(OPENCV2_LIB) -lopencv_core -lopencv_imgproc -lopencv_highgui

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: $(target)


$(target): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

install:
	cp $(target) /usr/local/bin
	
clean:
	rm -f /usr/local/bin/$(target)
	rm -f *.o
	rm -f $(target)

