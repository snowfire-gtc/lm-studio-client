//---------------------------------------------------------------------------
// LM Studio LAN Client - Base64 Encoding/Decoding Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Base64.h"

const char TBase64::Base64Chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//---------------------------------------------------------------------------
int TBase64::CharToIndex(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}
//---------------------------------------------------------------------------
AnsiString TBase64::Encode(TStream* stream)
{
    TMemoryStream* memStream = new TMemoryStream();
    try
    {
        memStream->CopyFrom(stream, stream->Size - stream->Position);
        memStream->Position = 0;
        
        int size = memStream->Size;
        unsigned char* data = (unsigned char*)memStream->Memory;
        
        AnsiString result = "";
        int i = 0;
        
        while (i < size)
        {
            int octet_a = i < size ? data[i++] : 0;
            int octet_b = i < size ? data[i++] : 0;
            int octet_c = i < size ? data[i++] : 0;
            
            int triple = (octet_a << 16) + (octet_b << 8) + octet_c;
            
            result += Base64Chars[(triple >> 18) & 0x3F];
            result += Base64Chars[(triple >> 12) & 0x3F];
            result += Base64Chars[(triple >> 6) & 0x3F];
            result += Base64Chars[triple & 0x3F];
        }
        
        // Add padding
        int mod = size % 3;
        if (mod > 0)
        {
            for (int j = 0; j < 3 - mod; j++)
                result[result.Length()] = Base64Pad;
        }
        
        return result;
    }
    __finally
    {
        delete memStream;
    }
}
//---------------------------------------------------------------------------
AnsiString TBase64::Encode(const AnsiString& data)
{
    TMemoryStream* stream = new TMemoryStream();
    try
    {
        stream->WriteBuffer(data.c_str(), data.Length());
        stream->Position = 0;
        return Encode(stream);
    }
    __finally
    {
        delete stream;
    }
}
//---------------------------------------------------------------------------
TMemoryStream* TBase64::Decode(const AnsiString& base64)
{
    TMemoryStream* result = new TMemoryStream();
    
    int len = base64.Length();
    int padding = 0;
    
    if (len > 0 && base64[len] == Base64Pad) padding++;
    if (len > 1 && base64[len - 1] == Base64Pad) padding++;
    
    for (int i = 1; i <= len - padding; i += 4)
    {
        int sextet_a = CharToIndex(base64[i]);
        int sextet_b = CharToIndex(base64[i + 1]);
        int sextet_c = (i + 2 <= len) ? CharToIndex(base64[i + 2]) : 0;
        int sextet_d = (i + 3 <= len) ? CharToIndex(base64[i + 3]) : 0;
        
        int triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;
        
        unsigned char byte1 = (triple >> 16) & 0xFF;
        unsigned char byte2 = (triple >> 8) & 0xFF;
        unsigned char byte3 = triple & 0xFF;
        
        result->WriteBuffer(&byte1, 1);
        if (i + 2 <= len - padding || padding < 2)
            result->WriteBuffer(&byte2, 1);
        if (i + 3 <= len - padding || padding < 1)
            result->WriteBuffer(&byte3, 1);
    }
    
    result->Position = 0;
    return result;
}
//---------------------------------------------------------------------------
AnsiString TBase64::DecodeToString(const AnsiString& base64)
{
    TMemoryStream* stream = Decode(base64);
    try
    {
        AnsiString result = "";
        result.SetLength(stream->Size);
        stream->ReadBuffer(result.c_str(), stream->Size);
        return result;
    }
    __finally
    {
        delete stream;
    }
}
//---------------------------------------------------------------------------
