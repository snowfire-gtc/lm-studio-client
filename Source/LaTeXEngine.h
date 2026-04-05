//---------------------------------------------------------------------------
// LM Studio LAN Client - LaTeX Rendering Engine (GDI+ Unicode Substitution)
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef LaTeXEngineH
#define LaTeXEngineH

#include <Classes.hpp>
#include <Graphics.hpp>

class TLaTeXEngine
{
private:
    TStringList* symbolMap; // LaTeX -> Unicode mapping
    
    AnsiString SubstituteSymbols(const AnsiString& latex);
    void RenderFraction(TCanvas* canvas, const AnsiString& num, 
                        const AnsiString& den, int x, int y);
    
public:
    __fastcall TLaTeXEngine();
    __fastcall ~TLaTeXEngine();
    
    void RenderInline(TCanvas* canvas, const AnsiString& latex, 
                      int& x, int y, int lineHeight);
    void RenderDisplay(TCanvas* canvas, const AnsiString& latex, 
                       int& x, int& y);
};

#endif
