
CC=g++

SRC := main.cpp monoC-matting.cpp tools.cpp
OBJ := $(subst .cpp,.o,$(SRC))

target := monoC

CFLAGS := -std=c++11 -O3 -I$(OPENCV2_INCLUDE) -L$(OPENCV2_LIB) -lopencv_core -lopencv_imgproc -lopencv_highgui

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: $(target)


$(target): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

install:
	cp $(target) /usr/local/bin

uninstall:
	rm -f  /usr/local/bin/$(target)
	
clean:
	rm -f *.o
	rm -f $(target)

