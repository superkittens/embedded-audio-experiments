/*
 * DelayLine.c
 *
 *  Created on: Aug 8, 2020
 *      Author: superkittens
 */

#include "DelayLine.h"


DelayLine *createDelayLine(size_t M)
{
	DelayLine *d = (DelayLine *)malloc(sizeof(DelayLine));
	if (d == NULL)
		return NULL;

	if (M != 0)
	{
		d->buffer = (float *)malloc(sizeof(float) * M);
		if (d->buffer == NULL)
		{
			free(d);
			return NULL;
		}

		arm_fill_f32(0.f, d->buffer, M);
	}
	else
		d->buffer = NULL;

	d->M = M;
	d->currentPtr = 0;

	return d;
}


void deleteDelayLine(DelayLine *d)
{
	if (d == NULL)
		return;

	if (d->buffer != NULL)
	{
		free(d->buffer);
		d->buffer = NULL;
	}

	free(d);
	d = NULL;

	return;
}


int delayLineShift(DelayLine *d, float32_t x, float32_t *y)
{
	if (d == NULL) return -1;
	if (d->M != 0)
	{
		//	0 <= currentPtr < N assertion
		if (d->currentPtr >= d->M)
			return -1;

		*y = d->buffer[d->currentPtr];
		d->buffer[d->currentPtr] = x;

		d->currentPtr += 1;
		if (d->currentPtr >= d->M)
			d->currentPtr = 0;
	}

	//	Pass-through case (N = 0)
	else
		*y = x;

	return 0;
}


int delayLinePeek(DelayLine *d, float32_t *y)
{
	if (d == NULL) return -1;
	if (d->currentPtr >= d->M) return -1;

	*y = d->buffer[d->currentPtr];

	return 0;
}


