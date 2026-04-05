//---------------------------------------------------------------------------
// LM Studio LAN Client - Streaming JSON Parser
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef StreamParserH
#define StreamParserH

#include <Classes.hpp>

typedef void (__fastcall *TTokenCallback)(const AnsiString& token);

class TStreamParser
{
private:
    AnsiString buffer;
    TTokenCallback OnTokenReceived;
    
    AnsiString UnescapeJSON(const AnsiString& escaped);
    
public:
    __fastcall TStreamParser();
    
    void ParseChunk(const AnsiString& json);
    void Reset();
    
    __property TTokenCallback OnToken = { read = OnTokenReceived, write = OnTokenReceived };
};

#endif
