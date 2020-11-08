#ifndef TEXT_H
#define TEXT_H

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */


#include <stdio.h>
#include <stdint.h>

/// Function FileSize() returns this value if any error occured.
#define FSIZE_ERR SIZE_MAX 

/// Contains pointers to first and last ('\0') line symbols.
typedef struct {
	char* first; ///< pointer to the first symbol.
	char* last;  ///< pointer to the last symbol.
} string_t;

/**
 * \brief Returns size of the file in bytes. Rewinds the stream.
 *
 * \param [in] file file handle. Must be nonnull. Error indicator must be unset.
 * \return size of the file in bytes or FS_ERR if smth bad happen in stdio.
 */
size_t const fsize(FILE* const file);

/// Error codes for ReadText() function.
typedef enum {
	RT_OK 	  = 0, ///< Terminated successfully.
	RT_MEMORY = 1, ///< Out of memory (malloc returned NULL).
	RT_IO 	  = 2  ///< IO functions terminated with error. See ferror(), perror().
} RT_err_t;
	
/**
 * \brief Reads whole data from the stream. Modifies stream state.
 *
 * \param [in] file file with data to be read. Must be nonnull. Error indicator must be
 * 		unset.
 * \param [out] psize pointer to size of the array. Unmodified in case of any error.
 * 		Must be nonnull.
 * \param [out] errc error code (RT_OK if the function terminated successfully).
 *		May be NULL. 
 *
 * \return array of bytes read from the file or NULL in case of any error.
 *
 * \warning You have to call free() for the return value.
 * \warning In case of any error state of stream will be modified too.
 */
char* const read_text(FILE* const file, size_t* const psize, RT_err_t* const errc);

/// Error codes for ReadFile() function
typedef enum {
	RF_OK 	  	= 0, ///< Terminated successfully.
	RF_NOTFOUND = 1, ///< File not found.
	RF_MEMORY 	= 2, ///< Out of memory (malloc returned NULL).
	RF_STDIO 	= 3, ///< stdio error. See ferror().
} RF_err_t;

/// Associates error codes of function ReadFile() with error strings.
char const * const RF_errstr(RF_err_t const errc);

/**
 * \brief Reads whole data from the file to array.
 *
 * \param[in] fname name of the file. Must be nonnull.
 * \param[out] psize size of the data was read. Must be nonull.
 * \param[out] errc error code (RF_OK if the function terminated successfully).
 * 		May be NULL.
 * \return byte array read from the file or NULL in case of any error.
 *
 * \warning You have to call free() for the return value.
 */
char* const read_text2(char const* const fname, size_t* const psize, RF_err_t* const errc);

/**
 * \brief Splits text on array of separate lines.
 *
 * \param[in] text the text to be processed
 * \param[in] size size of the text in bytes
 * \param[out] pnlines number of lines. Unmodified if the function failed
 * \return pointer to the lines or NULL if malloc() failed.
 *
 * \warning You have to call free() for the return value.
 */
string_t* const get_text_lines(char* const text, size_t const size, size_t* const pnlines);

/**
 * \brief Count a number of lines in the text.
 *
 * \param[in] text pointer to the text.
 * \param[in] size length of the text.
 *
 * \return a number of lines in text.
 */
size_t const count_lines(char const * const text, size_t const size);

/**
 * \brief Splits the text on array of separated lines. Places '\0' in place of '\n'.
 *
 * \param [in] text the text to be processed.
 * \param [in] size size of the text.
 * \param[out] array of pointers to lines first symbol.
 * \param[out] array of pointers to lines last ('\0') symbol.
 *
 * \return count of lines.
 */
size_t const split_text(char* const text, size_t const size, string_t* const lines);

/**
 * \brief Writes lines to file.
 *
 * \param[in] file handle of the file to write. Must be nonnull. Error indicator must
 * 		be unset.
 * \param[in] lines pointer to the lines array. Must be nonnull.
 * \param[in] nlines count of the lines.
 *
 * \return 1 in case of success, 0 otherwise.
 */
int const write_lines(FILE* const file, string_t const * const lines, 
					   size_t const nlines);

/**
 * \brief Writes lines to file.
 *
 * \param[in] file name. Must be nonnull.
 * \param[in] lines pointer to the lines array. Must be nonnull.
 * \param[in] nlines count of the lines.
 *
 * \return 1 in case of success, 0 otherwise.
 */
int const write_lines2(char const* const fname, string_t const * const lines,
					size_t const nlines);

/* 
 * \brief Places '\0' in places of '\n' symbols in all text.
 *
 * \param[in] text pointer to the text. Must be nonnull.
 * \param[in] size size of the text
 */
void repair_text(char* const text, size_t const size);

/*
 * \brief Writes text to file
 *
 * \param[in] filename file name. Must be nonnull.
 * \param[in] text pointer to the text. Must be nonnull.
 * \param[in] size size of the text.
 */
int const write_text(char const* const filename, char const* const text, 
					 size_t const size);

#endif
