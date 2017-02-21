#ifndef STRING_H
#define STRING_H

#define null NULL //make null and NULL identical - not sure if this is a good idea...
// Basic string functions

void strcpy( char *d, const char *s );
int strcmp( const char *a, const char *b );
int strncmp( const char *a, const char *b, unsigned length );
unsigned strlen( const char *s );
unsigned strlen_int( const int *s );
const char * strchr( const char *s, char ch );
char * strtok ( char *s, const char *delim);
void	memset( char *vd, char value, unsigned length );
void	memcpy( char *vd, const char *vs, unsigned length );

#endif