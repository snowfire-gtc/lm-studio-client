//---------------------------------------------------------------------------
// LM Studio LAN Client - Configuration Storage Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConfigStore.h"
#include "Utils.h"

TConfigStore* TConfigStore::Instance = NULL;

//---------------------------------------------------------------------------
TConfigStore::TConfigStore()
{
    UseRegistry = true;
    RegIniFile = NULL;
    IniFile = NULL;
    
    // Try registry first, fall back to INI file
    try
    {
        RegIniFile = new TRegIniFile("Software\\LMStudioClient");
        UseRegistry = true;
    }
    catch (...)
    {
        // Registry access failed, use INI file
        AnsiString iniPath = ExtractFilePath(Application->ExeName) + "lmstudio.ini";
        IniFile = new TIniFile(iniPath);
        UseRegistry = false;
    }
}
//---------------------------------------------------------------------------
__fastcall TConfigStore::~TConfigStore()
{
    if (RegIniFile) delete RegIniFile;
    if (IniFile) delete IniFile;
}
//---------------------------------------------------------------------------
TConfigStore* TConfigStore::GetInstance()
{
    if (Instance == NULL)
    {
        Instance = new TConfigStore();
    }
    return Instance;
}
//---------------------------------------------------------------------------
void TConfigStore::Load()
{
    // Load is automatic via Get methods, but we can preload defaults here
    if (!GetServerHost().IsEmpty()) return; // Already configured
    
    // Set defaults
    SetServerHost("127.0.0.1");
    SetServerPort(1234);
    SetServerTimeout(30);
    SetDefaultModel("local-model");
    SetExportFolder(ExtractFilePath(Application->ExeName) + "Exports");
    SetFilenameTemplate("{model}_{timestamp}_{index}");
    SetAutoOverwrite(false);
    SetFontName("Courier New");
    SetFontSize(10);
    SetTheme("Default");
    SetEnableLaTeX(true);
    SetMaxImageRes(1024);
    SetMaxContextTokens(4096);
    SetTemperature(0.7);
    SetStreamChunkSize(5);
}
//---------------------------------------------------------------------------
void TConfigStore::Save()
{
    // Settings are saved immediately when set
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetServerHost()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Server", "Host", "127.0.0.1");
    else
        return IniFile->ReadString("Server", "Host", "127.0.0.1");
}
//---------------------------------------------------------------------------
void TConfigStore::SetServerHost(const AnsiString& host)
{
    if (UseRegistry)
        RegIniFile->WriteString("Server", "Host", host);
    else
        IniFile->WriteString("Server", "Host", host);
}
//---------------------------------------------------------------------------
int TConfigStore::GetServerPort()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Server", "Port", 1234);
    else
        return IniFile->ReadInteger("Server", "Port", 1234);
}
//---------------------------------------------------------------------------
void TConfigStore::SetServerPort(int port)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Server", "Port", port);
    else
        IniFile->WriteInteger("Server", "Port", port);
}
//---------------------------------------------------------------------------
int TConfigStore::GetServerTimeout()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Server", "Timeout", 30);
    else
        return IniFile->ReadInteger("Server", "Timeout", 30);
}
//---------------------------------------------------------------------------
void TConfigStore::SetServerTimeout(int timeout)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Server", "Timeout", timeout);
    else
        IniFile->WriteInteger("Server", "Timeout", timeout);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetDefaultModel()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Models", "Default", "local-model");
    else
        return IniFile->ReadString("Models", "Default", "local-model");
}
//---------------------------------------------------------------------------
void TConfigStore::SetDefaultModel(const AnsiString& model)
{
    if (UseRegistry)
        RegIniFile->WriteString("Models", "Default", model);
    else
        IniFile->WriteString("Models", "Default", model);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetLastUsedModel()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Models", "LastUsed", "");
    else
        return IniFile->ReadString("Models", "LastUsed", "");
}
//---------------------------------------------------------------------------
void TConfigStore::SetLastUsedModel(const AnsiString& model)
{
    if (UseRegistry)
        RegIniFile->WriteString("Models", "LastUsed", model);
    else
        IniFile->WriteString("Models", "LastUsed", model);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetExportFolder()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Export", "DefaultFolder", "");
    else
        return IniFile->ReadString("Export", "DefaultFolder", "");
}
//---------------------------------------------------------------------------
void TConfigStore::SetExportFolder(const AnsiString& folder)
{
    if (UseRegistry)
        RegIniFile->WriteString("Export", "DefaultFolder", folder);
    else
        IniFile->WriteString("Export", "DefaultFolder", folder);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetFilenameTemplate()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Export", "FilenameTemplate", "{model}_{timestamp}_{index}");
    else
        return IniFile->ReadString("Export", "FilenameTemplate", "{model}_{timestamp}_{index}");
}
//---------------------------------------------------------------------------
void TConfigStore::SetFilenameTemplate(const AnsiString& template)
{
    if (UseRegistry)
        RegIniFile->WriteString("Export", "FilenameTemplate", template);
    else
        IniFile->WriteString("Export", "FilenameTemplate", template);
}
//---------------------------------------------------------------------------
bool TConfigStore::GetAutoOverwrite()
{
    if (UseRegistry)
        return RegIniFile->ReadBool("Export", "AutoOverwrite", false);
    else
        return IniFile->ReadBool("Export", "AutoOverwrite", false);
}
//---------------------------------------------------------------------------
void TConfigStore::SetAutoOverwrite(bool overwrite)
{
    if (UseRegistry)
        RegIniFile->WriteBool("Export", "AutoOverwrite", overwrite);
    else
        IniFile->WriteBool("Export", "AutoOverwrite", overwrite);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetFontName()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Rendering", "FontName", "Courier New");
    else
        return IniFile->ReadString("Rendering", "FontName", "Courier New");
}
//---------------------------------------------------------------------------
void TConfigStore::SetFontName(const AnsiString& font)
{
    if (UseRegistry)
        RegIniFile->WriteString("Rendering", "FontName", font);
    else
        IniFile->WriteString("Rendering", "FontName", font);
}
//---------------------------------------------------------------------------
int TConfigStore::GetFontSize()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Rendering", "FontSize", 10);
    else
        return IniFile->ReadInteger("Rendering", "FontSize", 10);
}
//---------------------------------------------------------------------------
void TConfigStore::SetFontSize(int size)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Rendering", "FontSize", size);
    else
        IniFile->WriteInteger("Rendering", "FontSize", size);
}
//---------------------------------------------------------------------------
AnsiString TConfigStore::GetTheme()
{
    if (UseRegistry)
        return RegIniFile->ReadString("Rendering", "Theme", "Default");
    else
        return IniFile->ReadString("Rendering", "Theme", "Default");
}
//---------------------------------------------------------------------------
void TConfigStore::SetTheme(const AnsiString& theme)
{
    if (UseRegistry)
        RegIniFile->WriteString("Rendering", "Theme", theme);
    else
        IniFile->WriteString("Rendering", "Theme", theme);
}
//---------------------------------------------------------------------------
bool TConfigStore::GetEnableLaTeX()
{
    if (UseRegistry)
        return RegIniFile->ReadBool("Rendering", "EnableLaTeX", true);
    else
        return IniFile->ReadBool("Rendering", "EnableLaTeX", true);
}
//---------------------------------------------------------------------------
void TConfigStore::SetEnableLaTeX(bool enable)
{
    if (UseRegistry)
        RegIniFile->WriteBool("Rendering", "EnableLaTeX", enable);
    else
        IniFile->WriteBool("Rendering", "EnableLaTeX", enable);
}
//---------------------------------------------------------------------------
int TConfigStore::GetMaxImageRes()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Rendering", "MaxImageRes", 1024);
    else
        return IniFile->ReadInteger("Rendering", "MaxImageRes", 1024);
}
//---------------------------------------------------------------------------
void TConfigStore::SetMaxImageRes(int res)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Rendering", "MaxImageRes", res);
    else
        IniFile->WriteInteger("Rendering", "MaxImageRes", res);
}
//---------------------------------------------------------------------------
int TConfigStore::GetMaxContextTokens()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Advanced", "MaxContextTokens", 4096);
    else
        return IniFile->ReadInteger("Advanced", "MaxContextTokens", 4096);
}
//---------------------------------------------------------------------------
void TConfigStore::SetMaxContextTokens(int tokens)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Advanced", "MaxContextTokens", tokens);
    else
        IniFile->WriteInteger("Advanced", "MaxContextTokens", tokens);
}
//---------------------------------------------------------------------------
double TConfigStore::GetTemperature()
{
    AnsiString strVal;
    if (UseRegistry)
        strVal = RegIniFile->ReadString("Advanced", "Temperature", "0.7");
    else
        strVal = IniFile->ReadString("Advanced", "Temperature", "0.7");
    return StrToFloatDef(strVal, 0.7);
}
//---------------------------------------------------------------------------
void TConfigStore::SetTemperature(double temp)
{
    AnsiString strVal = FloatToStr(temp);
    if (UseRegistry)
        RegIniFile->WriteString("Advanced", "Temperature", strVal);
    else
        IniFile->WriteString("Advanced", "Temperature", strVal);
}
//---------------------------------------------------------------------------
int TConfigStore::GetStreamChunkSize()
{
    if (UseRegistry)
        return RegIniFile->ReadInteger("Advanced", "StreamChunkSize", 5);
    else
        return IniFile->ReadInteger("Advanced", "StreamChunkSize", 5);
}
//---------------------------------------------------------------------------
void TConfigStore::SetStreamChunkSize(int size)
{
    if (UseRegistry)
        RegIniFile->WriteInteger("Advanced", "StreamChunkSize", size);
    else
        IniFile->WriteInteger("Advanced", "StreamChunkSize", size);
}
//---------------------------------------------------------------------------
