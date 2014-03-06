CFLAGS = `pkg-config --cflags --libs sdl2`
LIB = -lglut -lGLU -lGL -lGLEW 

run: popper.cpp
	g++ popper.cpp -g $(LIB) $(CFLAGS) -o popper
        
clean: 
	rm -f *.out *~ run
