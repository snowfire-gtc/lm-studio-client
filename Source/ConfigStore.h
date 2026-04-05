//---------------------------------------------------------------------------
// LM Studio LAN Client - Configuration Storage (Registry/INI)
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef ConfigStoreH
#define ConfigStoreH

#include <Classes.hpp>
#include <IniFiles.hpp>

class TConfigStore
{
private:
    static TConfigStore* Instance;
    
    TRegIniFile* RegIniFile;
    TIniFile* IniFile;
    bool UseRegistry;
    
    TConfigStore();
    
public:
    __fastcall ~TConfigStore();
    
    static TConfigStore* GetInstance();
    
    void Load();
    void Save();
    
    // Server Settings
    AnsiString GetServerHost();
    void SetServerHost(const AnsiString& host);
    int GetServerPort();
    void SetServerPort(int port);
    int GetServerTimeout();
    void SetServerTimeout(int timeout);
    
    // Model Settings
    AnsiString GetDefaultModel();
    void SetDefaultModel(const AnsiString& model);
    AnsiString GetLastUsedModel();
    void SetLastUsedModel(const AnsiString& model);
    
    // Export Settings
    AnsiString GetExportFolder();
    void SetExportFolder(const AnsiString& folder);
    AnsiString GetFilenameTemplate();
    void SetFilenameTemplate(const AnsiString& template);
    bool GetAutoOverwrite();
    void SetAutoOverwrite(bool overwrite);
    
    // Rendering Settings
    AnsiString GetFontName();
    void SetFontName(const AnsiString& font);
    int GetFontSize();
    void SetFontSize(int size);
    AnsiString GetTheme();
    void SetTheme(const AnsiString& theme);
    bool GetEnableLaTeX();
    void SetEnableLaTeX(bool enable);
    int GetMaxImageRes();
    void SetMaxImageRes(int res);
    
    // Advanced Settings
    int GetMaxContextTokens();
    void SetMaxContextTokens(int tokens);
    double GetTemperature();
    void SetTemperature(double temp);
    int GetStreamChunkSize();
    void SetStreamChunkSize(int size);
};

#endif
