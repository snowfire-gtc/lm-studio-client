//---------------------------------------------------------------------------
// LM Studio LAN Client - Syntax Highlighting Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SyntaxHighlight.h"
#include "Utils.h"

//---------------------------------------------------------------------------
__fastcall TSyntaxHighlighter::TSyntaxHighlighter()
{
    for (int i = 0; i < 8; i++)
        KeywordLists[i] = new TStringList();
    
    InitCppRules();
    InitPythonRules();
    InitSQLRules();
    InitJavaScriptRules();
    InitCSHRules();
    InitJavaRules();
    InitHTMLRules();
}
//---------------------------------------------------------------------------
__fastcall TSyntaxHighlighter::~TSyntaxHighlighter()
{
    for (int i = 0; i < 8; i++)
        delete KeywordLists[i];
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitCppRules()
{
    // Keywords (index 0)
    KeywordLists[0]->Add("int|void|class|return|if|else|for|while|do|switch|case|break|continue");
    KeywordLists[0]->Add("public|private|protected|virtual|static|const|extern|inline");
    KeywordLists[0]->Add("new|delete|this|try|catch|throw|namespace|using|template|typename");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitPythonRules()
{
    // Keywords (index 1)
    KeywordLists[1]->Add("def|class|return|if|elif|else|for|while|try|except|finally");
    KeywordLists[1]->Add("import|from|as|with|lambda|yield|raise|pass|break|continue");
    KeywordLists[1]->Add("True|False|None|and|or|not|in|is");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitSQLRules()
{
    // Keywords (index 2)
    KeywordLists[2]->Add("SELECT|FROM|WHERE|JOIN|ON|INSERT|INTO|VALUES|UPDATE|DELETE");
    KeywordLists[2]->Add("CREATE|TABLE|ALTER|DROP|INDEX|VIEW|TRIGGER|PROCEDURE");
    KeywordLists[2]->Add("AND|OR|NOT|IN|BETWEEN|LIKE|IS|NULL|ORDER|BY|GROUP|HAVING");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitJavaScriptRules()
{
    // Keywords (index 3)
    KeywordLists[3]->Add("function|var|let|const|return|if|else|for|while|do|switch|case|break|continue");
    KeywordLists[3]->Add("class|extends|new|this|try|catch|finally|throw|import|export|default");
    KeywordLists[3]->Add("true|false|null|undefined|typeof|instanceof|in|of|async|await");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitCSHRules()
{
    // Keywords (index 4)
    KeywordLists[4]->Add("class|interface|struct|enum|namespace|using|public|private|protected");
    KeywordLists[4]->Add("static|virtual|override|abstract|sealed|partial|async|await");
    KeywordLists[4]->Add("if|else|for|foreach|while|do|switch|case|break|continue|return");
    KeywordLists[4]->Add("try|catch|finally|throw|new|this|base|typeof|is|as");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitJavaRules()
{
    // Keywords (index 5)
    KeywordLists[5]->Add("class|interface|extends|implements|package|import");
    KeywordLists[5]->Add("public|private|protected|static|final|abstract|native|transient|volatile");
    KeywordLists[5]->Add("if|else|for|while|do|switch|case|break|continue|return");
    KeywordLists[5]->Add("try|catch|finally|throw|throws|new|this|super|instanceof");
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::InitHTMLRules()
{
    // Keywords (index 6) - HTML tags treated as keywords
    KeywordLists[6]->Add("html|head|body|div|span|p|a|img|table|tr|td|th|ul|ol|li");
    KeywordLists[6]->Add("form|input|button|select|option|textarea|label|fieldset|legend");
    KeywordLists[6]->Add("script|style|link|meta|title|h1|h2|h3|h4|h5|h6");
}
//---------------------------------------------------------------------------
TColor TSyntaxHighlighter::GetColorForToken(TTokenType type, const AnsiString& language)
{
    switch (type)
    {
        case ttKeyword: return clBlue;
        case ttComment: return clGreen;
        case ttString: return clRed;
        case ttNumber: return clMaroon;
        case ttPreprocessor: return clNavy;
        case ttOperator: return clGray;
        default: return clBlack;
    }
}
//---------------------------------------------------------------------------
TList* TSyntaxHighlighter::TokenizeLine(const AnsiString& line, const AnsiString& language)
{
    TList* tokens = new TList();
    int langIndex = 0; // Default to C++
    
    if (language == "python" || language == "py") langIndex = 1;
    else if (language == "sql") langIndex = 2;
    
    int pos = 1;
    while (pos <= line.Length())
    {
        TToken* token = new TToken();
        
        // Skip whitespace
        if (line[pos] <= ' ')
        {
            int start = pos;
            while (pos <= line.Length() && line[pos] <= ' ') pos++;
            if (pos > start)
            {
                token->text = line.SubString(start, pos - start);
                token->type = ttPlainText;
                tokens->Add(token);
                continue;
            }
        }
        
        // Check for comments
        if (pos < line.Length() && line.SubString(pos, 2) == "//")
        {
            token->text = line.SubString(pos, line.Length() - pos + 1);
            token->type = ttComment;
            token->color = clGreen;
            token->style = fsItalic;
            tokens->Add(token);
            break;
        }
        
        // Check for strings
        if (line[pos] == '"')
        {
            int start = pos;
            pos++;
            while (pos <= line.Length() && line[pos] != '"')
            {
                if (line[pos] == '\\' && pos < line.Length()) pos++;
                pos++;
            }
            if (pos <= line.Length()) pos++;
            token->text = line.SubString(start, pos - start);
            token->type = ttString;
            token->color = clRed;
            tokens->Add(token);
            continue;
        }
        
        // Check for numbers
        if (isdigit(line[pos]))
        {
            int start = pos;
            while (pos <= line.Length() && (isdigit(line[pos]) || line[pos] == '.')) pos++;
            token->text = line.SubString(start, pos - start);
            token->type = ttNumber;
            token->color = clMaroon;
            tokens->Add(token);
            continue;
        }
        
        // Check for identifiers/keywords
        if (isalpha(line[pos]) || line[pos] == '_')
        {
            int start = pos;
            while (pos <= line.Length() && (isalnum(line[pos]) || line[pos] == '_')) pos++;
            token->text = line.SubString(start, pos - start);
            
            // Check if keyword
            bool isKeyword = false;
            for (int i = 0; i < KeywordLists[langIndex]->Count; i++)
            {
                if (KeywordLists[langIndex]->Strings[i].Pos("|" + token->text + "|") > 0 ||
                    KeywordLists[langIndex]->Strings[i] == "|" + token->text ||
                    token->text + "|" == KeywordLists[langIndex]->Strings[i].SubString(
                        KeywordLists[langIndex]->Strings[i].Length() - token->text.Length(), 
                        token->text.Length() + 1))
                {
                    isKeyword = true;
                    break;
                }
            }
            
            if (isKeyword)
            {
                token->type = ttKeyword;
                token->color = clBlue;
                token->style = fsBold;
            }
            else
            {
                token->type = ttIdentifier;
            }
            
            tokens->Add(token);
            continue;
        }
        
        // Operators and other characters
        token->text = line[pos];
        token->type = ttOperator;
        token->color = clGray;
        tokens->Add(token);
        pos++;
    }
    
    return tokens;
}
//---------------------------------------------------------------------------
void TSyntaxHighlighter::DrawCodeBlock(TCanvas* canvas, 
                                       const AnsiString& code,
                                       const AnsiString& language,
                                       TRect rect)
{
    TStringList* lines = new TStringList();
    try
    {
        lines->Text = code;
        int y = rect.Top;
        int lineHeight = canvas->TextHeight("Xy");
        
        for (int i = 0; i < lines->Count; i++)
        {
            AnsiString line = lines->Strings[i];
            TList* tokens = TokenizeLine(line, language);
            
            int x = rect.Left + 10; // Padding
            for (int j = 0; j < tokens->Count; j++)
            {
                TToken* tok = (TToken*)tokens->Items[j];
                canvas->Font->Color = tok->color;
                canvas->Font->Style = tok->style;
                canvas->TextOut(x, y, tok->text);
                x += canvas->TextWidth(tok->text);
                delete tok;
            }
            delete tokens;
            y += lineHeight;
        }
    }
    __finally
    {
        delete lines;
    }
}
//---------------------------------------------------------------------------
TList* TSyntaxHighlighter::HighlightCode(const AnsiString& code, const AnsiString& language)
{
    return TokenizeLine(code, language);
}
//---------------------------------------------------------------------------
