/**
 * \file
 * \brief File contains functions and macros for unit testing.
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include "comp.h"

/**
 * \brief Call the function and compares return value with the reference value.
 * 		Log details to stderr if something gone wrong.
 *
 * \param[in] call complete function call
 * \param[in] ref int reference value for the function return value
 */
#define TEST_IRV(call, ref)														\
	do { 																				\
		int res = call;																\
		if(res != ref)																	\
			fprintf(stderr, "FAIL AT %s(%i): " #call " == %i, but should be %i\n", 		\
					__FILE__, __LINE__, res, ref);										\
	} while(0)


/**
 * \brief Compares the value with the reference value. Log details to stderr of 
 * 		someting gone wrong.
 *
 * 	\param[in] val the value
 * 	\param[in] ref the reference value
 *	\param[in] eps comparison accuracy
 */
#define TEST_DV(val, ref, eps) 															\
	do {																				\
		if(!IsAbout(val, ref, eps))														\
			printf("FAILT AT %s(%i): " #val " == %lf, but should be %lf\n",				\
					__FILE__, __LINE__, val, ref);										\
	} while(0)
	

#endif /* TEST_H */
