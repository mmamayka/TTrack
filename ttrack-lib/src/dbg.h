#ifndef DBG_H
#define DBG_H

#include <stdlib.h>
#include <stdio.h>

/**
 * \brief Prints formated debug message.
 *
 * Firstly, prints head head format (like "ERROR AT %s (%i):"), secondly
 * prints body format with assotiated variadic arguments 
 * (like "SOME SHIT HAPPEN"), thidrly moves carret to the next line.
 *
 * \param [in] stream output stream the formated message to be printed.
 * \param [in] head_format format of the message header. May contain %s and %i 
 * 		sequences. 
 * \param [in] body_format format of the message body 
 * \param [in] ... associated with body_format variadic arguments.
 *
 * \warning Private function for DBG and ERR macro. Don't use it unless you have
 * 		your head up your ass. Otherwise %s and %i sequences in head_format must be 
 * 		written is THAT ORDER: %s before %i. They are hardcoded in printf.
 */
void DebugMessage(char const* const file, int line, FILE* const stream, 
		char const* const head_format, char const* const body_format, ...);

/* \def DBG(...)
 * \brief Prints debug message formated with printf rules to stdout stream in debug mode.
 * 		Does nothing in release mode (NDEBUG is defined).
 *
 * 	Format of the message: DEBUG <filename> (<line no>): <the message>.
 */

/* \def ERR(...)
 * \brief Prints error message formated with printf rules to stderr stream.
 *
 * Format of the message in debug mode (NDEBUG isn't defined):
 * 		ERROR AT <filename> (<line no>): <error message>.
 *
 * Format of the message in release mode (NDEBUG is defined):
 * 		ERROR: <error message>.
 */

#ifdef NDEBUG

#	define DBG(...) 

#	define ERR(...)																	\
	do {																			\
		DebugMessage(__FILE__, __LINE__, stderr, "ERROR: ", __VA_ARGS__);			\
		exit(EXIT_FAILURE);															\
	} while(0)

#else

#	define DBG(...) 																\
		DebugMessage(__FILE__, __LINE__, stdout, "DEBUG %s (%i): ", __VA_ARGS__)

#	define ERR(...) 																\
	do {																			\
		DebugMessage(__FILE__, __LINE__, stderr, "ERROR AT %s (%i): ", __VA_ARGS__);\
		exit(EXIT_FAILURE);															\
	} while(0)

#endif

#endif /* DBG_H */
