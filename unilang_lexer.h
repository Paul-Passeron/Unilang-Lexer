/**
 * unilang_lexer.h
 * Copyright (C) 2024 Paul Passeron
 * UNILANG_LEXER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef UNILANG_LEXER_H
#define UNILANG_LEXER_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
  TK_UNKNOWN,
  TK_IDENTIFIER,
  TK_KEYWORD,
  TK_STRLIT,
  TK_NUMLIT,
  TK_CHARLIT,
  TK_PLUS,
  TK_MINUS,
  TK_MULT,
  TK_DIV,
  TK_MODULO,
  TK_GRTR,
  TK_GRTR_EQ,
  TK_LSSR,
  TK_LSSR_EQ,
  TK_AND,
  TK_OR,
  TK_NOT,
  TK_EQ,
  TK_DIFF,
  TK_BIGARR,
  TK_SMALLARR,
  TK_SMALLARRLARGE,
  TK_OPENPAREN,
  TK_CLOSEPAREN,
  TK_OPENBRACKET,
  TK_OPENBRACE,
  TK_CLOSEBRACKET,
  TK_CLOSEBRACE,
  TK_COLON,
  TK_SEMICOLON,
  TK_DOT,
  TK_COMMA,
  TK_LOG_AND,
  TK_LOG_OR,
  TK_INCLUDE,
} TokenKind;

typedef struct {
  TokenKind type;
  const char *start; // Start of token in the source code
  const char *filename;
  int length; // Length of the token
  int line;   // Line number where the token appears
  int col;    // Col. number where the token appears
} Token;

typedef struct {
  const char *filename;
  const char *start;
  const char *current;
  int length;
  int line;
  int col;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);
void lexer_init_from_file(Lexer *lexer, const char *filename);
Token lexer_next_token(Lexer *lexer);
const char *token_kind_to_string(TokenKind type);
Token *tokenize(const char *filename, int *length);
void debug_print_includes(void);
void free_tokenized(void);
void print_lexeme(FILE *stream, Token t);
int get_precedence(TokenKind t);
bool is_unary_operator(TokenKind t);

#ifdef __cplusplus
}
#endif // __cplusplus

// #define UNILANG_LEXER_IMPL
#ifdef UNILANG_LEXER_IMPL

#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *token_kind_to_str(TokenKind t) {
  switch (t) {
  case TK_STRLIT:
    return "TK_STRLIT";
  case TK_NUMLIT:
    return "TK_NUMLIT";
  case TK_CHARLIT:
    return "TK_CHARLIT";
  case TK_IDENTIFIER:
    return "TK_IDENTIFIER";
  case TK_KEYWORD:
    return "TK_KEYWORD";
  case TK_PLUS:
    return "TK_PLUS";
  case TK_MINUS:
    return "TK_MINUS";
  case TK_MULT:
    return "TK_MULT";
  case TK_DIV:
    return "TK_DIV";
  case TK_MODULO:
    return "TK_MODULO";
  case TK_GRTR:
    return "TK_GRTR";
  case TK_GRTR_EQ:
    return "TK_GRTR_EQ";
  case TK_LSSR:
    return "TK_LSSR";
  case TK_LSSR_EQ:
    return "TK_LSSR_EQ";
  case TK_AND:
    return "TK_AND";
  case TK_OR:
    return "TK_OR";
  case TK_NOT:
    return "TK_NOT";
  case TK_EQ:
    return "TK_EQ";
  case TK_DIFF:
    return "TK_DIFF";
  case TK_BIGARR:
    return "TK_BIGARR";
  case TK_SMALLARR:
    return "TK_SMALLARR";
  case TK_OPENPAREN:
    return "TK_OPENPAREN";
  case TK_CLOSEPAREN:
    return "TK_CLOSEPAREN";
  case TK_OPENBRACKET:
    return "TK_OPENBRACKET";
  case TK_OPENBRACE:
    return "TK_OPENBRACE";
  case TK_CLOSEBRACKET:
    return "TK_CLOSEBRACKET";
  case TK_CLOSEBRACE:
    return "TK_CLOSEBRACE";
  case TK_COLON:
    return "TK_COLON";
  case TK_SEMICOLON:
    return "TK_SEMICOLON";
  case TK_DOT:
    return "TK_DOT";
  case TK_COMMA:
    return "TK_COMMA";
  default:
    return "";
  }
}

int get_precedence(TokenKind t) {
  switch (t) {
  case TK_MULT:
  case TK_DIV:
  case TK_MODULO:
    return 11;
  case TK_MINUS:
    return 10;
  case TK_PLUS:
    return 9;
  case TK_GRTR:
  case TK_GRTR_EQ:
  case TK_LSSR:
  case TK_LSSR_EQ:
    return 7;
  case TK_EQ:
  case TK_DIFF:
    return 6;
  case TK_LOG_AND:
    return 5;
  case TK_LOG_OR:
    return 3;
  case TK_AND:
    return 2;
  case TK_OR:
    return 1;
  default:
    return -1;
  }
}

bool is_unary_operator(TokenKind t) {
  switch (t) {
  case TK_MINUS:
  case TK_PLUS:
  case TK_NOT:
    return true;
  default:
    return false;
  }
}

void print_lexeme(FILE *stream, Token t) {
  for (int i = 0; i < t.length; i++) {
    putc(*(t.start + i), stream);
  }
}

const char *token_kind_to_string(TokenKind type) {
  switch (type) {
  case TK_UNKNOWN: {
    return "TK_UNKNOWN";
  }
  case TK_IDENTIFIER: {
    return "TK_IDENTIFIER";
  }
  case TK_KEYWORD: {
    return "TK_KEYWORD";
  }
  case TK_STRLIT: {
    return "TK_STRLIT";
  }
  case TK_NUMLIT: {
    return "TK_NUMLIT";
  }
  case TK_CHARLIT: {
    return "TK_CHARLIT";
  }
  case TK_PLUS: {
    return "TK_PLUS";
  }
  case TK_MINUS: {
    return "TK_MINUS";
  }
  case TK_MULT: {
    return "TK_MULT";
  }
  case TK_DIV: {
    return "TK_DIV";
  }
  case TK_MODULO: {
    return "TK_MODULO";
  }
  case TK_GRTR: {
    return "TK_GRTR";
  }
  case TK_GRTR_EQ: {
    return "TK_GRTR_EQ";
  }
  case TK_LSSR: {
    return "TK_LSSR";
  }
  case TK_LSSR_EQ: {
    return "TK_LSSR_EQ";
  }
  case TK_AND: {
    return "TK_AND";
  }
  case TK_OR: {
    return "TK_OR";
  }
  case TK_NOT: {
    return "TK_NOT";
  }
  case TK_EQ: {
    return "TK_EQ";
  }
  case TK_DIFF: {
    return "TK_DIFF";
  }
  case TK_BIGARR: {
    return "TK_BIGARR";
  }
  case TK_SMALLARR: {
    return "TK_SMALLARR";
  }
  case TK_SMALLARRLARGE: {
    return "TK_SMALLARRLARGE";
  }
  case TK_OPENPAREN: {
    return "TK_OPENPAREN";
  }
  case TK_CLOSEPAREN: {
    return "TK_CLOSEPAREN";
  }
  case TK_OPENBRACKET: {
    return "TK_OPENBRACKET";
  }
  case TK_OPENBRACE: {
    return "TK_OPENBRACE";
  }
  case TK_CLOSEBRACKET: {
    return "TK_CLOSEBRACKET";
  }
  case TK_CLOSEBRACE: {
    return "TK_CLOSEBRACE";
  }
  case TK_COLON: {
    return "TK_COLON";
  }
  case TK_SEMICOLON: {
    return "TK_SEMICOLON";
  }
  case TK_DOT: {
    return "TK_DOT";
  }
  case TK_COMMA: {
    return "TK_COMMA";
  }
  case TK_LOG_AND: {
    return "TK_LOG_AND";
  }
  case TK_LOG_OR: {
    return "TK_LOG_OR";
  }
  case TK_INCLUDE: {
    return "TK_INCLUDE";
  }
  default:
    return "";
  }
}

char lexer_consume_char(Lexer *lexer) {
  if (!*lexer->current)
    return 0;
  char res = *(lexer->current++);
  lexer->col++;
  if (res == '\n') {
    lexer->line++;
    lexer->col = 1;
  }
  return res;
}

void lexer_consume_n_chars(Lexer *lexer, int n) {
  while (n--)
    lexer_consume_char(lexer);
}

char lexer_peek_char(Lexer lexer) { return lexer_consume_char(&lexer); }

char lexer_peek_n_char(Lexer lexer, int n) {
  for (int i = 0; i < n - 1; i++)
    lexer_consume_char(&lexer);
  return lexer_peek_char(lexer);
}

int lexer_matches_string(Lexer lexer, const char *str) {
  while (*str) {
    if (lexer_consume_char(&lexer) != *(str++))
      return 0;
  }
  return 1;
}

void lexer_init(Lexer *lexer, const char *source) {
  lexer->start = source;
  lexer->length = strlen(source);
  lexer->current = source;
  lexer->line = 1;
  lexer->col = 1;
  lexer->filename = NULL;
}
// /!\ This function mallocs lexer->souree
void lexer_iniTK_from_file(Lexer *lexer, const char *path) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    assert(false && "TODO: Error reporting, Could not open file at path");
  }
  fseek(f, 0, SEEK_END);
  size_t length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *source = (char *)malloc(length + 1);
  fread(source, 1, length, f);
  fclose(f);
  source[length] = 0;
  lexer_init(lexer, source);
  lexer->filename = path;
}

static char keywords[][12] = {
    "let",  "if",   "return", "else",   "while", "iter",
    "loop", "this", "enum",   "struct", "union", "@include",
};

Token step_keyword(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]) - 1; i++) {
    char *current = keywords[i];
    if (lexer_matches_string(*lexer, current)) {
      lexer_consume_n_chars(lexer, strlen(current));
      return (Token){TK_KEYWORD,           start, lexer->filename,
                     (int)strlen(current), line,  col};
    }
  }
  char *current = keywords[sizeof(keywords) / sizeof(keywords[0]) - 1];
  if (lexer_matches_string(*lexer, current)) {
    lexer_consume_n_chars(lexer, strlen(current));
    return (Token){TK_INCLUDE,           start, lexer->filename,
                   (int)strlen(current), line,  col};
  }
  return (Token){TK_UNKNOWN, start, lexer->filename, 0, line, col};
}

int is_delim(char c) {
  char delimiters[] = ":.{}()[];,+*-|<>!=&/%";
  for (size_t i = 0; i < sizeof(delimiters) - 1; i++) {
    if (c == delimiters[i])
      return 1;
  }
  return 0;
}

int is_word(char c) {
  char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
  for (size_t i = 0; i < sizeof(chars) - 1; i++) {
    if (c == chars[i])
      return 1;
  }
  return 0;
}

int is_whitespace(char c) { return c && (c == ' ' || c == '\n'); }

int is_digit(char c) { return c >= '0' && c <= '9'; }

int is_ready(Lexer lexer) {
  if (!lexer_peek_char(lexer))
    return 1;
  return (!is_whitespace(lexer_peek_char(lexer))) &&
         (!lexer_matches_string(lexer, "//")) &&
         (!lexer_matches_string(lexer, "/*"));
}

int is_keyword(Lexer lexer) {
  for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
    char *current = keywords[i];
    if (lexer_matches_string(lexer, current))
      return 1;
  }
  return 0;
}

void clean_input(Lexer *lexer) {
  if (lexer_matches_string(*lexer, "//")) {
    char res;
    while ((res = lexer_consume_char(lexer)) != 0 && res != '\n')
      ;
  }
  if (lexer_matches_string(*lexer, "/*")) {
    int depth = 1;
    lexer_consume_char(lexer);
    lexer_consume_char(lexer);
    while (depth > 0 && lexer_peek_char(*lexer)) {
      if (lexer_matches_string(*lexer, "/*")) {
        depth++;
        lexer_consume_char(lexer);
      } else if (lexer_matches_string(*lexer, "*/")) {
        depth--;
        lexer_consume_char(lexer);
      }
      if (!lexer_consume_char(lexer))
        break;
    }
  }
  while (is_whitespace(lexer_peek_char(*lexer)) && lexer_peek_char(*lexer))
    lexer_consume_char(lexer);
}

