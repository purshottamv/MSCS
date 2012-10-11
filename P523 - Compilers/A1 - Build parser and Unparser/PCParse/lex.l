/* -*-c-*- */
/*----------------------------------------------------------------------------
 * Lexical analyzer (tokenizer) for PidginC.
 * Arun Chauhan, Dept of Computer Science, Indiana University, 2005, 2006.
 *----------------------------------------------------------------------------*/

%option noyywrap
%option never-interactive
%option yylineno
%array  /* define yytext as a character array, not as a pointer */

%{
  /*--------------------------------------------------------------------------
  *  GENERAL COMMENTS:
  * 
  *  This lexical scanner description is meant to be compiled with GNU flex
  *  v2.5.4 or greater.
  *---------------------------------------------------------------------------*/

#include <string.h>
#include "tokens.h"

#if ! (defined (FLEX_SCANNER) \
       && defined (YY_FLEX_MAJOR_VERSION) && YY_FLEX_MAJOR_VERSION >= 2 \
       && defined (YY_FLEX_MINOR_VERSION) && YY_FLEX_MINOR_VERSION >= 5)
#error lex.l requires flex version 2.5.4 or later
#endif


  // define YY_INPUT to insert a newline at the end of the input
#define YY_INPUT(buf, result, max_size) \
{ \
  if (feof(yyin)) \
     result = 0; \
  else { \
    unsigned char* b = (unsigned char*)(buf); \
    if (((result = fread( b, 1, max_size, yyin)) == 0) && ferror(yyin)) \
	 yy_fatal_error("input in flex scanner failed"); \
    if ((result < max_size) && feof(yyin)) \
	b[result++] = '\n'; \
  } \
}
  /*--------------------------------------------------------------------------*/
%}

LEX_D		[0-9]
LEX_S		[ \t]
LEX_NL		((\n)|(\r\n))
LEX_IDENT	([_a-zA-Z][_a-zA-Z0-9]*)
LEX_EXPON	([DdEe][+-]?{LEX_D}+)
LEX_INTEGER	({LEX_D}+)
LEX_REAL	(({LEX_D}+\.{LEX_D}*{LEX_EXPON}?)|({LEX_D}+{LEX_EXPON})|(\.{LEX_D}+{LEX_EXPON}?))
%%


%{
  /* Gobble C Comments. */
%}
"/*"([^*]|("*"+[^*/]))*"*"+"/" {
}

%{
  /* Keywords. */
%}

break {
  return BREAK;
}

continue {
  return CONTINUE;
}

case {
  return CASE;
}

else {
  return ELSE;
}

for {
  return FOR;
}

if {
  return IF;
}

default {
  return DEFAULT;
}

int {
  return INT;
}

double {
  return DOUBLE;
}

char {
  return CHAR;
}

return {
  return RETURN;
}

switch {
  return SWITCH;
}

void {
  return VOID;
}

while {
  return WHILE;
}


%{
  /* Operators.  Since '*', '+', and '-' are also unary operators these are
     returned as individual tokens. */
%}
"+"	{ return '+'; }
"-"	{ return '-'; }
"*"	{ return '*'; }
"/"	{ return '/'; }
"||"	{ return BOOL_OP; }
"&&"	{ return BOOL_OP; }
"<"	{ return REL_OP; }
">"	{ return REL_OP; }
"<="	{ return REL_OP; }
">="	{ return REL_OP; }
"=="	{ return REL_OP; }
"!="	{ return REL_OP; }
"!"	{ return PREFIX_OP; }
"&"	{ return PREFIX_OP; }
":"	{ return ':'; }
"="	{ return '='; }
";"     { return ';'; }

%{
  /* Other single character tokens */
%}
"("	{ return '('; }
")"	{ return ')'; }
"{"	{ return '{'; }
"}"	{ return '}'; }
"["	{ return '['; }
"]"	{ return ']'; }
"."	{ return '.'; }
","	{ return ','; }
"\""	{ return '"'; }


%{
  /* Real numbers.  Don't grab the `.' part of a dot operator as part of the
     constant. */
%}
{LEX_REAL} {
  return REAL_NUM;
}

%{
  /* Unsigned integer numbers.  The sign is returned as a separate token. */
%}
{LEX_INTEGER} {
  return INT_NUM;
}

%{
  /* Identifiers. */
%}
{LEX_IDENT} {
  return IDENTIFIER;
}


%{
  /* Strings. */
%}
(\"[^\"\n\r]*\")+ {
  return STRING;
}


({LEX_S}|{LEX_NL})+ {
  /* Just gobble up spaces and newlines */
}


%{
  /* End of file. */
%}
<<EOF>> {
  return END_OF_INPUT;
}


%{
  /* Unrecognized input is a lexical error. */
%}
. {
  return LEXICAL_ERROR;
}

%%
/*----------------------------------------------------------------------------*/

