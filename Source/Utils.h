//---------------------------------------------------------------------------
// LM Studio LAN Client - Utility Functions
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH

#include <Classes.hpp>
#include <Graphics.hpp>

// Logging
void LogError(const AnsiString& category, const AnsiString& message);
void LogInfo(const AnsiString& message);

// GDI+ Wrapper
class TGDIPlusWrap
{
private:
    static bool Initialized;
    
public:
    static void Initialize();
    static void Shutdown();
};

// File utilities
AnsiString ReadTextFile(const AnsiString& filePath);
void WriteTextFile(const AnsiString& filePath, const AnsiString& content);
int GetFileSize(const AnsiString& filePath);
bool FileExistsSafe(const AnsiString& filePath);

// String utilities
AnsiString Trim(const AnsiString& str);
AnsiString ReplaceAll(const AnsiString& str, const AnsiString& oldStr, const AnsiString& newStr);
AnsiString FormatDateTime(const AnsiString& format, TDateTime dt);
AnsiString GetTimestamp();

// Graphic utilities
TGraphic* LoadGraphic(const AnsiString& filePath);
void ResizeImage(TGraphic*& img, int maxDimension);

#endif
