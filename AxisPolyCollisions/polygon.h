#pragma once
#include "SDL.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

struct vec2 { float x = 0; float y = 0; };
const float PI = std::atanf(1) * 4;

class polygon
{
public:
	vec2 position;
	std::vector<vec2> vertices;
	SDL_Color color = { 255, 255, 255 };

	void setAsBox(float sizeX, float sizeY);
	void setAsRegularShape(int sides, vec2 scale);
	std::vector<vec2> getGlobalVertices();
	void render(SDL_Renderer* renderer);

	static vec2 normalBetween(vec2 v1, vec2 v2);
	static float dot(vec2 v1, vec2 v2);

	float linePositionY = 0;
	float mlineStartX = 0;
	float mlineEndX = 0;
	float clineStartX = 0;
	float clineEndX = 0;

	struct projectedLine
	{
		float start;
		float end;
	};
	static projectedLine projectLine(std::vector<vec2> s, vec2 n);
	static projectedLine projectSliceX(std::vector<vec2>& s, float slice);

	static bool projectedLinesCollide(projectedLine p1, projectedLine p2);
	static bool checkOverlapSAT(std::vector<vec2> s1, std::vector<vec2> s2);
	bool collidesWith(polygon& p);

	struct mtv
	{
		vec2 normal;
		float magnitude;
	};
	static mtv* mtvBetween(std::vector<vec2> mover, std::vector<vec2> collider);
	void resolveFor(polygon& p);
};