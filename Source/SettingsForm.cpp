//---------------------------------------------------------------------------
// LM Studio LAN Client - Settings Form Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SettingsForm.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TSettingsForm* SettingsForm;
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
    : TForm(Owner)
{
    config = TConfigStore::GetInstance();
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::FormCreate(TObject* Sender)
{
    // Load current settings
    edtHost->Text = config->GetServerHost();
    edtPort->Text = IntToStr(config->GetServerPort());
    edtTimeout->Text = IntToStr(config->GetServerTimeout());
    
    edtExportFolder->Text = config->GetExportFolder();
    edtTemplate->Text = config->GetFilenameTemplate();
    
    cbFontName->Items->Add("Arial");
    cbFontName->Items->Add("Courier New");
    cbFontName->Items->Add("Times New Roman");
    cbFontName->Items->Add("Verdana");
    cbFontName->Text = config->GetFontName();
    
    edtFontSize->Text = IntToStr(config->GetFontSize());
    chkEnableLaTeX->Checked = config->GetEnableLaTeX();
    edtMaxImageRes->Text = IntToStr(config->GetMaxImageRes());
    
    edtMaxTokens->Text = IntToStr(config->GetMaxContextTokens());
    edtTemperature->Text = FloatToStr(config->GetTemperature());
    edtChunkSize->Text = IntToStr(config->GetStreamChunkSize());
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::btnOKClick(TObject* Sender)
{
    // Save server settings
    config->SetServerHost(edtHost->Text);
    config->SetServerPort(StrToIntDef(edtPort->Text, 1234));
    config->SetServerTimeout(StrToIntDef(edtTimeout->Text, 30));
    
    // Save export settings
    config->SetExportFolder(edtExportFolder->Text);
    config->SetFilenameTemplate(edtTemplate->Text);
    
    // Save rendering settings
    config->SetFontName(cbFontName->Text);
    config->SetFontSize(StrToIntDef(edtFontSize->Text, 10));
    config->SetEnableLaTeX(chkEnableLaTeX->Checked);
    config->SetMaxImageRes(StrToIntDef(edtMaxImageRes->Text, 1024));
    
    // Save advanced settings
    config->SetMaxContextTokens(StrToIntDef(edtMaxTokens->Text, 4096));
    config->SetTemperature(StrToFloatDef(edtTemperature->Text, 0.7));
    config->SetStreamChunkSize(StrToIntDef(edtChunkSize->Text, 5));
    
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::btnBrowseFolderClick(TObject* Sender)
{
    TSelectDirectoryDialog* dlg = new TSelectDirectoryDialog(this);
    try
    {
        dlg->InitialDir = edtExportFolder->Text;
        if (dlg->Execute())
        {
            edtExportFolder->Text = dlg->FileName;
        }
    }
    __finally
    {
        delete dlg;
    }
}
//---------------------------------------------------------------------------
