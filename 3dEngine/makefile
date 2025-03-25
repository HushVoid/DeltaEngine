CC = gcc
CXX = g++
LIB = -lstdc++ -L src\lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lglew32 -lcimgui
INCDIRS = -Isrc\include -Isrc\include\imgui -Isrc\include\imgui\backends -Isrc\include\generated -Isrc\include\generated\backends
SOURCES = main.c shader.c fileread.c camera.c

# Пути и объектные файлы
OBJ_DIR = obj
IMGUI_OBJS = \
    $(OBJ_DIR)/imgui.o \
    $(OBJ_DIR)/imgui_demo.o \
    $(OBJ_DIR)/imgui_draw.o \
    $(OBJ_DIR)/imgui_tables.o \
    $(OBJ_DIR)/imgui_widgets.o \
    $(OBJ_DIR)/imgui_impl_sdl2.o \
    $(OBJ_DIR)/imgui_impl_opengl3.o

GEN_OBJS = \
    $(OBJ_DIR)/dcimgui.o \
    $(OBJ_DIR)/dcimgui_impl_sdl2.o \
    $(OBJ_DIR)/dcimgui_impl_opengl3.o

OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.c=.o))
LIBRARY = src/lib/libcimgui.a

# Флаги компиляции
CFLAGS = $(INCDIRS)
CXXFLAGS = $(INCDIRS) -std=c++20 
LDFLAGS = -static-libgcc -static-libstdc++

# Правила
.PHONY: all clean

all: $(LIBRARY) main.exe
	@echo Build completed!
	main.exe
	
main.exe: $(OBJECTS) $(LIBRARY)
	$(CC) $^ -o $@ $(LIB) $(CFLAGS) $(LDFLAGS)

$(LIBRARY): $(IMGUI_OBJS) $(GEN_OBJS)
	if not exist "$(@D)" mkdir "$(@D)"
	ar rcs $@ $^

$(OBJ_DIR)/main.o: main.c
	$(CC) -c main.c -o $@ $(INC)

$(OBJ_DIR)/shader.o: shader.c shader.h
	$(CC) -c shader.c -o $@ $(INC)

$(OBJ_DIR)/fileread.o: fileread.c fileread.h
	$(CC) -c fileread.c -o $@ $(INC)

$(OBJ_DIR)/camera.o: camera.c camera.h
	$(CC) -c camera.c -o $@ $(INC)


# Правило для компиляции .cpp файлов из директории imgui
$(OBJ_DIR)/%.o: src/include/imgui/%.cpp
	if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CXXFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории backends
$(OBJ_DIR)/%.o: src/include/imgui/backends/%.cpp
	if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CXXFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории generated
$(OBJ_DIR)/%.o: src/include/generated/%.cpp
	if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CXXFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории generated/backends
$(OBJ_DIR)/%.o: src/include/generated/backends/%.cpp
	if not exist "$(@D)" mkdir "$(@D)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	if exist "main.exe" del /f /q "main.exe"
cleansource:
	del obj\main.o
	del obj\camera.o
	del obj\fileread.o
	del obj\shader.o
