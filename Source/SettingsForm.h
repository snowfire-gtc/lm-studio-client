//---------------------------------------------------------------------------
// LM Studio LAN Client - Settings Form Header
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef SettingsFormH
#define SettingsFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "ConfigStore.h"

class TSettingsForm : public TForm
{
__published:
    TPageControl* PageControl;
    TTabSheet* tsServer;
    TTabSheet* tsExport;
    TTabSheet* tsRendering;
    TTabSheet* tsAdvanced;
    
    TLabel* lblHost;
    TEdit* edtHost;
    TLabel* lblPort;
    TEdit* edtPort;
    TLabel* lblTimeout;
    TEdit* edtTimeout;
    
    TLabel* lblExportFolder;
    TEdit* edtExportFolder;
    TButton* btnBrowseFolder;
    TLabel* lblTemplate;
    TEdit* edtTemplate;
    
    TLabel* lblFontName;
    TComboBox* cbFontName;
    TLabel* lblFontSize;
    TEdit* edtFontSize;
    TCheckBox* chkEnableLaTeX;
    TLabel* lblMaxImageRes;
    TEdit* edtMaxImageRes;
    
    TLabel* lblMaxTokens;
    TEdit* edtMaxTokens;
    TLabel* lblTemperature;
    TEdit* edtTemperature;
    TLabel* lblChunkSize;
    TEdit* edtChunkSize;
    
    TButton* btnOK;
    TButton* btnCancel;
    
    void __fastcall FormCreate(TObject* Sender);
    void __fastcall btnOKClick(TObject* Sender);
    void __fastcall btnBrowseFolderClick(TObject* Sender);
    
private:
    TConfigStore* config;
    
public:
    __fastcall TSettingsForm(TComponent* Owner);
};

extern PACKAGE TSettingsForm* SettingsForm;

#endif
