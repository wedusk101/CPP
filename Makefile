Vector: Vector01.o Vector00.o
	g++ Vector01.o Vector00.o -o Vector

Vector01.o:
	g++ -c Vector01.cpp Vector00.cpp Vector.h
	
Vector00.o:
	g++ -c Vector00.cpp Vector.h		

clean:
	rm -f Vector01.o Vector00.o Vector.h.gch