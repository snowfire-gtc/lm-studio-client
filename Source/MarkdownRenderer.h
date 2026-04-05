//---------------------------------------------------------------------------
// LM Studio LAN Client - Markdown Parser and Renderer
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef MarkdownRendererH
#define MarkdownRendererH

#include <Classes.hpp>
#include <Graphics.hpp>
#include "SyntaxHighlight.h"
#include "LaTeXEngine.h"

enum TMarkdownState
{
    msText,
    msBold,
    msItalic,
    msCodeInline,
    msCodeBlockStart,
    msCodeBlock,
    msMathInline,
    msMathBlock,
    msLink
};

enum TTokenType
{
    mtText,
    mtBold,
    mtItalic,
    mtCodeInline,
    mtCodeBlock,
    mtMathInline,
    mtMathDisplay,
    mtHeading,
    mtList,
    mtLink
};

struct TMarkdownToken
{
    TTokenType type;
    AnsiString content;
    AnsiString language; // For code blocks
    TRect renderRect;    // For click detection (code block export)
    
    __fastcall TMarkdownToken() : type(mtText) {}
};

class TMarkdownRenderer
{
private:
    TList* tokens;
    TMarkdownState currentState;
    TSyntaxHighlighter* syntaxHighlighter;
    TLaTeXEngine* latexEngine;
    
    void Parse(const AnsiString& input);
    TMarkdownToken* CreateToken(TTokenType type, const AnsiString& content);
    
public:
    __fastcall TMarkdownRenderer();
    __fastcall ~TMarkdownRenderer();
    
    void FeedChunk(const AnsiString& chunk); // For streaming
    void Render(TCanvas* canvas, int& yPos, int width);
    void Clear();
    
    // Code block export support
    TList* GetCodeBlocks();
};

#endif
