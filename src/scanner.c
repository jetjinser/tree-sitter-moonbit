#include "tree_sitter/parser.h"
#include <stdbool.h>
#include <wctype.h>

enum TokenType {
  FLOAT_LITERAL,
  COMMENT,
  DOCSTRING,
  PIPE_OPERATOR,
  DOT_OPERATOR,
  COLON,
  COLON_COLON,
  QUESTION_OPERATOR,
  DERIVE,
  DOT_DOT,
  MULTILINE_STRING_FRAGMENT
};

void *tree_sitter_moonbit_external_scanner_create() { return NULL; }
void tree_sitter_moonbit_external_scanner_destroy(void *p) {}
void tree_sitter_moonbit_external_scanner_reset(void *p) {}
unsigned tree_sitter_moonbit_external_scanner_serialize(void *p, char *buffer) {
  return 0;
}
void tree_sitter_moonbit_external_scanner_deserialize(void *p, const char *b,
                                                      unsigned n) {}

static void advance(TSLexer *lexer) { lexer->advance(lexer, false); }
static void skip(TSLexer *lexer) { lexer->advance(lexer, true); }
static bool is_num_char(int32_t c) { return c == '_' || iswdigit(c); }

bool tree_sitter_moonbit_external_scanner_scan(void *payload, TSLexer *lexer,
                                               const bool *valid_symbols) {
  if (valid_symbols[FLOAT_LITERAL]) {
    lexer->result_symbol = FLOAT_LITERAL;

    while (iswspace(lexer->lookahead)) {
      skip(lexer);
    }

    if (!iswdigit(lexer->lookahead)) {
      return false;
    }

    advance(lexer);

    while (is_num_char(lexer->lookahead)) {
      advance(lexer);
    }

    bool has_fraction = false, has_exponent = false;

    if (lexer->lookahead == '.') {
      has_fraction = true;
      advance(lexer);
      if (iswalpha(lexer->lookahead)) {
        // this is an integer
        return false;
      }

      if (lexer->lookahead == '.') {
        return false;
      }

      while (is_num_char(lexer->lookahead)) {
        advance(lexer);
      }
    }

    lexer->mark_end(lexer);

    if (lexer->lookahead == 'e' || lexer->lookahead == 'E') {
      has_exponent = true;
      advance(lexer);
      if (lexer->lookahead == '+' || lexer->lookahead == '-') {
        advance(lexer);
      }
      if (!is_num_char(lexer->lookahead)) {
        return true;
      }
      advance(lexer);
      while (is_num_char(lexer->lookahead)) {
        advance(lexer);
      }
      lexer->mark_end(lexer);
    }
    if (!has_fraction && !has_fraction) {
      return false;
    }
    return true;
  } else if (
    valid_symbols[COMMENT] ||
    valid_symbols[DOCSTRING] ||
    valid_symbols[PIPE_OPERATOR] ||
    valid_symbols[DOT_OPERATOR] ||
    valid_symbols[COLON] ||
    valid_symbols[COLON_COLON] ||
    valid_symbols[QUESTION_OPERATOR] ||
    valid_symbols[DERIVE] ||
    valid_symbols[DOT_DOT] ||
    valid_symbols[MULTILINE_STRING_FRAGMENT]
  ) {
    while (iswspace(lexer->lookahead)) {
      skip(lexer);
    }
    if (lexer->lookahead == '/') {
      advance(lexer);
      if (lexer->lookahead != '/') {
        return false;
      }
      advance(lexer);
      if (lexer->lookahead == '/') {
        lexer->result_symbol = DOCSTRING;
      } else {
        lexer->result_symbol = COMMENT;
      }
      while (lexer->lookahead != '\n' && lexer->lookahead != '\0') {
        advance(lexer);
      }
      lexer->mark_end(lexer);
      return true;
    } else if (lexer->lookahead == '|') {
      advance(lexer);
      if (lexer->lookahead != '>') {
        return false;
      }
      advance(lexer);
      lexer->result_symbol = PIPE_OPERATOR;
      lexer->mark_end(lexer);
      return true;
    } else if (lexer->lookahead == '.') {
      advance(lexer);
      if (lexer->lookahead != '.') {
        lexer->mark_end(lexer);
        lexer->result_symbol = DOT_OPERATOR;
        return true;
      }
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = DOT_DOT;
      return true;
    } else if (lexer->lookahead == ':') {
      advance(lexer);
      if (lexer->lookahead != ':') {
        lexer->mark_end(lexer);
        lexer->result_symbol = COLON;
        return true;
      }
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = COLON_COLON;
      return true;
    } else if (lexer->lookahead == '?') {
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = QUESTION_OPERATOR;
      return true;
    } else if (lexer->lookahead == 'd') {
      advance(lexer);
      if (lexer->lookahead != 'e') {
        return false;
      }
      advance(lexer);
      if (lexer->lookahead != 'r') {
        return false;
      }
      advance(lexer);
      if (lexer->lookahead != 'i') {
        return false;
      }
      advance(lexer);
      if (lexer->lookahead != 'v') {
        return false;
      }
      advance(lexer);
      if (lexer->lookahead != 'e') {
        return false;
      }
      advance(lexer);
      lexer->result_symbol = DERIVE;
      lexer->mark_end(lexer);
      return true;
    } else if (lexer->lookahead == '#') {
      advance(lexer);
      if (lexer->lookahead != '|') {
        return false;
      }
      advance(lexer);
      while (lexer->lookahead != '\n' && lexer->lookahead != '\0') {
        advance(lexer);
      }
      if (valid_symbols[MULTILINE_STRING_FRAGMENT]) {
        lexer->result_symbol = MULTILINE_STRING_FRAGMENT;
      } else {
        return false;
      }
      lexer->mark_end(lexer);
      return true;
    }
    return false;
  }
  return false;
}
