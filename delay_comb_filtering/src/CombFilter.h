/*
 * CombFilter.h
 *
 *  Created on: Aug 8, 2020
 *      Author: superkittens
 */

#ifndef SRC_COMBFILTER_H_
#define SRC_COMBFILTER_H_

#include "arm_math.h"
#include "DelayLine.h"
#include "stdlib.h"



//	Feed-forward Comb Filter
typedef struct
{
	float32_t b0;
	float32_t bm;
	DelayLine *M;
}FFCF;


//	Feedback Comb Filter
typedef struct
{
	float32_t b0;
	float32_t am;
	DelayLine *M;
}FBCF;


//	Allpass Comb Filter (Direct Form I)
typedef struct
{
	FFCF *ff;
	FBCF *fb;
}APCF;



FFCF		*createFFCF(size_t M, float32_t b0, float32_t bm);
FBCF		*createFBCF(size_t M, float32_t b0, float32_t am);
APCF		*createAPCF(size_t M, float32_t b0, float32_t am);

void 		deleteFFCF(FFCF *f);
void 		deleteFBCF(FBCF *f);
void 		deleteAPCF(APCF *a);

int 		ffcfShift(FFCF *f, float32_t x, float32_t *y);
int 		fbcfShift(FBCF *f, float32_t x, float32_t *y);
int 		apcfShift(APCF *a, float32_t x, float32_t *y);



#endif /* SRC_COMBFILTER_H_ */
