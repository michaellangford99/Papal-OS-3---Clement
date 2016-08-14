#include "string.h"

// Basic string functions

void strcpy( char *d, const char *s )
{
	while(*s) {
		*d++ = *s++;
	}
	*d = 0;
}

int strcmp( const char *a, const char *b )
{
	while(1) {
		if(*a<*b) {
			return -1;
		} else if(*a>*b) {
			return 1;
		} else if(*a==0) {
			return 0;
		} else {
			a++;
			b++;
		}
	}			
}

int strncmp( const char *a, const char *b, unsigned length )
{
	while(length>0) {
		if(*a<*b) {
			return -1;
		} else if(*a>*b) {
			return 1;
		} else if(*a==0) {
			return 0;
		} else {
			a++;
			b++;
			length--;
		}
	}
	return 0;
}

unsigned strlen( const char *s )
{
	unsigned len=0;
	while(*s) {
		len++;
		s++;
	}
	return len;
}

unsigned strlen_int( const int *s )
{
	unsigned len=0;
	while(*s) {
		len++;
		s++;
	}
	return len;
}

const char * strchr( const char *s, char ch )
{
	while(*s) {
		if(*s==ch) return s;
		s++;
	}
	return 0;
}

char * strtok ( char *s, const char *delim)
{
	static char *oldword=0;
	char *word;

	if(!s) s=oldword;

	while(*s && strchr(delim,*s)) s++;

	if(!*s) {
		oldword = s;
		return 0;
	}

	word = s;
	while(*s && !strchr(delim,*s)) s++;

	if(*s) {
		*s = 0;
		oldword = s+1;
	} else {
		oldword = s;
	}

	return word;
}

void	memset( char *vd, char value, unsigned length )
{
	char *d = vd;
	while(length) {
		*d = value;
		length--;
		d++;
	}
}

void	memcpy( char *vd, const char *vs, unsigned length )
{
	char *d = vd;
	const char *s = vs;
	while(length) {
		*d = *s;
		d++;
		s++;
		length--;
	}
}
