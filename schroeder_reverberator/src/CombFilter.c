/*
 * CombFilter.c
 *
 *  Created on: Aug 8, 2020
 *      Author: superkittens
 */

#include "CombFilter.h"


FFCF *createFFCF(size_t M, float32_t b0, float32_t bm)
{
	FFCF *f = (FFCF *)malloc(sizeof(FFCF));
	if (f == NULL)
		return NULL;

	f->M = createDelayLine(M);
	if (f->M == NULL)
	{
		free(f);
		return NULL;
	}

	f->b0 = b0;
	f->bm = bm;

	return f;
}


FBCF *createFBCF(size_t M, float32_t b0, float32_t am)
{
	FBCF *f = (FBCF *)malloc(sizeof(FBCF));
	if (f == NULL)
		return NULL;

	f->M = createDelayLine(M);
	if (f->M == NULL)
	{
		free(f);
		return NULL;
	}

	f->b0 = b0;
	f->am = am;

	return f;
}


APCF *createAPCF(size_t M, float32_t b0, float32_t am)
{
	APCF *a = (APCF *)malloc(sizeof(APCF));
	if (a == NULL)
		return NULL;

	a->ff = createFFCF(M, b0, 1);
	if (a->ff == NULL)
	{
		free(a);
		return NULL;
	}

	a->fb = createFBCF(M, 1, am);
	if (a->fb == NULL)
	{
		free(a);
		return NULL;
	}

	return a;
}


void deleteFFCF(FFCF *f)
{
	if (f == NULL) return;

	deleteDelayLine(f->M);
	free(f);
	f = NULL;

	return;
}


void deleteFBCF(FBCF *f)
{
	if (f == NULL) return;

	deleteDelayLine(f->M);
	free(f);
	f = NULL;

	return;
}


void deleteAPCF(APCF *a)
{
	if (a == NULL) return;

	deleteFFCF(a->ff);
	deleteFBCF(a->fb);

	free(a);
	a = NULL;

	return;
}


int ffcfShift(FFCF *f, float32_t x, float32_t *y)
{
	if (f == NULL) return -1;

	float d_y = 0;
	int status = delayLineShift(f->M, x, &d_y);

	if (status < 0)
		return -1;

	*y = (x * f->b0) + (d_y * f->bm);

	return 0;
}


int fbcfShift(FBCF *f, float32_t x, float32_t *y)
{
	if (f == NULL) return -1;

	float32_t delayOut = 0;
	delayLinePeek(f->M, &delayOut);
	float32_t v = (delayOut * f->am) + x;
	int status = delayLineShift(f->M, v, &delayOut);

	if (status < 0)
		return -1;

	*y = v * f->b0;

	return 0;
}


int apcfShift(APCF *a, float32_t x, float32_t *y)
{
	if (a == NULL) return -1;

	float32_t d_y = 0;
	int status = ffcfShift(a->ff, x, &d_y);
	if (status < 0)
		return -1;

	status = fbcfShift(a->fb, d_y, &d_y);
	if (status < 0)
		return -1;

	*y = d_y;

	return 0;
}