Token step_identifier(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  char c;
  int length = 0;
  while ((c = lexer_peek_char(*lexer)) && !is_delim(c) && !is_whitespace(c)) {
    lexer_consume_char(lexer);
    length++;
  }
  return (Token){TK_IDENTIFIER, start, lexer->filename, length, line, col};
}

Token step_numlit(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  char c;
  int length = 0;
  int has_point = 0;
  while (is_digit(c = lexer_peek_char(*lexer)) ||
         (!has_point && (has_point |= (c == '.')))) {
    lexer_consume_char(lexer);
    length++;
  }
  if (lexer_peek_char(*lexer) == 'f' && has_point) {
    lexer_consume_char(lexer);
    length++;
  }
  return (Token){TK_NUMLIT, start, lexer->filename, length, line, col};
}

Token step_strlit(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  lexer_consume_char(lexer);
  int length = 1;
  while (lexer_peek_char(*lexer) && !lexer_matches_string(*lexer, "\"")) {
    lexer_consume_char(lexer);
    length++;
  }
  lexer_consume_char(lexer);
  length++;
  return (Token){TK_STRLIT, start, lexer->filename, length, line, col};
}

Token step_charlit(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  lexer_consume_char(lexer);
  int length = 1;
  while (lexer_peek_char(*lexer) && !lexer_matches_string(*lexer, "\'")) {
    lexer_consume_char(lexer);
    length++;
  }
  lexer_consume_char(lexer);
  length++;
  return (Token){TK_CHARLIT, start, lexer->filename, length, line, col};
}

