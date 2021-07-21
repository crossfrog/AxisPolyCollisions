#include "polygon.h"

void polygon::setAsBox(float sizeX, float sizeY)
{
	vertices.clear();

	vec2 halfSize = { sizeX / 2, sizeY / 2 };

	vertices.push_back({ -halfSize.x, -halfSize.y });
	vertices.push_back({ halfSize.x, -halfSize.y });
	vertices.push_back({ halfSize.x, halfSize.y });
	vertices.push_back({ -halfSize.x, halfSize.y });
}

void polygon::setAsRegularShape(int sides, vec2 scale)
{
	vertices.clear();

	float increment = PI * 2 / sides;
	for (int i = 0; i < sides; i++)
	{
		float angle = i * increment;
		vertices.push_back({ std::sinf(angle) * scale.x, std::cosf(angle) * -scale.y });
	}
}

std::vector<vec2> polygon::getGlobalVertices()
{
	std::vector<vec2> globalVertices;
	for (auto v : vertices)
	{
		globalVertices.push_back({ v.x + position.x, v.y + position.y });
	}
	return globalVertices;
}

void polygon::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	std::vector<vec2> gv = getGlobalVertices();

	for (int i = 0; i < (int)gv.size(); i++)
	{
		vec2 v1 = gv[i];
		vec2 v2 = gv[(i + 1) % (int)gv.size()];
		SDL_RenderDrawLine(renderer, (int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
	}

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderDrawLine(renderer, (int)mlineStartX, (int)linePositionY, (int)mlineEndX, (int)linePositionY);
	SDL_RenderDrawLine(renderer, (int)clineStartX, (int)linePositionY, (int)clineEndX, (int)linePositionY);
}

vec2 polygon::normalBetween(vec2 v1, vec2 v2)
{
	return { -(v1.y - v2.y), v1.x - v2.x };
}

float polygon::dot(vec2 v1, vec2 v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y);
}

polygon::projectedLine polygon::projectLine(std::vector<vec2> s, vec2 n)
{
	projectedLine pl = { INFINITY, -INFINITY };
	for (auto v : s)
	{
		float ps = dot(v, n);
		pl.start = std::min(ps, pl.start);
		pl.end = std::max(ps, pl.end);
	}
	return pl;
}

bool polygon::projectedLinesCollide(projectedLine p1, projectedLine p2)
{
	return (p1.start < p2.end && p1.end > p2.start);
}

bool polygon::checkOverlapSAT(std::vector<vec2> s1, std::vector<vec2> s2)
{
	for (int i = 0; i < (int)s1.size(); i++)
	{
		vec2 v = s1[i];
		vec2 nv = s1[(i + 1) % s1.size()];

		vec2 n = normalBetween(v, nv);
		projectedLine p1 = projectLine(s1, n);
		projectedLine p2 = projectLine(s2, n);

		if (!projectedLinesCollide(p1, p2))
			return false;
	}
	return true;
}

bool polygon::collidesWith(polygon& p)
{
	std::vector<vec2> s1 = getGlobalVertices();
	std::vector<vec2> s2 = p.getGlobalVertices();
	return checkOverlapSAT(s1, s2) && checkOverlapSAT(s2, s1);
}

polygon::projectedLine polygon::projectSliceX(std::vector<vec2> &s, float slice)
{
	std::vector<float> pointsOnSlice;

	for (int i = 0; i < s.size(); i++)
	{
		vec2 p1 = s[i];
		vec2 p2 = s[(i + 1) % s.size()];

		float minY = std::min(p1.y, p2.y);
		float maxY = std::max(p1.y, p2.y);

		if ((minY <= slice) && (maxY >= slice))
		{
			float minX = p1.x;
			float maxX = p2.x;
			float interpolant = (slice - p1.y) / (p2.y - p1.y);
			pointsOnSlice.push_back(minX + (maxX - minX) * interpolant);
		}
	}

	projectedLine l = { INFINITY, -INFINITY };
	for (auto p : pointsOnSlice)
	{
		l.start = std::min(l.start, p);
		l.end = std::max(l.end, p);
	}
	return l;
}

float resolve1D(float moverStart, float moverEnd, float colliderStart, float colliderEnd, float& axis)
{
	if (!(moverStart < colliderEnd && moverEnd > colliderStart))
		return INFINITY;

	return colliderStart - moverEnd;
}

static float responseAcrossLine(polygon::projectedLine l1, polygon::projectedLine l2)
{
	float distToStartOfLine2 = l2.start - l1.end;
	if (distToStartOfLine2 > 0)
		return INFINITY;

	float distToEndOfLine2 = l2.end - l1.start;
	if (distToEndOfLine2 < 0)
		return INFINITY;

	if (-distToStartOfLine2 < distToEndOfLine2) // negate distToStartOfLine2, cause it's always negative
		return distToStartOfLine2;
	else
		return distToEndOfLine2;
}

polygon::mtv* polygon::mtvBetween(std::vector<vec2> mover, std::vector<vec2> collider)
{
	float minResponseMagnitude = INFINITY;
	vec2 responseNormal = { 0, 0 };
	for (int i = 0; i < collider.size(); i++)
	{
		vec2 p1 = collider[i];
		vec2 p2 = collider[(i + 1) % collider.size()];
		vec2 cEdgeNormal = normalBetween(p1, p2);

		float diffX = p2.x - p1.x;
		float diffY = p2.y - p1.y;
		float l = std::sqrt(diffX * diffX + diffY * diffY);

		cEdgeNormal = { cEdgeNormal.x / l, cEdgeNormal.y / l };

		projectedLine mProjected = projectLine(mover, cEdgeNormal);
		projectedLine cProjected = projectLine(collider, cEdgeNormal);

		float responseMagnitude = responseAcrossLine(mProjected, cProjected);

		if (responseMagnitude == INFINITY)
			return nullptr;

		if (std::abs(responseMagnitude) < std::abs(minResponseMagnitude))
		{
			minResponseMagnitude = responseMagnitude;
			responseNormal = cEdgeNormal;
		}
	}

	return new mtv { responseNormal, minResponseMagnitude };
}

void polygon::resolveFor(polygon& p)
{
	clineStartX = 32;
	clineEndX = 64;

	std::vector<vec2> v1 = getGlobalVertices();
	std::vector<vec2> v2 = p.getGlobalVertices();

	mtv* r1 = mtvBetween(v1, v2);
	if (r1 == nullptr)
		return;

	mtv* r2 = mtvBetween(v2, v1);
	if (r2 == nullptr)
		return;

	r2->normal.x *= -1;
	r2->normal.y *= -1;

	mtv* min;

	if (std::abs(r2->magnitude) < std::abs(r1->magnitude))
	{
		min = r2;
		std::cout << r2->normal.x << std::endl;
	}
	else
	{
		min = r1;
	}

	if (min->magnitude == INFINITY)
		return;

	position.x += min->normal.x * min->magnitude;
	position.y += min->normal.y * min->magnitude;
}
