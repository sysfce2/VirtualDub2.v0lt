// Ami - Language resource compiler for VirtualDub
//
// Copyright (C) 2013 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_AMI_LEXER_H
#define f_VD2_AMI_LEXER_H

#include <string>

enum {
	kTokenEOF			= -1,
	kTokenNull			= 0,
	kTokenInteger		= 0x10000,
	kTokenString,
	kTokenIdentifier,

	kTokenPlusPlus,
	kTokenMinusMinus,

	kTokenEQ,
	kTokenNE,
	kTokenLE,
	kTokenGE,
	kTokenLogicalAnd,
	kTokenLogicalOr,

	kTokenInclude,
	kTokenEnum,
	kTokenLet,
	kTokenDeclare,
	kTokenUsing,
	kTokenNow,

	kTokenLabel,
	kTokenEdit,
	kTokenEditInt,
	kTokenButton,
	kTokenCheckBox,
	kTokenListBox,
	kTokenComboBox,
	kTokenListView,
	kTokenTrackbar,
	kTokenFileControl,
	kTokenSet,
	kTokenPageSet,
	kTokenGrid,
	kTokenOption,
	kTokenGroup,
	kTokenSplitter,
	kTokenTextEdit,
	kTokenTextArea,
	kTokenTrackBar,
	kTokenHotkey,
	kTokenCustomWindow,

	kTokenListItem,
	kTokenPage,
	kTokenColumn,
	kTokenRow,
	kTokenNextRow,

	kTokenStringSet,
	kTokenMessage,
	kTokenOverride,
	kTokenDialog,
	kTokenTemplate,

	// properties
	kTokenMarginL,
	kTokenMarginT,
	kTokenMarginR,
	kTokenMarginB,
	kTokenPadL,
	kTokenPadT,
	kTokenPadR,
	kTokenPadB,
	kTokenMinW,
	kTokenMinH,
	kTokenMaxW,
	kTokenMaxH,
	kTokenAlign,
	kTokenVAlign,
	kTokenSpacing,
	kTokenAspect,
	kTokenAffinity,
	kTokenRowSpan,
	kTokenColSpan,

	kTokenVertical,
	kTokenRaised,
	kTokenSunken,
	kTokenChild,
	kTokenMultiline,
	kTokenReadonly,
	kTokenCheckable,
	kTokenNoHeader,
	kTokenDefault,

	kTokenEnable,
	kTokenValue,

	kTokenLeft,
	kTokenCenter,
	kTokenRight,
	kTokenTop,
	kTokenBottom,
	kTokenFill,
	kTokenExpand,
	kTokenLink,
	kTokenAddColumn
};


const std::wstring& lexident();
int lexint();
const wchar_t* lexfilename();
int lexlineno();
void lexopen(const wchar_t* fn);
bool lexisunicode();
void lextestunicode();
void lexinclude(const std::wstring& filename);
wint_t lexrawgetc();
void lexungetc(wint_t c);
wint_t lexgetc();
wint_t lexgetescape();
void lexpush(int token);
int lex();
std::string lextokenname(int token, bool expand = true);

#endif
