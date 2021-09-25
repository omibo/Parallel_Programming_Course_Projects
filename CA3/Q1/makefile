CC=g++

CFLAGS=-c -msse4.1

INCDIR=-I/usr/local/include/opencv4

LIBDIR=-L/usr/local/lib

LIBFILE=-lopencv_gapi -lopencv_stitching  -lopencv_highgui -lopencv_dnn -lopencv_videoio -lopencv_ml -lopencv_video -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_flann -lopencv_photo -lopencv_imgproc -lopencv_core

CST11=-std=c++11

all: main

main: main.o 
	$(CC) main.o $(LIBDIR) $(LIBFILE) $(CST11)  -o main

main.o: main.cpp
	$(CC) $(CFLAGS) $(INCDIR) $(CST11) main.cpp

clean:
	rm -f *o main

