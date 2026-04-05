//---------------------------------------------------------------------------
// LM Studio LAN Client - Syntax Highlighting Engine
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef SyntaxHighlightH
#define SyntaxHighlightH

#include <Classes.hpp>
#include <Graphics.hpp>

enum TTokenType
{
    ttKeyword,
    ttComment,
    ttString,
    ttNumber,
    ttPreprocessor,
    ttOperator,
    ttIdentifier,
    ttPlainText
};

struct TToken
{
    AnsiString text;
    TTokenType type;
    TColor color;
    TFontStyle style;
    
    __fastcall TToken() : type(ttPlainText), color(clBlack), style(fsNormal) {}
};

class TSyntaxHighlighter
{
private:
    TStringList* KeywordLists[8]; // One per language
    
    void InitCppRules();
    void InitPythonRules();
    void InitSQLRules();
    void InitJavaScriptRules();
    void InitCSHRules();
    void InitJavaRules();
    void InitHTMLRules();
    
    TList* TokenizeLine(const AnsiString& line, const AnsiString& language);
    TColor GetColorForToken(TTokenType type, const AnsiString& language);
    
public:
    __fastcall TSyntaxHighlighter();
    __fastcall ~TSyntaxHighlighter();
    
    void DrawCodeBlock(TCanvas* canvas, 
                       const AnsiString& code,
                       const AnsiString& language,
                       TRect rect);
    
    TList* HighlightCode(const AnsiString& code, const AnsiString& language);
};

#endif