#define OPCOUNT 29
static char delimiters[OPCOUNT][4] = {
    "->>", "->", "=>", "==", "!=", "&&", "||", ">=", "<=", "<",
    ">",   "+",  "-",  "/",  "*",  "%",  "!",  "(",  ")",  "[",
    "]",   "{",  "}",  ";",  ",",  ".",  ":",  "&",  "|",
};

static TokenKind dels_kinds[] = {
    TK_SMALLARRLARGE, TK_SMALLARR,  TK_BIGARR,     TK_EQ,
    TK_DIFF,          TK_AND,       TK_OR,         TK_GRTR_EQ,
    TK_LSSR_EQ,       TK_LSSR,      TK_GRTR,       TK_PLUS,
    TK_MINUS,         TK_DIV,       TK_MULT,       TK_MODULO,
    TK_NOT,           TK_OPENPAREN, TK_CLOSEPAREN, TK_OPENBRACKET,
    TK_CLOSEBRACKET,  TK_OPENBRACE, TK_CLOSEBRACE, TK_SEMICOLON,
    TK_COMMA,         TK_DOT,       TK_COLON,      TK_LOG_AND,
    TK_LOG_OR};

Token step_delim(Lexer *lexer) {
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  for (size_t i = 0; i < OPCOUNT; i++) {
    if (lexer_matches_string(*lexer, delimiters[i])) {
      lexer_consume_n_chars(lexer, strlen(delimiters[i]));
      return (Token){
          dels_kinds[i], start, lexer->filename, (int)strlen(delimiters[i]),
          line,          col,
      };
    }
  }
  return (Token){TK_UNKNOWN, start, lexer->filename, 0, line, col};
}

