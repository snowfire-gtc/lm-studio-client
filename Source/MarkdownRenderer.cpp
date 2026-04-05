//---------------------------------------------------------------------------
// LM Studio LAN Client - Markdown Parser and Renderer Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MarkdownRenderer.h"
#include "Utils.h"

//---------------------------------------------------------------------------
__fastcall TMarkdownRenderer::TMarkdownRenderer()
{
    tokens = new TList();
    currentState = msText;
    syntaxHighlighter = new TSyntaxHighlighter();
    latexEngine = new TLaTeXEngine();
}
//---------------------------------------------------------------------------
__fastcall TMarkdownRenderer::~TMarkdownRenderer()
{
    Clear();
    delete syntaxHighlighter;
    delete latexEngine;
}
//---------------------------------------------------------------------------
void TMarkdownRenderer::Clear()
{
    for (int i = 0; i < tokens->Count; i++)
        delete (TMarkdownToken*)tokens->Items[i];
    tokens->Clear();
    currentState = msText;
}
//---------------------------------------------------------------------------
TMarkdownToken* TMarkdownRenderer::CreateToken(TTokenType type, const AnsiString& content)
{
    TMarkdownToken* token = new TMarkdownToken();
    token->type = type;
    token->content = content;
    return token;
}
//---------------------------------------------------------------------------
void TMarkdownRenderer::FeedChunk(const AnsiString& chunk)
{
    // Accumulate chunks and parse incrementally
    // For simplicity, we'll just append to the last text token or create a new one
    if (tokens->Count > 0)
    {
        TMarkdownToken* last = (TMarkdownToken*)tokens->Items[tokens->Count - 1];
        if (last->type == mtText)
        {
            last->content += chunk;
            return;
        }
    }
    
    TMarkdownToken* token = CreateToken(mtText, chunk);
    tokens->Add(token);
}
//---------------------------------------------------------------------------
void TMarkdownRenderer::Parse(const AnsiString& input)
{
    Clear();
    
    AnsiString buffer = "";
    int pos = 1;
    int len = input.Length();
    
    while (pos <= len)
    {
        // Check for code blocks ```
        if (pos + 2 <= len && input.SubString(pos, 3) == "```")
        {
            if (currentState != msCodeBlock)
            {
                // Start of code block
                pos += 3;
                int langStart = pos;
                while (pos <= len && input[pos] != '\n') pos++;
                AnsiString language = Trim(input.SubString(langStart, pos - langStart));
                
                int codeStart = pos + 1;
                while (pos + 2 <= len && input.SubString(pos, 3) != "```")
                    pos++;
                
                AnsiString code = input.SubString(codeStart, pos - codeStart);
                TMarkdownToken* token = CreateToken(mtCodeBlock, code);
                token->language = language;
                tokens->Add(token);
                
                pos += 3; // Skip closing ```
                continue;
            }
        }
        
        // Check for display math $$
        if (pos + 1 <= len && input.SubString(pos, 2) == "$$")
        {
            pos += 2;
            int mathStart = pos;
            while (pos + 1 <= len && input.SubString(pos, 2) != "$$")
                pos++;
            
            AnsiString math = input.SubString(mathStart, pos - mathStart);
            tokens->Add(CreateToken(mtMathDisplay, math));
            pos += 2;
            continue;
        }
        
        // Check for inline math $
        if (input[pos] == '$' && (pos == 1 || input[pos-1] != '\\'))
        {
            pos++;
            int mathStart = pos;
            while (pos <= len && input[pos] != '$')
                pos++;
            
            AnsiString math = input.SubString(mathStart, pos - mathStart);
            tokens->Add(CreateToken(mtMathInline, math));
            pos++;
            continue;
        }
        
        // Check for bold **
        if (pos + 1 <= len && input.SubString(pos, 2) == "**")
        {
            pos += 2;
            int boldStart = pos;
            while (pos + 1 <= len && input.SubString(pos, 2) != "**")
                pos++;
            
            AnsiString bold = input.SubString(boldStart, pos - boldStart);
            tokens->Add(CreateToken(mtBold, bold));
            pos += 2;
            continue;
        }
        
        // Check for inline code `
        if (input[pos] == '`')
        {
            pos++;
            int codeStart = pos;
            while (pos <= len && input[pos] != '`')
                pos++;
            
            AnsiString code = input.SubString(codeStart, pos - codeStart);
            tokens->Add(CreateToken(mtCodeInline, code));
            pos++;
            continue;
        }
        
        // Check for headings #
        if (input[pos] == '#')
        {
            int level = 0;
            while (pos <= len && input[pos] == '#')
            {
                level++;
                pos++;
            }
            
            if (level <= 6)
            {
                while (pos <= len && input[pos] == ' ') pos++;
                int headingStart = pos;
                while (pos <= len && input[pos] != '\n') pos++;
                
                AnsiString heading = input.SubString(headingStart, pos - headingStart);
                TMarkdownToken* token = CreateToken(mtHeading, heading);
                // Encode level in language field (hack but works)
                token->language = IntToStr(level);
                tokens->Add(token);
                continue;
            }
        }
        
        // Regular text - accumulate until next special character
        int textStart = pos;
        while (pos <= len && 
               input[pos] != '`' && 
               input[pos] != '$' &&
               (pos + 1 > len || (input.SubString(pos, 2) != "**" && 
                                  input.SubString(pos, 2) != "``" &&
                                  input.SubString(pos, 2) != "$$")) &&
               (pos + 2 > len || input.SubString(pos, 3) != "```"))
        {
            pos++;
        }
        
        if (pos > textStart)
        {
            AnsiString text = input.SubString(textStart, pos - textStart);
            tokens->Add(CreateToken(mtText, text));
        }
    }
}
//---------------------------------------------------------------------------
void TMarkdownRenderer::Render(TCanvas* canvas, int& yPos, int width)
{
    int lineHeight = canvas->TextHeight("Xy");
    int x = 10;
    
    for (int i = 0; i < tokens->Count; i++)
    {
        TMarkdownToken* token = (TMarkdownToken*)tokens->Items[i];
        
        switch (token->type)
        {
            case mtText:
            {
                // Word wrap for long text
                AnsiString text = token->content;
                int wordPos = 1;
                int lineX = x;
                
                while (wordPos <= text.Length())
                {
                    int spacePos = text.Pos(" ", wordPos);
                    if (spacePos == 0) spacePos = text.Length() + 1;
                    
                    AnsiString word = text.SubString(wordPos, spacePos - wordPos);
                    int wordWidth = canvas->TextWidth(word);
                    
                    if (lineX + wordWidth > x + width)
                    {
                        lineX = x;
                        yPos += lineHeight;
                    }
                    
                    canvas->TextOut(lineX, yPos, word);
                    lineX += wordWidth;
                    wordPos = spacePos + 1;
                }
                yPos += lineHeight;
                break;
            }
            
            case mtBold:
            {
                canvas->Font->Style = fsBold;
                canvas->TextOut(x, yPos, token->content);
                canvas->Font->Style = fsNormal;
                x += canvas->TextWidth(token->content);
                break;
            }
            
            case mtItalic:
            {
                canvas->Font->Style = fsItalic;
                canvas->TextOut(x, yPos, token->content);
                canvas->Font->Style = fsNormal;
                x += canvas->TextWidth(token->content);
                break;
            }
            
            case mtCodeInline:
            {
                canvas->Font->Name = "Courier New";
                canvas->Font->Color = clRed;
                canvas->TextOut(x, yPos, token->content);
                canvas->Font->Name = "Arial";
                canvas->Font->Color = clBlack;
                x += canvas->TextWidth(token->content);
                break;
            }
            
            case mtCodeBlock:
            {
                // Draw code block background
                TStringList* lines = new TStringList();
                try
                {
                    lines->Text = token->content;
                    int codeHeight = lines->Count * lineHeight + 20;
                    
                    TRect codeRect = Rect(x - 5, yPos, x + width - 5, yPos + codeHeight);
                    canvas->Brush->Color = clBtnFace;
                    canvas->FillRect(codeRect);
                    canvas->Pen->Color = clGray;
                    canvas->Rectangle(codeRect);
                    
                    // Store rect for export button placement
                    token->renderRect = codeRect;
                    
                    // Syntax highlighted rendering
                    syntaxHighlighter->DrawCodeBlock(canvas, token->content, 
                                                     token->language, 
                                     Rect(x, yPos + 10, x + width - 20, yPos + codeHeight));
                    
                    yPos += codeHeight + 10;
                }
                __finally
                {
                    delete lines;
                }
                break;
            }
            
            case mtMathInline:
            {
                latexEngine->RenderInline(canvas, token->content, x, yPos, lineHeight);
                break;
            }
            
            case mtMathDisplay:
            {
                int centerX = x + width / 2;
                latexEngine->RenderDisplay(canvas, token->content, centerX, yPos);
                break;
            }
            
            case mtHeading:
            {
                int level = StrToIntDef(token->language, 1);
                int fontSize = canvas->Font->Size + (4 - level);
                canvas->Font->Size = fontSize;
                canvas->Font->Style = fsBold;
                canvas->TextOut(x, yPos, token->content);
                canvas->Font->Size = 10;
                canvas->Font->Style = fsNormal;
                yPos += lineHeight + 5;
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
TList* TMarkdownRenderer::GetCodeBlocks()
{
    TList* codeBlocks = new TList();
    
    for (int i = 0; i < tokens->Count; i++)
    {
        TMarkdownToken* token = (TMarkdownToken*)tokens->Items[i];
        if (token->type == mtCodeBlock)
        {
            codeBlocks->Add(token);
        }
    }
    
    return codeBlocks;
}
//---------------------------------------------------------------------------
