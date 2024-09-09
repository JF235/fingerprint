CC=gcc
CFLAGS=
INCLUDEPATH=../src/include
LIBPATH=-L../build
INCLUDE=-I$(INCLUDEPATH)
LIBS=-lstdc++ -lm -larboretum
SRC= main.cpp app.cpp
OBJS=$(subst .cpp,.o,$(SRC))

STD=-std=c++2a

# Implicit Rules
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(STD) -c $< -o $@ $(INCLUDE)

Arrays: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o Arrays $(INCLUDE) $(LIBPATH) $(LIBS)

clean:
	rm -f *.o
	rm -f Arrays
	rm -f SlimTree.dat