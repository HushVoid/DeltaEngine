CC=gcc
LIB=-L src/lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lglew32
INC=-I src/include
CIMGUI = src/include/cimgui/libcimgui.a
SOURCES=main.c shader.c fileread.c camera.c
OBJECTS=$(SOURCES:.c=.o)

ALL: main
	main

main: $(OBJECTS)
	$(CC)	$(OBJECTS) -o main $(LIB) $(CIMGUI)

main.o: main.c
	$(CC) -c main.c $(INC)

shader.o: shader.c shader.h
	$(CC) -c shader.c $(INC)

fileread.o: fileread.c fileread.h
	$(CC) -c fileread.c $(INC)

camera.o: camera.c camera.h
	$(CC) -c camera.c $(INC)
clean:
	del *.o main.exe
