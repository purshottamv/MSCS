/* Tokens.  */
#define INT_NUM 258
#define REAL_NUM 259
#define STRING 260
#define CHAR 261
#define INT 262
#define DOUBLE 263
#define VOID 264
#define IDENTIFIER 265
#define BOOL_OP 266
#define REL_OP 267
#define PREFIX_OP 268
#define BREAK 269
#define RETURN 270
#define CONTINUE 271
#define FOR 272
#define WHILE 273
#define IF 274
#define ELSE 275
#define SWITCH 276
#define CASE 277
#define DEFAULT 278
#define COMMENT 279
#define LEXICAL_ERROR 280
#define END_OF_INPUT 281
#define OMP_PARALLEL	282
#define OMP_PARALLEL_FOR 283
#define OMP_FOR			284
#define OMP_SECTIONS	285
#define OMP_SECTION		286
#define DEFINE_MACRO	287
#define OMP_CRITICAL	288

#ifdef _INCLUDED_FROM_SCANNER_
const char* token_name[] = {
  "INT_NUM",
  "REAL_NUM",
  "STRING",
  "CHAR",
  "INT",
  "DOUBLE",
  "VOID",
  "IDENTIFIER",
  "BOOL_OP",
  "REL_OP",
  "PREFIX_OP",
  "BREAK",
  "RETURN",
  "CONTINUE",
  "FOR",
  "WHILE",
  "IF",
  "ELSE",
  "SWITCH",
  "CASE",
  "DEFAULT",
  "COMMENT",
  "LEXICAL_ERROR",
  "END_OF_INPUT",
	"OMP_PARALLEL",
	"OMP_PARALLEL_FOR",
	"OMP_FOR",
	"OMP_SECTIONS",
	"OMP_SECTION",
	"DEFINE_MACRO",
	"OMP_CRITICAL",
};

int first_token = 258;
#endif
