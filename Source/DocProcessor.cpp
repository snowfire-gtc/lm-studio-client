//---------------------------------------------------------------------------
// LM Studio LAN Client - Document Processor Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DocProcessor.h"
#include "Utils.h"

//---------------------------------------------------------------------------
__fastcall TDocProcessor::TDocProcessor()
{
    MaxTokens = 4096;
}
//---------------------------------------------------------------------------
AnsiString TDocProcessor::ExtractText(const AnsiString& filePath)
{
    AnsiString ext = ExtractFileExt(filePath).LowerCase();
    
    if (ext == ".txt" || ext == ".csv" || ext == ".md")
    {
        return ReadTextFile(filePath);
    }
    else if (ext == ".rtf")
    {
        TRichEdit* rt = new TRichEdit(NULL);
        try
        {
            rt->Lines->LoadFromFile(filePath);
            return rt->Lines->Text;
        }
        __finally
        {
            delete rt;
        }
    }
    else if (ext == ".pdf")
    {
        // Limited support: try to read raw text streams
        return ExtractPDFText(filePath);
    }
    
    throw Exception("Unsupported format: " + ext);
}
//---------------------------------------------------------------------------
AnsiString TDocProcessor::ExtractPDFText(const AnsiString& filePath)
{
    // Basic PDF text extraction - reads text between stream markers
    // This is a simplified implementation for XP compatibility
    
    TStringList* lines = new TStringList();
    try
    {
        lines->LoadFromFile(filePath);
        
        AnsiString result = "";
        bool inTextStream = false;
        
        for (int i = 0; i < lines->Count; i++)
        {
            AnsiString line = lines->Strings[i];
            
            if (line.Pos("BT") > 0)
                inTextStream = true;
            else if (line.Pos("ET") > 0)
                inTextStream = false;
            
            if (inTextStream && line.Pos("Tj") > 0)
            {
                // Extract text between parentheses
                int start = line.Pos("(");
                int end = line.Pos(")");
                if (start > 0 && end > start)
                {
                    result += line.SubString(start + 1, end - start - 1) + " ";
                }
            }
        }
        
        return Trim(result);
    }
    __finally
    {
        delete lines;
    }
}
//---------------------------------------------------------------------------
