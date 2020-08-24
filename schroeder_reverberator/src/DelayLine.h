/*
 * DelayLine.h
 *
 *  Created on: Aug 8, 2020
 *      Author: superkittens
 */

#ifndef SRC_DELAYLINE_H_
#define SRC_DELAYLINE_H_

#include "arm_math.h"
#include "stdlib.h"

typedef struct
{
	float32_t *buffer;
	size_t currentPtr;
	size_t M;
}DelayLine;


DelayLine		*createDelayLine(size_t M);
void 			deleteDelayLine(DelayLine *d);
int 			delayLineShift(DelayLine *d, float32_t x, float32_t *y);
int 			delayLinePeek(DelayLine *d, float32_t *y);


#endif /* SRC_DELAYLINE_H_ */
