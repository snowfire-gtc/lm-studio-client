//---------------------------------------------------------------------------
// LM Studio LAN Client - LaTeX Rendering Engine Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LaTeXEngine.h"
#include "Utils.h"

//---------------------------------------------------------------------------
__fastcall TLaTeXEngine::TLaTeXEngine()
{
    symbolMap = new TStringList();
    
    // Greek letters (lowercase)
    symbolMap->Add("\\alpha|α");
    symbolMap->Add("\\beta|β");
    symbolMap->Add("\\gamma|γ");
    symbolMap->Add("\\delta|δ");
    symbolMap->Add("\\epsilon|ε");
    symbolMap->Add("\\zeta|ζ");
    symbolMap->Add("\\eta|η");
    symbolMap->Add("\\theta|θ");
    symbolMap->Add("\\iota|ι");
    symbolMap->Add("\\kappa|κ");
    symbolMap->Add("\\lambda|λ");
    symbolMap->Add("\\mu|μ");
    symbolMap->Add("\\nu|ν");
    symbolMap->Add("\\xi|ξ");
    symbolMap->Add("\\pi|π");
    symbolMap->Add("\\rho|ρ");
    symbolMap->Add("\\sigma|σ");
    symbolMap->Add("\\tau|τ");
    symbolMap->Add("\\upsilon|υ");
    symbolMap->Add("\\phi|φ");
    symbolMap->Add("\\chi|χ");
    symbolMap->Add("\\psi|ψ");
    symbolMap->Add("\\omega|ω");
    
    // Greek letters (uppercase)
    symbolMap->Add("\\Delta|Δ");
    symbolMap->Add("\\Sigma|Σ");
    symbolMap->Add("\\Omega|Ω");
    symbolMap->Add("\\Pi|Π");
    symbolMap->Add("\\Theta|Θ");
    symbolMap->Add("\\Lambda|Λ");
    
    // Operators and symbols
    symbolMap->Add("\\sum|∑");
    symbolMap->Add("\\prod|∏");
    symbolMap->Add("\\int|∫");
    symbolMap->Add("\\partial|∂");
    symbolMap->Add("\\infty|∞");
    symbolMap->Add("\\sqrt|√");
    symbolMap->Add("\\neq|≠");
    symbolMap->Add("\\leq|≤");
    symbolMap->Add("\\geq|≥");
    symbolMap->Add("\\approx|≈");
    symbolMap->Add("\\equiv|≡");
    symbolMap->Add("\\pm|±");
    symbolMap->Add("\\times|×");
    symbolMap->Add("\\div|÷");
    symbolMap->Add("\\cdot|·");
    symbolMap->Add("\\circ|∘");
    symbolMap->Add("\\star|★");
    
    // Logic and set theory
    symbolMap->Add("\\forall|∀");
    symbolMap->Add("\\exists|∃");
    symbolMap->Add("\\nexists|∄");
    symbolMap->Add("\\emptyset|∅");
    symbolMap->Add("\\in|∈");
    symbolMap->Add("\\notin|∉");
    symbolMap->Add("\\subset|⊂");
    symbolMap->Add("\\supset|⊃");
    symbolMap->Add("\\subseteq|⊆");
    symbolMap->Add("\\supseteq|⊇");
    symbolMap->Add("\\cup|∪");
    symbolMap->Add("\\cap|∩");
    symbolMap->Add("\\neg|¬");
    symbolMap->Add("\\wedge|∧");
    symbolMap->Add("\\vee|∨");
    
    // Arrows
    symbolMap->Add("\\rightarrow|→");
    symbolMap->Add("\\leftarrow|←");
    symbolMap->Add("\\leftrightarrow|↔");
    symbolMap->Add("\\Rightarrow|⇒");
    symbolMap->Add("\\Leftarrow|⇐");
    symbolMap->Add("\\Leftrightarrow|⇔");
    
    // Miscellaneous
    symbolMap->Add("\\hbar|ℏ");
    symbolMap->Add("\\imath|ı");
    symbolMap->Add("\\jmath|ȷ");
    symbolMap->Add("\\ell|ℓ");
    symbolMap->Add("\\wp|℘");
    symbolMap->Add("\\Re|ℜ");
    symbolMap->Add("\\Im|ℑ");
    symbolMap->Add("\\angle|∠");
    symbolMap->Add("\\perp|⊥");
    symbolMap->Add("\\parallel|∥");
}
//---------------------------------------------------------------------------
__fastcall TLaTeXEngine::~TLaTeXEngine()
{
    delete symbolMap;
}
//---------------------------------------------------------------------------
AnsiString TLaTeXEngine::SubstituteSymbols(const AnsiString& latex)
{
    AnsiString result = latex;
    
    for (int i = 0; i < symbolMap->Count; i++)
    {
        int pipePos = symbolMap->Strings[i].Pos("|");
        if (pipePos > 0)
        {
            AnsiString latexCmd = symbolMap->Strings[i].SubString(1, pipePos - 1);
            AnsiString unicodeChar = symbolMap->Strings[i].SubString(pipePos + 1, 
                                   symbolMap->Strings[i].Length() - pipePos);
            
            int pos = 1;
            while ((pos = result.Pos(latexCmd)) > 0)
            {
                result = result.SubString(1, pos - 1) + unicodeChar +
                        result.SubString(pos + latexCmd.Length(), result.Length());
            }
        }
    }
    
    return result;
}
//---------------------------------------------------------------------------
void TLaTeXEngine::RenderFraction(TCanvas* canvas, const AnsiString& num, 
                                  const AnsiString& den, int x, int y)
{
    // Simple fraction rendering: numerator / denominator
    int numWidth = canvas->TextWidth(num);
    int denWidth = canvas->TextWidth(den);
    int maxWidth = (numWidth > denWidth ? numWidth : denWidth);
    
    // Draw numerator
    canvas->TextOut(x + (maxWidth - numWidth) / 2, y, num);
    
    // Draw fraction bar
    int lineHeight = canvas->Pen->Width;
    canvas->Line(x, y + 12, x + maxWidth, y + 12);
    
    // Draw denominator
    canvas->TextOut(x + (maxWidth - denWidth) / 2, y + 15, den);
}
//---------------------------------------------------------------------------
void TLaTeXEngine::RenderInline(TCanvas* canvas, const AnsiString& latex, 
                                int& x, int y, int lineHeight)
{
    // Remove $ delimiters if present
    AnsiString content = latex;
    if (content.Pos("$") == 1)
        content = content.SubString(2, content.Length() - 2);
    
    // Handle fractions specially
    int fracPos = content.Pos("\\frac{");
    if (fracPos > 0)
    {
        // Extract numerator and denominator
        int startBrace = fracPos + 5;
        int endBrace = startBrace;
        int braceCount = 0;
        
        for (int i = startBrace; i <= content.Length(); i++)
        {
            if (content[i] == '{') braceCount++;
            else if (content[i] == '}') braceCount--;
            
            if (braceCount == 0)
            {
                endBrace = i;
                break;
            }
        }
        
        AnsiString num = content.SubString(startBrace + 1, endBrace - startBrace - 1);
        
        // Find denominator
        int denStart = endBrace + 2; // Skip }{
        int denEnd = denStart;
        braceCount = 0;
        
        for (int i = denStart; i <= content.Length(); i++)
        {
            if (content[i] == '{') braceCount++;
            else if (content[i] == '}') braceCount--;
            
            if (braceCount == 0)
            {
                denEnd = i;
                break;
            }
        }
        
        AnsiString den = content.SubString(denStart + 1, denEnd - denStart - 1);
        
        RenderFraction(canvas, SubstituteSymbols(num), SubstituteSymbols(den), x, y);
        int fracWidth = (canvas->TextWidth(num) > canvas->TextWidth(den) ? 
                         canvas->TextWidth(num) : canvas->TextWidth(den));
        x += fracWidth + 10;
        return;
    }
    
    // Simple substitution for other expressions
    AnsiString text = SubstituteSymbols(content);
    canvas->TextOut(x, y, text);
    x += canvas->TextWidth(text);
}
//---------------------------------------------------------------------------
void TLaTeXEngine::RenderDisplay(TCanvas* canvas, const AnsiString& latex, 
                                 int& x, int& y)
{
    // Remove $$ delimiters if present
    AnsiString content = latex;
    if (content.Pos("$$") == 1)
        content = content.SubString(3, content.Length() - 4);
    
    // Center the equation
    AnsiString text = SubstituteSymbols(content);
    int textWidth = canvas->TextWidth(text);
    int centerX = x - textWidth / 2;
    
    canvas->TextOut(centerX, y, text);
    y += canvas->TextHeight("Xy") * 2; // Extra spacing for display math
}
//---------------------------------------------------------------------------
