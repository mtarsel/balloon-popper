CFLAGS = `pkg-config --cflags --libs sdl2`
LIB = -lglut -lGLU -lGL -lGLEW 

run: flyer.cpp
	g++ flyer.cpp -g $(LIB) $(CFLAGS) -o flyer
        
clean: 
	rm -f *.out *~ run
