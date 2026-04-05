//---------------------------------------------------------------------------
// LM Studio LAN Client - Document Processor
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef DocProcessorH
#define DocProcessorH

#include <Classes.hpp>
#include <ComCtrls.hpp>

class TDocProcessor
{
private:
    int MaxTokens;
    
    AnsiString ExtractPDFText(const AnsiString& filePath);
    
public:
    __fastcall TDocProcessor();
    
    AnsiString ExtractText(const AnsiString& filePath);
    void SetMaxTokens(int tokens) { MaxTokens = tokens; }
};

#endif
