#include "gap.h"

#include <math.h>

#include "SDL_image.h"

#include "game.h"
#include "main.h"

#define GAP_SPRITE_WIDTH 35
#define GAP_HEIGHT 20

struct GapSurfaces GapSurfaces = { NULL, NULL, NULL };

void GapInit(struct Gap* gap, float y, float gapLeft)
{
	gap->Passed = false;
	gap->Y = y;
	gap->GapLeft = gapLeft;
	gap->GapRight = gapLeft + GAP_WIDTH;
}

bool GapUpdate(struct Gap* gap, float scroll)
{
	// Update scroll.
	gap->Y += scroll;

	return GapBottom(gap) > FIELD_HEIGHT;
}

void GapDraw(const struct Gap* gap)
{
	// Draw the left and right parts of the gap.
	// Draw the end sprites for the middle ends,
	// and fill out to the edges with the middle sprites.

	// Left part
	int x = SCREEN_X(gap->GapLeft) - GAP_SPRITE_WIDTH;
	int y = SCREEN_Y(gap->Y);
	SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 };
	SDL_BlitSurface(GapSurfaces.Right, NULL, Screen, &dest);
	while (x >= 0)
	{
		x -= GAP_SPRITE_WIDTH;
		dest.x = (Sint16)x;
		dest.y = (Sint16)y;
		SDL_BlitSurface(GapSurfaces.Middle, NULL, Screen, &dest);
	}

	// Right part
	x = SCREEN_X(gap->GapRight);
	dest.x = (Sint16)x;
	dest.y = (Sint16)y;
	SDL_BlitSurface(GapSurfaces.Left, NULL, Screen, &dest);
	while (x < SCREEN_X(FIELD_WIDTH))
	{
		x += GAP_SPRITE_WIDTH;
		dest.x = (Sint16)x;
		dest.y = (Sint16)y;
		SDL_BlitSurface(GapSurfaces.Middle, NULL, Screen, &dest);
	}
}

float GapBottom(const struct Gap* gap)
{
	return gap->Y - GAP_HEIGHT * FIELD_HEIGHT / SCREEN_HEIGHT;
}

bool GapIsOn(const struct Gap* gap, float x, float radius)
{
	// TODO Circle physics.
	return x - radius <= gap->GapLeft || x + radius >= gap->GapRight;
}

bool GapSurfacesLoad(struct GapSurfaces* surfaces)
{
	surfaces->Left = IMG_Load("stoneHalfLeft.png");
	if (surfaces->Left == NULL)
	{
		return false;
	}
	surfaces->Middle = IMG_Load("stoneHalfMid.png");
	if (surfaces->Middle == NULL)
	{
		return false;
	}
	surfaces->Right = IMG_Load("stoneHalfRight.png");
	if (surfaces->Right == NULL)
	{
		return false;
	}
	return true;
}
extern void GapSurfacesFree(struct GapSurfaces* surfaces)
{
	SDL_FreeSurface(surfaces->Left);
	SDL_FreeSurface(surfaces->Middle);
	SDL_FreeSurface(surfaces->Right);
}
