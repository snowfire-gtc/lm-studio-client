//---------------------------------------------------------------------------
// LM Studio LAN Client - Streaming JSON Parser Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StreamParser.h"

//---------------------------------------------------------------------------
__fastcall TStreamParser::TStreamParser()
{
    buffer = "";
    OnTokenReceived = NULL;
}
//---------------------------------------------------------------------------
void TStreamParser::Reset()
{
    buffer = "";
}
//---------------------------------------------------------------------------
AnsiString TStreamParser::UnescapeJSON(const AnsiString& escaped)
{
    AnsiString result = "";
    int i = 1;
    
    while (i <= escaped.Length())
    {
        char c = escaped[i];
        
        if (c == '\\' && i < escaped.Length())
        {
            char next = escaped[i + 1];
            switch (next)
            {
                case '"': result += '"'; i += 2; break;
                case '\\': result += '\\'; i += 2; break;
                case '/': result += '/'; i += 2; break;
                case 'b': result += '\b'; i += 2; break;
                case 'f': result += '\f'; i += 2; break;
                case 'n': result += '\n'; i += 2; break;
                case 'r': result += '\r'; i += 2; break;
                case 't': result += '\t'; i += 2; break;
                case 'u':
                    // Unicode escape \uXXXX - simplified handling
                    if (i + 5 <= escaped.Length())
                    {
                        AnsiString hex = escaped.SubString(i + 2, 4);
                        int code = StrToIntDef("$" + hex, 0);
                        if (code > 0 && code < 128)
                            result += (char)code;
                        else
                            result += '?';
                        i += 6;
                    }
                    else
                    {
                        result += c;
                        i++;
                    }
                    break;
                default:
                    result += c;
                    i++;
            }
        }
        else
        {
            result += c;
            i++;
        }
    }
    
    return result;
}
//---------------------------------------------------------------------------
void TStreamParser::ParseChunk(const AnsiString& json)
{
    // Quick validation
    if (json.Pos("\"choices\"") == 0) return;
    
    // Extract delta content manually (avoid full parse)
    int start = json.Pos("\"content\":\"");
    if (start == 0) return;
    start += 11;
    
    // Find closing quote (handle escaped quotes)
    int end = start;
    while (end <= json.Length())
    {
        if (json[end] == '"' && (end == start || json[end - 1] != '\\'))
            break;
        end++;
    }
    
    if (end > json.Length()) return;
    
    AnsiString delta = json.SubString(start, end - start);
    delta = UnescapeJSON(delta);
    
    // Fire callback
    if (OnTokenReceived != NULL)
    {
        OnTokenReceived(delta);
    }
}
//---------------------------------------------------------------------------
