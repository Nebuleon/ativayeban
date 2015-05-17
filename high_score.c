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
#include "high_score.h"

#include <stdio.h>

#include "cfgpath.h"
#include "init.h"
#include "main.h"
#include "text.h"
#include "utils.h"


#define HIGH_SCORE_FILE "falling_time_high_scores"
#define MAX_HIGH_SCORES 20

CArray HighScores;

void HighScoresInit(void)
{
	CArrayInit(&HighScores, sizeof(HighScore));
	char buf[MAX_PATH];
	get_user_config_file(buf, MAX_PATH, HIGH_SCORE_FILE);
	if (strlen(buf) == 0)
	{
		printf("Error: cannot find config file path\n");
		return;
	}
	FILE *f = fopen(buf, "r");
	if (f == NULL)
	{
		printf("Error: cannot open config file %s\n", buf);
		return;
	}
	while (fgets(buf, sizeof buf, f))
	{
		HighScore hs;
		struct tm tm;
		sscanf(
			buf, "%d %04d-%02d-%02d %02d:%02d:%02d",
			&hs.Score, &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
			&tm.tm_hour, &tm.tm_min, &tm.tm_sec);
		// Time correction
		tm.tm_year -= 1900;
		tm.tm_mon--;
		hs.Time = mktime(&tm);
		CArrayPushBack(&HighScores, &hs);
	}
	fclose(f);
}
void HighScoresFree(void)
{
	CArrayTerminate(&HighScores);
}

void HighScoresAdd(const int s)
{
	// Find the right position to add the score
	// Scores are in descending order
	HighScore hsNew;
	hsNew.Score = s;
	hsNew.Time = time(NULL);
	bool inserted = false;
	for (int i = 0; i < (int)HighScores.size; i++)
	{
		const HighScore *hs = CArrayGet(&HighScores, i);
		if (hs->Score <= s)
		{
			CArrayInsert(&HighScores, i, &hsNew);
			inserted = true;
			break;
		}
	}
	if (!inserted)
	{
		CArrayPushBack(&HighScores, &hsNew);
	}

	// Save to file
	char buf[MAX_PATH];
	get_user_config_file(buf, MAX_PATH, HIGH_SCORE_FILE);
	if (strlen(buf) == 0)
	{
		printf("Error: cannot find config file path\n");
		return;
	}
	FILE *f = fopen(buf, "w");
	if (f == NULL)
	{
		printf("Error: cannot open config file %s\n", buf);
		return;
	}
	for (int i = 0; i < MIN((int)HighScores.size, MAX_HIGH_SCORES); i++)
	{
		const HighScore *hs = CArrayGet(&HighScores, i);
		struct tm *ptm = gmtime(&hs->Time);
		strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", ptm);
		fprintf(f, "%d %s\n", hs->Score, buf);
	}
	fclose(f);
}


TTF_Font *hsFont = NULL;

#define HIGH_SCORE_DISPLAY_DY 20.0f
#define HIGH_SCORE_DISPLAY_DDY 20.0f
#define TEXT_COUNTER_LENGTH 1.5f
#define TEXT_BLUE_LOW 64
#define TEXT_BLUE_HIGH 196

void HighScoreDisplayInit(HighScoreDisplay *h)
{
	h->y = SCREEN_HEIGHT;
	h->dy = -HIGH_SCORE_DISPLAY_DY;
	h->h = 0;
	h->textCounter = 0;
}

void HighScoreDisplayUpdate(HighScoreDisplay *h, const Uint32 ms)
{
	// If display reached top or bottom of screen, scroll the other way
	if (h->y >= SCREEN_HEIGHT - h->h && h->y >= 0)
	{
		h->dy -= HIGH_SCORE_DISPLAY_DDY * ms / 1000;
		if (h->dy < -HIGH_SCORE_DISPLAY_DY) h->dy = -HIGH_SCORE_DISPLAY_DY;
	}
	else if (h->y <= 0 && h->y + h->h <= SCREEN_HEIGHT)
	{
		h->dy += HIGH_SCORE_DISPLAY_DDY * ms / 1000;
		if (h->dy > HIGH_SCORE_DISPLAY_DY) h->dy = HIGH_SCORE_DISPLAY_DY;
	}
	h->y += h->dy * ms / 1000;
	h->textCounter += ms / TEXT_COUNTER_LENGTH / 1000;
	if (h->textCounter > 1.0f) h->textCounter -= 1.0f;
}

void HighScoreDisplayDraw(HighScoreDisplay *h)
{
	char buf[2048];
	strcpy(buf, "High Scores\n\n");
	const bool countHeight = h->h == 0;
	if (countHeight) h->h += TTF_FontHeight(hsFont) * 2;
	for (int i = 0; i < (int)HighScores.size; i++)
	{
		const HighScore *hs = CArrayGet(&HighScores, i);
		char lbuf[256];
		struct tm *ptm = gmtime(&hs->Time);
		char tbuf[256];
		strftime(tbuf, sizeof tbuf, "%Y-%m-%d", ptm);
		sprintf(lbuf, "#%d        %d (%s)\n", i + 1, hs->Score, tbuf);
		strcat(buf, lbuf);
		if (countHeight)
		{
			h->h += TTF_FontHeight(hsFont);
		}
	}
	// Pulsate
	const float scalar =
		h->textCounter > 0.5f ? 1.0f - h->textCounter : h->textCounter;
	SDL_Color c;
	c.b = TEXT_BLUE_LOW + (Uint8)((TEXT_BLUE_HIGH - TEXT_BLUE_LOW) * scalar);
	c.r = c.g = c.b / 2;
	TextRenderCentered(Screen, hsFont, buf, (int)h->y, c);
}