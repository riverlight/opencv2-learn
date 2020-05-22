
CC=g++

SRC := main.cpp util.cpp vs_ppt.cpp input.cpp
OBJ := $(subst .cpp,.o,$(SRC))

target := vs_ppt

CFLAGS := -std=c++11 -I$(OPENCV2_INCLUDE) -L$(OPENCV2_LIB) -lopencv_nonfree -lopencv_ml -lopencv_contrib -lopencv_superres -lopencv_legacy -lopencv_stitching -lopencv_ocl -lopencv_photo -lopencv_ocl -lopencv_objdetect -lopencv_features2d -lopencv_calib3d -lopencv_flann -lopencv_core -lopencv_imgproc -lopencv_highgui

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

