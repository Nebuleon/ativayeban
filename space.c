/*
Copyright (c) 2015, Cong Xu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "space.h"

#include "game.h"
#include "utils.h"

Space space;

void SpaceInit(Space *s)
{
	memset(s, 0, sizeof *s);

	// Init physics space
	s->Space = cpSpaceNew();
	cpSpaceSetIterations(s->Space, 30);
	cpSpaceSetGravity(s->Space, cpv(0, GRAVITY));
	cpSpaceSetCollisionSlop(s->Space, 0.5);
	cpSpaceSetSleepTimeThreshold(s->Space, 1.0f);

	SpaceReset(s);
}
static void RemoveEdgeShape(cpBody *body, cpShape *shape, void *data);
static void AddEdgeShapes(Space *s, const float y);
void SpaceReset(Space *s)
{
	if (s->edgeBodies != NULL)
	{
		cpBodyEachShape(s->edgeBodies, RemoveEdgeShape, s->Space);
	}

	// Segments around screen
	s->edgeBodies = cpSpaceGetStaticBody(s->Space);
	AddEdgeShapes(s, 0);
	s->edgeBodiesBottom = -FIELD_HEIGHT * 4;
}
void SpaceFree(Space *s)
{
	cpSpaceFree(s->Space);
}

void SpaceAddBottomEdge(Space *s)
{
	cpShapeFilter edgeFilter =
	{
		CP_NO_GROUP, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES
	};
	cpShape *shape = cpSpaceAddShape(s->Space, cpSegmentShapeNew(
		s->edgeBodies, cpv(0, 0), cpv(FIELD_WIDTH, 0), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, edgeFilter);
}

// As the camera scrolls down, need to create new edge bodies
void SpaceUpdate(Space *s, const float y)
{
	if (y < s->edgeBodiesBottom)
	{
		cpBodyEachShape(s->edgeBodies, RemoveEdgeShape, s->Space);
		AddEdgeShapes(s, y);
	}
}

static void RemoveEdgeShape(cpBody *body, cpShape *shape, void *data)
{
	UNUSED(body);
	cpSpace *s = data;
	cpSpaceRemoveShape(s, shape);
	cpShapeFree(shape);
}
static void AddEdgeShapes(Space *s, const float y)
{
	cpShape *shape;
	cpShapeFilter edgeFilter =
	{
		CP_NO_GROUP, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES
	};
	const float top = y + FIELD_HEIGHT * 2;
	// Left edge
	s->edgeBodiesBottom = y - FIELD_HEIGHT * 4;
	shape = cpSpaceAddShape(s->Space, cpSegmentShapeNew(
		s->edgeBodies, cpv(0, s->edgeBodiesBottom), cpv(0, top), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, edgeFilter);
	// Right edge
	shape = cpSpaceAddShape(s->Space, cpSegmentShapeNew(
		s->edgeBodies, cpv(FIELD_WIDTH, s->edgeBodiesBottom),
		cpv(FIELD_WIDTH, top), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetFilter(shape, edgeFilter);
}
