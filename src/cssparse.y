/*
 * cssparse.c --
 *
 *     This file contains a lemon parser syntax for CSS stylesheets as parsed
 *     by Tkhtml.
 *
 *----------------------------------------------------------------------------
 * Copyright (c) 2005 Eolas Technologies Inc.
 * All rights reserved.
 *
 * This Open Source project was made possible through the financial support
 * of Eolas Technologies Inc.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <ORGANIZATION> nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

%name tkhtmlCssParser
%extra_argument {CssParse *pParse}
%include {
#include "cssInt.h"
#include <string.h>
#include <ctype.h>
}

/* Token prefix 'CT' stands for Css Token. */
%token_prefix CT_
%token_type {CssToken}

/* Need this value for a trick in the tokenizer used to parse CT_FUNCTION. */
%nonassoc RRP. 

%syntax_error {
    pParse->pStyle->nSyntaxErr++;
    pParse->isIgnore = 0;
}

/* Style sheet consists of a header followed by a body. */
stylesheet ::= ss_header ss_body.

/* Optional whitespace. */
ws ::= .
ws ::= SPACE ws.

/*********************************************************************
** Style sheet header. Contains @charset and @import directives. Both
** of these are ignored for now.
*/
ss_header ::= ws charset_opt imports_opt.

charset_opt ::= CHARSET_SYM ws STRING ws SEMICOLON ws.
charset_opt ::= .

imports_opt ::= imports_opt IMPORT_SYM ws term(X) medium_list_opt SEMICOLON ws.
{
    HtmlCssImport(pParse, &X);
}
imports_opt ::= .

medium_list_opt ::= medium_list.
medium_list_opt ::= .

/*********************************************************************
** Style sheet body. A list of style sheet body items.
*/
/*
ss_body ::= .
ss_body ::= ss_body_item ws ss_body.
*/

ss_body ::= ss_body_item.
ss_body ::= ss_body ws ss_body_item.

ss_body_item ::= media.
ss_body_item ::= ruleset.
ss_body_item ::= font_face. 

/*********************************************************************
** @media {...} block.
*/
media ::= MEDIA_SYM ws medium_list LP ws ruleset_list RP. {
    pParse->isIgnore = 0;
}

%type medium_list {int}
%type medium_list_item {int}

medium_list_item(A) ::= IDENT(X). {
    if (
        (X.n == 3 && 0 == strncasecmp(X.z, "all", 3)) ||
        (X.n == 6 && 0 == strncasecmp(X.z, "screen", 6))
    ) {
        A = 0;
    } else {
        A = 1;
    }
}

medium_list(A) ::= medium_list_item(X) ws. {
    A = X;
    pParse->isIgnore = A;
}

medium_list(A) ::= medium_list_item(X) ws COMMA ws medium_list(Y). {
    A = (X && Y) ? 1 : 0;
    pParse->isIgnore = A;
}

/*********************************************************************
** @page {...} block. 
*/
page ::= page_sym ws pseudo_opt LP ws declaration_list RP. {
  pParse->isIgnore = 0;
}

pseudo_opt ::= COLON IDENT ws.
pseudo_opt ::= .

page_sym ::= PAGE_SYM. {
  pParse->isIgnore = 1;
}

/*********************************************************************
** @font_face {...} block.
*/
font_face ::= FONT_SYM LP declaration_list RP.

/*********************************************************************
** Style sheet rules. e.g. "<selector> { <properties> }"
*/
ruleset_list ::= ruleset ws.
ruleset_list ::= ruleset ws ruleset_list.

ruleset ::= selector_list LP ws declaration_list semicolon_opt RP. {
    HtmlCssRule(pParse, 1);
}
ruleset ::= page.

selector_list ::= selector.
selector_list ::= selector_list comma ws selector.

comma ::= COMMA. {
    HtmlCssSelectorComma(pParse);
}

declaration_list ::= .
declaration_list ::= declaration.
declaration_list ::= declaration_list SEMICOLON ws declaration.

semicolon_opt ::= SEMICOLON ws.
semicolon_opt ::= .

declaration ::= IDENT(X) ws COLON ws expr(E) prio(I). {
    HtmlCssDeclaration(pParse, &X, &E, I);
}