Token lexer_next_token(Lexer *lexer) {
  // handles nested multiline comments now
  char c;
  const char *start = lexer->current;
  int line = lexer->line;
  int col = lexer->col;
  if (lexer_peek_char(*lexer)) {

    while (!is_ready(*lexer))
      clean_input(lexer);
    if (is_keyword(*lexer))
      return step_keyword(lexer);
    else if (is_word(c = lexer_peek_char(*lexer))) {
      return step_identifier(lexer);
    } else if (is_digit(lexer_peek_char(*lexer))) {
      return step_numlit(lexer);
    } else if (lexer_peek_char(*lexer) == '\"') {
      return step_strlit(lexer);
    } else if (lexer_peek_char(*lexer) == '\'') {
      return step_charlit(lexer);
    } else if (lexer_peek_char(*lexer) == '\'') {
      return step_charlit(lexer);
    } else if (is_delim(lexer_peek_char(*lexer))) {
      return step_delim(lexer);
    }
  }
  return (Token){TK_UNKNOWN, start, lexer->filename, 0, line, col};
}

char paths[1024][PATH_MAX] = {0};
const void *malloced_ptrs[PATH_MAX] = {0};
int n_of_malloced = 0;
int n_of_paths = 0;
int res_index = 0;

void free_malloced_ptrs(void) {
  for (int i = 0; i < n_of_malloced; i++) {
    free((void *)malloced_ptrs[i]);
  }
}

