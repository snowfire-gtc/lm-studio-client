//---------------------------------------------------------------------------
// LM Studio LAN Client - Main Application Entry Point
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "SettingsForm.h"
#include "HttpClient.h"
#include "StreamParser.h"
#include "MarkdownRenderer.h"
#include "SyntaxHighlight.h"
#include "LaTeXEngine.h"
#include "CodeExporter.h"
#include "VisionProcessor.h"
#include "DocProcessor.h"
#include "Base64.h"
#include "ConfigStore.h"
#include "Utils.h"

USEFORM("MainForm.dfm", MainForm);
USEFORM("SettingsForm.dfm", SettingsForm);
USEFORM("ExportDialog.dfm", ExportDialog);

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "LM Studio LAN Client";
        Application->HelpFile = "";
        
        // Initialize GDI+ for anti-aliased rendering
        TGDIPlusWrap::Initialize();
        
        // Load configuration from registry
        TConfigStore* config = TConfigStore::GetInstance();
        config->Load();
        
        // Create and show main form
        Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->CreateForm(__classid(TSettingsForm), &SettingsForm);
        Application->CreateForm(__classid(TExportDialog), &ExportDialog);
        
        Application->Run();
        
        // Cleanup GDI+
        TGDIPlusWrap::Shutdown();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
        return 1;
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
        return 1;
    }
    return 0;
}
//---------------------------------------------------------------------------
