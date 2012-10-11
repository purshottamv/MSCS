#define _INCLUDED_FROM_SCANNER_
#include "ruby.h"
#include "tokens.h"

int yylex (void);
extern char yytext[];
extern int yylineno;

ID id_push, id_clear, id_to_s;
VALUE obj_false;

static VALUE t_init (VALUE self)
{
  VALUE arr;

  arr = rb_ary_new();
  rb_iv_set(self, "@arr", arr);
  return self;
}

static VALUE t_next_token (VALUE self)
{
  VALUE arr;
  int token;

  token = yylex();
  arr = rb_iv_get(self, "@arr");
  rb_funcall(arr, id_clear, 0);
  if (token != END_OF_INPUT) {
    if (token >= first_token)
      rb_funcall(arr, id_push, 1, ID2SYM(rb_intern(token_name[token-first_token])));
    else
      rb_funcall(arr, id_push, 1, rb_str_new2(yytext));
    rb_funcall(arr, id_push, 1, rb_str_new2(yytext));
  } else {
    rb_funcall(arr, id_push, 1, Qfalse);
    rb_funcall(arr, id_push, 1, Qfalse);
  }
  return arr;
}

static VALUE t_to_s (VALUE self)
{
  VALUE arr;

  arr = rb_iv_get(self, "@arr");
  return rb_funcall(arr, id_to_s, 0);
}

static VALUE t_lineno (VALUE self)
{
  return INT2FIX(yylineno);
}

VALUE cScanner;

void Init_scanner ()
{
  cScanner = rb_define_class("Scanner", rb_cObject);
  rb_define_method(cScanner, "initialize", t_init, 0);
  rb_define_method(cScanner, "next_token", t_next_token, 0);
  rb_define_method(cScanner, "lineno", t_lineno, 0);
  id_push = rb_intern("push");
  id_clear = rb_intern("clear");
  id_to_s = rb_intern("to_s");
  obj_false = rb_gv_get("false");
}
