CC=g++
CFLAGS=
INCLUDEPATH=../src/include
LIBPATH=-L../build
INCLUDE=-I$(INCLUDEPATH)
LIBS=-lstdc++ -lm -larboretum
SRC= main.cpp app.cpp

STD=-std=c++17

main: $(SRC)
	$(CC) $(SRC) $(CLAGS) $(STD) $(LIBPATH) $(INCLUDE) $(LIBS) -o main 

clean:
	rm main