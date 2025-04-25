CC = gcc
LIB = -lstdc++ -L src\lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lglew32 -lcimgui -lassimp -lcJSON
INCDIRS = -Isrc\include -Isrc\include\imgui -Isrc\include\imgui\backends -Isrc\include\generated -Isrc\include\generated\backends

# Список исходных файлов
SOURCES = src/main.c src/shader.c src/filehandle.c   src/dynlist.c src/mesh.c src/model.c
SOURCES += src/nodes/spatial_node.c
SOURCES += src/nodes/light_node.c
SOURCES += src/nodes/model_node.c
SOURCES += src/nodes/player_node.c
SOURCES += src/nodes/camera_node.c
SOURCES += src/nodes/components.c
SOURCES += src/nodes/node.c
SOURCES += src/nodes/collision_node.c
SOURCES += src/scene.c
SOURCES += src/ui.c
SOURCES += src/gpu_light.c



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

CJSON_OBJS = \
		$(OBJ_DIR)/cJSON.o

OBJECTS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
LIBRARY = src/lib/libcimgui.a
JSON = src/lib/libcJSON.a

# Флаги компиляции
CFLAGS = $(INCDIRS) -Wall 
CCFLAGS = $(INCDIRS) -std=c++20 
LDFLAGS = -static-libgcc -static-libstdc++

# Правила
.PHONY: all clean

all: $(LIBRARY) $(JSON) main.exe
	@echo Build completed!
	main.exe
	
main.exe: $(OBJECTS) $(LIBRARY)
	$(CC) $^ -o $@ $(LIB) $(CFLAGS) $(LDFLAGS)

$(LIBRARY): $(IMGUI_OBJS) $(GEN_OBJS)
	@if not exist "$(@D)" mkdir "$(@D)"
	ar rcs $@ $^

$(JSON): $(CJSON_OBJS)
	@if not exist "$(@D)" mkdir "$(@D)"
	ar rcs $@ $^

# Общее правило для компиляции .c файлов
$(OBJ_DIR)/%.o: src/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o: src/nodes/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории imgui
$(OBJ_DIR)/%.o: src/include/imgui/%.cpp
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CCFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории backends
$(OBJ_DIR)/%.o: src/include/imgui/backends/%.cpp
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CCFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории generated
$(OBJ_DIR)/%.o: src/include/generated/%.cpp
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CCFLAGS) -c $< -o $@

# Правило для компиляции .cpp файлов из директории generated/backends
$(OBJ_DIR)/%.o: src/include/generated/backends/%.cpp
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/include/cJSON/%.c
	@if not exist "$(@D)" mkdir "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@if exist "main.exe" del /f /q "main.exe"

cleansource:
	@del /f /q $(OBJ_DIR)\main.o $(OBJ_DIR)\camera.o $(OBJ_DIR)\fileread.o $(OBJ_DIR)\shader.o
