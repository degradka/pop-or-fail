default:
ifeq ($(OS),Windows_NT)
	gcc main.c -lraylib -lopengl32 -lgdi32 -lwinmm -o pop-or-fail.exe
else
	gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o pop-or-fail
endif