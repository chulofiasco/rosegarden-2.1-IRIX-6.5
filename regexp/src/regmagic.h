
/* Henry Spencer's regexp library, minor changes (to ANSI function
   interfaces only) by Chris Cannam */

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	MAGIC	0234
