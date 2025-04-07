SRC = $(wildcard src/*.c)

all: 
	gcc -O3 $(SRC) -flto -Iinclude -DMAINLOOP_WINDOWS -Llib -lmingw32 -lSDL2 -lopengl32 -lshlwapi -lcomdlg32 -lole32 -o famikun.exe

emcc:
	emcc $(SRC) -Iinclude -O3 -flto -sUSE_SDL=2 -sNO_INVOKE_RUN -o website/emulator.js

mappers:
	gcc codegen/mappers.c -o mappers.exe
	mappers.exe
	del mappers.exe

debug-compile:
	gcc -pg -no-pie -Iinclude src/*.c -Llib -lmingw32 -lSDL2 -lopengl32
	
debug-graph:
	gprof a.exe | gprof2dot -n0 -e0 | dot -Tsvg -o graph.svg