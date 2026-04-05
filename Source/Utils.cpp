//---------------------------------------------------------------------------
// LM Studio LAN Client - Utility Functions Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Utils.h"
#include <fstream>
#include <jpeg.hpp>
#include <pngimage.hpp>

bool TGDIPlusWrap::Initialized = false;

//---------------------------------------------------------------------------
void LogError(const AnsiString& category, const AnsiString& message)
{
    try
    {
        AnsiString logPath = ExtractFilePath(Application->ExeName) + "lmstudio.log";
        
        // Check log size and rotate if needed (1MB limit)
        if (FileExists(logPath))
        {
            TFileStream* fs = new TFileStream(logPath, fmOpenRead);
            try
            {
                if (fs->Size > 1024 * 1024)
                {
                    delete fs;
                    DeleteFile(logPath + ".old");
                    RenameFile(logPath, logPath + ".old");
                }
                else
                {
                    delete fs;
                }
            }
            catch (...)
            {
                // Ignore rotation errors
            }
        }
        
        TFileStream* log = new TFileStream(logPath, fmOpenWrite | fmCreate);
        try
        {
            log->Seek(0, soEnd);
            AnsiString entry = FormatDateTime("yyyy-mm-dd hh:nn:ss", Now()) +
                              " [" + category + "] " + message + "\r\n";
            log->Write(entry.c_str(), entry.Length());
        }
        __finally
        {
            delete log;
        }
    }
    catch (...)
    {
        // Silent fail for logging
    }
}
//---------------------------------------------------------------------------
void LogInfo(const AnsiString& message)
{
    LogError("INFO", message);
}
//---------------------------------------------------------------------------
void TGDIPlusWrap::Initialize()
{
    // GDI+ is built into Windows XP SP3
    // This is a placeholder for actual GDI+ initialization if needed
    Initialized = true;
}
//---------------------------------------------------------------------------
void TGDIPlusWrap::Shutdown()
{
    Initialized = false;
}
//---------------------------------------------------------------------------
AnsiString ReadTextFile(const AnsiString& filePath)
{
    TStringList* sl = new TStringList();
    try
    {
        sl->LoadFromFile(filePath);
        return sl->Text;
    }
    __finally
    {
        delete sl;
    }
}
//---------------------------------------------------------------------------
void WriteTextFile(const AnsiString& filePath, const AnsiString& content)
{
    TStringList* sl = new TStringList();
    try
    {
        sl->Text = content;
        sl->SaveToFile(filePath);
    }
    __finally
    {
        delete sl;
    }
}
//---------------------------------------------------------------------------
int GetFileSize(const AnsiString& filePath)
{
    TSearchRec sr;
    if (FindFirst(filePath, faAnyFile, sr) == 0)
    {
        FindClose(sr);
        return sr.Size;
    }
    return 0;
}
//---------------------------------------------------------------------------
bool FileExistsSafe(const AnsiString& filePath)
{
    return FileExists(filePath);
}
//---------------------------------------------------------------------------
AnsiString Trim(const AnsiString& str)
{
    int start = 1;
    int end = str.Length();
    
    while (start <= end && str[start] <= ' ') start++;
    while (end >= start && str[end] <= ' ') end--;
    
    if (start > end) return "";
    return str.SubString(start, end - start + 1);
}
//---------------------------------------------------------------------------
AnsiString ReplaceAll(const AnsiString& str, const AnsiString& oldStr, const AnsiString& newStr)
{
    AnsiString result = str;
    int pos = 1;
    
    while ((pos = result.Pos(oldStr)) > 0)
    {
        result = result.SubString(1, pos - 1) + newStr + 
                 result.SubString(pos + oldStr.Length(), result.Length());
    }
    
    return result;
}
//---------------------------------------------------------------------------
AnsiString FormatDateTime(const AnsiString& format, TDateTime dt)
{
    // Simplified DateTime formatting for common patterns
    AnsiString result = format;
    
    int year = YearOf(dt);
    int month = MonthOf(dt);
    int day = DayOf(dt);
    int hour = HourOf(dt);
    int minute = MinuteOf(dt);
    int second = SecondOf(dt);
    
    result = ReplaceAll(result, "yyyy", IntToStr(year));
    result = ReplaceAll(result, "mm", (month < 10 ? "0" : "") + IntToStr(month));
    result = ReplaceAll(result, "dd", (day < 10 ? "0" : "") + IntToStr(day));
    result = ReplaceAll(result, "hh", (hour < 10 ? "0" : "") + IntToStr(hour));
    result = ReplaceAll(result, "nn", (minute < 10 ? "0" : "") + IntToStr(minute));
    result = ReplaceAll(result, "ss", (second < 10 ? "0" : "") + IntToStr(second));
    
    return result;
}
//---------------------------------------------------------------------------
AnsiString GetTimestamp()
{
    return FormatDateTime("yyyyMMdd_HHmmss", Now());
}
//---------------------------------------------------------------------------
TGraphic* LoadGraphic(const AnsiString& filePath)
{
    AnsiString ext = ExtractFileExt(filePath).LowerCase();
    
    if (ext == ".jpg" || ext == ".jpeg")
    {
        TJPEGImage* img = new TJPEGImage();
        img->LoadFromFile(filePath);
        return img;
    }
    else if (ext == ".png")
    {
        TPNGImage* img = new TPNGImage();
        img->LoadFromFile(filePath);
        return img;
    }
    else if (ext == ".bmp")
    {
        TBitmap* img = new TBitmap();
        img->LoadFromFile(filePath);
        return img;
    }
    
    throw Exception("Unsupported image format: " + ext);
}
//---------------------------------------------------------------------------
void ResizeImage(TGraphic*& img, int maxDimension)
{
    if (img->Width <= maxDimension && img->Height <= maxDimension)
        return;
    
    double ratio = (double)maxDimension / (img->Width > img->Height ? img->Width : img->Height);
    int newWidth = (int)(img->Width * ratio);
    int newHeight = (int)(img->Height * ratio);
    
    TBitmap* resized = new TBitmap();
    try
    {
        resized->Width = newWidth;
        resized->Height = newHeight;
        resized->Canvas->StretchDraw(Rect(0, 0, newWidth, newHeight), img);
        
        // Replace original with resized
        img->Assign(resized);
    }
    __finally
    {
        delete resized;
    }
}
//---------------------------------------------------------------------------
