//---------------------------------------------------------------------------
// LM Studio LAN Client - Base64 Encoding/Decoding Utility
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef Base64H
#define Base64H

#include <Classes.hpp>

class TBase64
{
private:
    static const char Base64Chars[];
    static const char Base64Pad = '=';
    
    static int CharToIndex(char c);
    
public:
    static AnsiString Encode(TStream* stream);
    static AnsiString Encode(const AnsiString& data);
    static TMemoryStream* Decode(const AnsiString& base64);
    static AnsiString DecodeToString(const AnsiString& base64);
};

#endif
