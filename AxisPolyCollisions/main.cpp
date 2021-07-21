#include "SDL.h"
#include "engine.h"
#include <array>

int main(int argc, char* argv[])
{
	engine e;

	while (e.update()) { }

	SDL_Quit();
	return 0;
}