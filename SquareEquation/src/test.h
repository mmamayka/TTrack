#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include "comp.h"

#define TEST_IRV_(call, ref, file, line)												\
	do { 																				\
		int res = call;																	\
		if(res != ref)																	\
			fprintf(stderr, "FAIL AT %s(%i): " #call " == %i, but should be %i\n", 		\
					file, line, res, ref);												\
	} while(0)
#define TEST_IRV(call, ref) TEST_IRV_(call, ref, __FILE__, __LINE__)



#define TEST_DV_(val, ref, eps,  file, line) 											\
	do {																				\
		if(!IsAbout(val, ref, eps))														\
			printf("FAILT AT %s(%i): " #val " == %lf, but should be %lf\n",				\
					file, line, val, ref);												\
	} while(0)
#define TEST_DV(val, ref, eps) \
	TEST_DV_(val, ref, eps, __FILE__, __LINE__)
	

#endif /* TEST_H */
