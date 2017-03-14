#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * We're using a dynamic, resizable buffer to read strings.  If we 
 * encounter an input string that's longer than the current buffer 
 * size, attempt to double it.  
 */
int expand( char **buf, size_t *bufsize )
{
  char *tmp = realloc( *buf, *bufsize * 2 );
  if ( tmp )
  {
    *bufsize *= 2;
    *buf = tmp;
  }
  return tmp != NULL;
}

/**
 * Read the next token from the input stream.  In this program,
 * tokens are either sequences of alphanumeric characters, or individual
 * non-alphanumeric characters.  Both are saved as strings to buf.  
 *
 * For example, the input ">>text3.txt" would be tokenized as ">", ">", 
 * "text3", ".", "txt".  
 */
int getNextSym( FILE *stream, char **buf, size_t *bufsize )
{
  size_t charsRead = 0;
  int c;

  while ( ( c = fgetc( stream ) ) != EOF )
  {
    if ( !isalpha( c ) && !isdigit( c ) )
    {
      if ( charsRead > 0 )
      {
        /**
         * If we've read a sequence of alphanumeric characters,
         * we *don't* want this non-alphanumeric character to be
         * part of the string (IOW, we treat all non-alphanumerics
         * as string delimiters).  Push the current character back
         * on to the input stream, terminate the buffer, and return.
         */
        ungetc( c, stream );
        (*buf)[charsRead] = 0;
        return 1;
      }
      else
      {
        /**
         * All non-alphanumeric characters are returned in their own
         * 1-character strings.
         */
        (*buf)[0] = c;
        (*buf)[1] = 0;
        return 1;
      }
    }
    else
    {
      /**
       * Current character is alphanumeric, so we want to
       * append it to the buffer, extending the buffer if
       * necessary.  
       */
       if ( (charsRead+1) < *bufsize || expand( buf, bufsize ) ) 
      {
        (*buf)[charsRead++] = c;
      }
      else if ( charsRead == *bufsize )
      {
        /**
         * Buffer expansion failed; push the latest character back on
         * to the input stream, terminate the buffer, and return a failure
         * code.  It's up to the caller to decide what to do next.  
         */
        ungetc( c, stream );
        (*buf)[charsRead-1] = 0;
        return 0;
      }
    }
  }
  if ( charsRead > 0 )
  {
    /**
     * We hit EOF while reading an alphanumeric sequence; terminate
     * the buffer.  
     */
    (*buf)[charsRead] = 0;
    return 1;
  }

  return 0;
}

/**
 * Parse the sequence number out of the file name.  This assumes the
 * file name is of the form "toolchain<N>.sh". 
 */
int getOrdinal( const char *filename )
{
  int ord;
  if ( sscanf( filename, "toolchain%d.sh", &ord ) == 1 )
    return ord;

  return -1;
}

int main( int argc, char **argv )
{
  // arg1 = filename
  // arg2 = symbol to replace
  // arg3 = replacement symbol
  
  if ( argc < 4 )
  {
    fprintf( stderr, "USAGE: %s toolchain<N>.sh <search-sym> <replace-sym>\n", argv[0] );
    exit( EXIT_FAILURE );
  }

  char *filename = argv[1];
  char *sym1     = argv[2];
  char *sym2     = argv[3];

  int ordinal = getOrdinal( filename );
  if ( ordinal < 0 )
  {
    fprintf( stderr, "could not parse out ordinal from %s\n", filename );
    exit( EXIT_FAILURE );
  }

  /**
   * 32-bit integer is ~10 decimal digits, round up to 11, add 1 for the 
   * 0 terminator.  You'll obviously want to change that for 64-bit int.  
   */
  char ofilename[ sizeof "toolchain" + sizeof ".sh" + 12 ];
  if ( !sprintf( ofilename, "toolchain%d.sh", ordinal + 1 ) )
  {
    fprintf( stderr, "problem while building output file name toolchain%d.sh", ordinal + 1 );
    exit( EXIT_FAILURE );
  }

  FILE *input = fopen( filename, "r" );
  if ( !input )
  {
    fprintf( stderr, "could not open %s\n", filename );
    exit( EXIT_FAILURE );
  }

  FILE *output = fopen( ofilename, "w" );
  if ( !output )
  {
    fprintf( stderr, "could not create %s\n", ofilename );
    exit( EXIT_FAILURE );
  }

  size_t bufsize = 80; // initial buffer size
  char *buffer = malloc( bufsize );

  /**
   * Meat of the program - read the next symbol from the input file.
   * If it matches our search symbol, write the replacement symbol
   * to the output file - otherwise, copy the input symbol to the
   * output file.  **/
  while ( getNextSym( input, &buffer, &bufsize ) )
  {
    if ( strcmp( buffer, sym1 ) == 0 )
      fprintf( output, "%s", sym2 );
    else
      fprintf( output, "%s", buffer );
  }

  free( buffer );
  fclose( input );
  fclose( output );

  return 0;
}

