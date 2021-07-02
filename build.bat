g++ main.cpp -o build/game.exe -DDebug -Og -std=c++11 -Wno-missing-braces -I src/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
remedybg.exe build/game.exe