void tokenize_aux(Lexer *lexer, Token **res, int *capacity, int *length) {
  Token to_append = lexer_next_token(lexer);
  while (to_append.type != TK_UNKNOWN) {

    if (to_append.type == TK_INCLUDE) {
      Token file = lexer_next_token(lexer);
      if (file.type != TK_STRLIT) {
        assert(false &&
               "TODO: error reporting, no strlit filename after include "
               "directive !");
      }
      char current[PATH_MAX] = {0};
      char filepath[PATH_MAX] = {0};
      char absolute[PATH_MAX] = {0};
      if (lexer->filename == NULL) {
        getcwd(filepath, PATH_MAX);
      } else {
        strcpy(current, lexer->filename);
        strcpy(filepath, dirname(current));
      }
      strcat(filepath, "/");
      memcpy(filepath + strlen(filepath), file.start + 1, file.length - 2);
      if (realpath(filepath, absolute) == NULL) {
        assert(false && "TODO: error reporting, name too long");
      }
      bool path_found = false;
      for (int i = 0; i < n_of_paths; i++) {
        if (strcmp(paths[i], absolute) == 0) {
          path_found = true;
          break;
        }
      }
      if (!path_found) {
        strcpy(paths[n_of_paths++], absolute);
        Lexer new_l;
        lexer_iniTK_from_file(&new_l, absolute);
        malloced_ptrs[n_of_malloced++] = new_l.start;
        tokenize_aux(&new_l, res, capacity, length);
      }

    } else {
      if (*capacity == *length) {
        *capacity *= 2;
        Token *new_res = (Token *)realloc(*res, *capacity * sizeof(Token));
        *res = new_res;
      }
      (*res)[(*length)++] = to_append;
    }
    to_append = lexer_next_token(lexer);
  }
}

Token *tokenize(const char *filename, int *length) {
  Lexer lexer;
  lexer_iniTK_from_file(&lexer, filename);
  char absolute[PATH_MAX] = {0};
  realpath(filename, absolute);
  malloced_ptrs[n_of_malloced++] = lexer.start;
  Token *res = (Token *)malloc(lexer.length * sizeof(Token));
  res_index = n_of_malloced;
  malloced_ptrs[n_of_malloced++] = NULL;
  strcpy(paths[n_of_paths++], absolute);

  int l = 0;
  int c = lexer.length;
  tokenize_aux(&lexer, &res, &c, &l);
  res = (Token *)realloc(res, sizeof(Token) * l);
  malloced_ptrs[res_index] = res;
  *length = l;
  return res;
}

void debug_prinTK_includes(void) {
  for (int i = 0; i < n_of_paths; i++) {
    printf("%s\n", paths[i]);
  }
}

void free_tokenized(void) { free_malloced_ptrs(); }

#endif // UNILANG_LEXER_IMPL

#endif // UNILANG_LEXER_H
