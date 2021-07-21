#pragma once
#include "SDL.h"
#include "polygon.h"
#include <vector>
#include <iostream>

class engine
{
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event* event;

	polygon poly1;
	polygon poly2;

	bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false, firePressed = false;

	const float moveSpeed = 2;

	engine()
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("Per-Axis Convex Collision", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, NULL);
		renderer = SDL_CreateRenderer(window, 0, NULL);
		event = new SDL_Event();

		poly1.setAsRegularShape(7, { 32, 32 });
		poly1.position = { 64, 64 };
		poly1.color = { 255, 0, 0 };

		poly2.setAsRegularShape(8, { 32, 32 });
		poly2.position = { 128, 96 };
		poly2.color = { 0, 255, 255 };
	}
	bool update()
	{
		while (SDL_PollEvent(event))
		{
			switch (event->type)
			{
			case SDL_QUIT:
				return false;
				break;
			case SDL_KEYDOWN:
				keyDown();
				break;
			case SDL_KEYUP:
				keyUp();
				break;
			default:
				break;
			}
		}

		if (upPressed)
			poly1.position.y -= moveSpeed;
		else if (downPressed)
			poly1.position.y += moveSpeed;

		if (leftPressed)
			poly1.position.x -= moveSpeed;
		else if (rightPressed)
			poly1.position.x += moveSpeed;

		poly1.resolveFor(poly2);

		render();
		SDL_Delay(1000 / 60);
		return true;
	}
	void keyDown()
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_UP:
			upPressed = true;
			break;
		case SDLK_DOWN:
			downPressed = true;
			break;
		case SDLK_LEFT:
			leftPressed = true;
			break;
		case SDLK_RIGHT:
			rightPressed = true;
			break;
		case SDLK_SPACE:
			firePressed = true;
			break;
		default:
			break;
		}
	}
	void keyUp()
	{
		switch (event->key.keysym.sym)
		{
		case SDLK_UP:
			upPressed = false;
			break;
		case SDLK_DOWN:
			downPressed = false;
			break;
		case SDLK_LEFT:
			leftPressed = false;
			break;
		case SDLK_RIGHT:
			rightPressed = false;
			break;
		case SDLK_SPACE:
			firePressed = false;
			break;
		default:
			break;
		}
	}
	void render()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		poly1.render(renderer);
		poly2.render(renderer);

		SDL_RenderPresent(renderer);
	}
};