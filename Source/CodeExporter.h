//---------------------------------------------------------------------------
// LM Studio LAN Client - Code Exporter
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef CodeExporterH
#define CodeExporterH

#include <Classes.hpp>
#include <Dialogs.hpp>

class TCodeExporter
{
private:
    TSaveDialog* SaveDialog;
    AnsiString DefaultFolder;
    AnsiString FilenameTemplate;
    
    AnsiString ApplyTemplate(const AnsiString& template, 
                             const AnsiString& language,
                             const AnsiString& model);
    AnsiString GetExtensionForLanguage(const AnsiString& language);
    
public:
    __fastcall TCodeExporter();
    __fastcall ~TCodeExporter();
    
    bool ExportCodeBlock(const AnsiString& code, 
                         const AnsiString& language,
                         const AnsiString& suggestedName);
    
    void SetDefaultFolder(const AnsiString& folder) { DefaultFolder = folder; }
    void SetFilenameTemplate(const AnsiString& template) { FilenameTemplate = template; }
};

#endif