%type prio {int}
prio(X) ::= IMPORTANT_SYM ws. {X = (pParse->pStyleId) ? 1 : 0;}
prio(X) ::= .                 {X = 0;}

/*********************************************************************
** Selector syntax. This is in a section of it's own because it's
** complicated.
*/
selector ::= simple_selector ws.
selector ::= simple_selector combinator(X) selector. 

%type combinator {int}
combinator ::= ws PLUS ws. {
    HtmlCssSelector(pParse, CSS_SELECTORCHAIN_ADJACENT, 0, 0);
}
combinator ::= ws GT ws. {
    HtmlCssSelector(pParse, CSS_SELECTORCHAIN_CHILD, 0, 0);
}
combinator ::= SPACE ws. {
    HtmlCssSelector(pParse, CSS_SELECTORCHAIN_DESCENDANT, 0, 0);
}

simple_selector ::= IDENT(X) simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_TYPE, 0, &X);
}
simple_selector ::= STAR simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_UNIVERSAL, 0, 0);
}
simple_selector ::= simple_selector_tail.

simple_selector_tail_opt ::= simple_selector_tail.
simple_selector_tail_opt ::= .

simple_selector_tail ::= HASH IDENT(X) simple_selector_tail_opt. {
    CssToken id = {"id", 2};
    HtmlCssSelector(pParse, CSS_SELECTOR_ATTRVALUE, &id, &X);
}
simple_selector_tail ::= DOT IDENT(X) simple_selector_tail_opt. {
    /* A CSS class selector may not begin with a digit. Presumably this is
     * because they expect to use this syntax for something else in a
     * future version. For now, just insert a "never-match" condition into
     * the rule to prevent it from having any affect.
     */
    if (X.n > 0 && !isdigit((int)(*X.z))) {
        CssToken cls = {"class", 5};
        HtmlCssSelector(pParse, CSS_SELECTOR_ATTRLISTVALUE, &cls, &X);
    } else {
        HtmlCssSelector(pParse, CSS_SELECTOR_NEVERMATCH, 0, 0);
    }
}
simple_selector_tail ::= LSP IDENT(X) RSP simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_ATTR, &X, 0);
}
simple_selector_tail ::= 
    LSP IDENT(X) EQUALS STRING(Y) RSP simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_ATTRVALUE, &X, &Y);
}
simple_selector_tail ::= 
    LSP IDENT(X) TILDE EQUALS STRING(Y) RSP simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_ATTRLISTVALUE, &X, &Y);
}
simple_selector_tail ::= 
    LSP IDENT(X) PIPE EQUALS STRING(Y) RSP simple_selector_tail_opt. {
    HtmlCssSelector(pParse, CSS_SELECTOR_ATTRHYPHEN, &X, &Y);
}

/* Todo: Deal with pseudo selectors. This rule makes the parser ignore them. */
simple_selector_tail ::= COLON IDENT(X) simple_selector_tail_opt. {
    HtmlCssSelector(pParse, HtmlCssPseudo(&X), 0, 0);
}

/*********************************************************************
** Expression syntax. This is very simple, it may need to be extended
** so that the structure of the expression is preserved. At present,
** all stylesheet expressions are stored as strings.
*/
expr(A) ::= term(X) ws.               { A = X; }
expr(A) ::= term(X) operator expr(Y). { A.z = X.z; A.n = (Y.z+Y.n - X.z); }

operator ::= ws COMMA ws.
operator ::= ws SLASH ws.
operator ::= SPACE ws.

term(A) ::= IDENT(X). { A = X; }
term(A) ::= DOT(X) IDENT(Y). { A.z = X.z; A.n = (Y.z+Y.n - X.z); }
term(A) ::= IDENT(X) DOT IDENT(Y). { A.z = X.z; A.n = (Y.z+Y.n - X.z); }
term(A) ::= STRING(X). { A = X; }
term(A) ::= FUNCTION(X). { A = X; }
term(A) ::= HASH(X) IDENT(Y). { A.z = X.z; A.n = (Y.z+Y.n - X.z); }
term(A) ::= PLUS(X) IDENT(Y). { A.z = X.z; A.n = (Y.z+Y.n - X.z); }

