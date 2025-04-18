// Csv Lexical Analyzer
// rrvt 9/1/94
// Copyright Software Design & Engineering, Robert R. Van Tuyl, 2013.  All rights reserved.


#include "pch.h"
#include "CSVLexf.h"
#include "GetPathDlg.h"

//#include "MessageBox.h"


typedef enum {other, quote, comma, sChar, bslsh, eol, cr, delch, eofch} Character_Classes;


// character class table

static Character_Classes character_class_table[] = {
  other, other, other, other, other, other, other, other, // 00 -
  other, sChar,   eol, other, sChar, cr,    other, other, // 08 -
  other, other, other, other, other, other, other, other, // 16 -
  other, other, eofch, other, other, other, other, other, // 24 -
  sChar, sChar, quote, sChar, sChar, sChar, sChar, sChar, // 32 -
  sChar, sChar, sChar, sChar, comma, sChar, sChar, sChar, // 40 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 48 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 56 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 64 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 72 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 80 -
  sChar, sChar, sChar, sChar, bslsh, sChar, sChar, sChar, // 88 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, // 96 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //104 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //112 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, delch, //120 -
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //128
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //Safety
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //
  sChar, sChar, sChar, sChar, sChar, sChar, sChar, sChar, //  -255
  };



CSVLexF::CSVLexF() : getNext(true)
                             {source[0] = &source_line; source[1] = &source_line1;   initialize();}


bool CSVLexF::open(PathDlgDsc& dsc, String& path)
                                     {return getOpenDlg(dsc, path) && fi.open(path, FileIO::Read);}

// open lexical analyser file

bool CSVLexF::initialize() {

  state = begin_tok;

  current_source = 0; pline = source[current_source]; pline->clear();
  token.line_number = line_number = 1;  tokenNo = 0;
  ch = quote_ch = 0;
  accept_two_tokens(); return true;
  }


// accept token by clearing the token code

void CSVLexF::accept_token()      {token.code = NoToken;}

void CSVLexF::accept_two_tokens() {token.code = token1.code = NoToken;}


// get next token when token is empty, values set in globals


CSVtokCode CSVLexF::get_token() {

  if (token.code  != NoToken) return token.code;

  if (token1.code != NoToken) token = token1;
  else next_tok(token);

  next_tok(token1);   return token.code;
  }


void CSVLexF::next_tok(CSVtok& tok) {
Character_Classes ch_class;                 // character class of current character

  tok.clear();  ptok = &tok.name;

  loop {

    nextChar();   ch_class = character_class_table[ch];

//static bool gotIt = false;   if (ch == _T('"')) gotIt = true;
//if (gotIt) {String s;   s.format(_T("%c -- %i"), ch, ch_class);   messageBox(s);}

    switch (state) {

      // Begin Token State, Look for the first character of each token type
      // Ignore spaces, return, line feed (end-of-line)
      // Convert tabs to spaces (the display always moves to specific columns)

      case begin_tok:

        switch (ch_class) {

          case cr:      accept_char(); continue;

          case eol:     state = got_eol;
fin_eol:                ptok = &tok.name; ptok->clear(); tok.code = EolToken; goto fin_op;

          case other:
          case bslsh:
          case sChar:   state = collect_symbol; start_token(tok); break;


          case quote:   state = collect_string; quote_ch = ch;
                        start_token(tok); accept_char(); add_to_line(); continue;

          case comma:   tok.code = CommaToken;
fin_op:                 start_token(tok); move_char(); terminate(tok, source); return;

          case eofch:   goto eof;

          default:      start_token(tok); state = illegal_tok;
          }
        move_char(); continue;


      // Collect string up to a comma or crlf

      case collect_symbol:

        switch (ch_class) {
          case cr   : accept_char();
          case eol  :
          case comma: tok.code = StringToken; terminate(tok, source); state = begin_tok; return;

          default   : break;
          }
        move_char(); continue;


      // Collect String or character literal with hardly any discrimination between the two

      case collect_string:

        switch(ch_class) {
          case bslsh:   state = got_backslash; accept_char(); add_to_line(); continue;

          case quote:   accept_char(); add_to_line();  state = got_quote; continue;
          case cr   :   accept_char(); continue;

          case eofch:   tok.code = StringToken; goto fin_tok;

          default   :   break;
          }
        move_char(); continue;


      // Look for second quote as a method or allowing a quote to be in the String

      case got_quote:
        switch (ch_class) {
          case quote  : state = collect_string; break;
          default     : tok.code = StringToken;
fin_tok:                state = begin_tok; terminate(tok, source); return;
          }
        move_char(); continue;


      case got_backslash:
        switch (ch_class) {
          case cr:      accept_char(); continue;
          case eol:     state = collect_string; continue;
          default:      state = collect_string;
          }
        move_char(); continue;


      case got_eol:

        switch (ch_class) {

          case cr:      accept_char(); continue;

          case eol:     goto fin_eol;

eof:      case eofch:   tok.code = EOFToken;
                        state = end_of_file; terminate(tok, source);
                        return;

          default:      state = begin_tok; continue;
          }

      // End of file has been seen, do nothing except return end of file tokens

      case end_of_file: tok.code = EOFToken; terminate(tok, source); return;


      // Illegal token, gather all illegal characters into one token and return as a group

      case illegal_tok:
        switch (ch_class) {
//        case cr:
          case eol:  case sChar: case quote: case comma: case eofch:
                   tok.code = IllegalToken; goto fin_tok;

          default: break;
          }
        move_char(); continue;

      default:          state = begin_tok; continue;              // This should never happen, but ...
      }
    }
  }




void CSVLexF::terminate(CSVtok& tok, String* source[]) {

  tok.line_number = line_number; tok.tokenNo = tokenNo; tok.psource = source[current_source];

  if (tok.code == EolToken) {
    current_source = (current_source + 1) % 2;   line_number++;   tokenNo = 0;
    pline = source[current_source]; pline->clear();
    }
  }


// Globals

static int error_count;     /* number of syntax errors */


// Functions

void CSVLexF::error(CSVtok& tok, Tchar* stg) {
String name = tok.name;

  if (tok.code == EolToken) name = _T("<eol>");

  errStg.format(_T("       Token #%i: \"%s\" -> Error %i: %s\n"),
                                                              tok.tokenNo, name.str(), error_count, stg);
  }


// display source line associated with current tok.
// Return offset of current tok

String CSVLexF::getSourceLine(CSVtok& tok) {
String* pstg   = tok.psource;
String  f;

  pstg->trim();

  if (pstg->empty()) {return _T("\n");}

  f.format(_T("%3i: %s\n"), tok.line_number, pstg->str()); return f;
  }




