//---------------------------------------------------------------------------
// LM Studio LAN Client - Main Form Header
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include "HttpClient.h"
#include "StreamParser.h"
#include "MarkdownRenderer.h"
#include "CodeExporter.h"
#include "VisionProcessor.h"
#include "DocProcessor.h"
#include "ConfigStore.h"

// Attachment structure
struct TAttachment
{
    enum Type { atDocument, atImage };
    Type type;
    AnsiString filePath;
    AnsiString mimeType;
    AnsiString base64Data;
    int fileSize;
    
    __fastcall TAttachment(Type t, const AnsiString& path)
    {
        type = t;
        filePath = path;
        fileSize = GetFileSize(path);
    }
};

// Message structure
struct TMessage
{
    AnsiString role;
    AnsiString content;
    TList* attachments;
    bool isStreaming;
    TDateTime timestamp;
    
    __fastcall TMessage()
    {
        attachments = new TList();
    }
    
    __fastcall ~TMessage()
    {
        for (int i = 0; i < attachments->Count; i++)
            delete (TAttachment*)attachments->Items[i];
        delete attachments;
    }
};

class TMainForm : public TForm
{
__published:
    TPanel* pnlTop;
    TComboBox* cbModel;
    TLabel* lblStatus;
    TShape* shpStatus;
    TPanel* pnlChat;
    TPaintBox* pbChat;
    TScrollBar* sbChat;
    TPanel* pnlAttachments;
    TListBox* lbFiles;
    TButton* btnAddFile;
    TButton* btnAddImage;
    TButton* btnRemoveFile;
    TPanel* pnlInput;
    TMemo* mmoInput;
    TButton* btnSend;
    TButton* btnStop;
    TButton* btnSettings;
    TStatusBar* StatusBar;
    TTimer* tmrStream;
    TOpenDialog* dlgOpenFile;
    TOpenPictureDialog* dlgOpenImage;
    TSaveDialog* dlgExport;
    TMainMenu* MainMenu;
    TMenuItem* File1;
    TMenuItem* NewChat1;
    TMenuItem* SaveChat1;
    TMenuItem* LoadChat1;
    TMenuItem* N1;
    TMenuItem* Exit1;
    TMenuItem* Edit1;
    TMenuItem* Copy1;
    TMenuItem* Clear1;
    TMenuItem* Settings1;
    TMenuItem* Help1;
    TMenuItem* About1;
    
    void __fastcall FormCreate(TObject* Sender);
    void __fastcall FormDestroy(TObject* Sender);
    void __fastcall btnSendClick(TObject* Sender);
    void __fastcall btnStopClick(TObject* Sender);
    void __fastcall btnSettingsClick(TObject* Sender);
    void __fastcall btnAddFileClick(TObject* Sender);
    void __fastcall btnAddImageClick(TObject* Sender);
    void __fastcall btnRemoveFileClick(TObject* Sender);
    void __fastcall pbChatPaint(TObject* Sender);
    void __fastcall sbChatChange(TObject* Sender);
    void __fastcall tmrStreamTimer(TObject* Sender);
    void __fastcall NewChat1Click(TObject* Sender);
    void __fastcall Exit1Click(TObject* Sender);
    void __fastcall About1Click(TObject* Sender);
    
private:
    THttpClient* httpClient;
    TStreamParser* streamParser;
    TMarkdownRenderer* markdownRenderer;
    TCodeExporter* codeExporter;
    TVisionProcessor* visionProcessor;
    TDocProcessor* docProcessor;
    TConfigStore* config;
    
    TList* messages; // List of TMessage*
    TList* attachments; // List of TAttachment*
    AnsiString currentResponse;
    bool isStreaming;
    int scrollPos;
    
    void __fastcall OnTokenReceived(const AnsiString& token);
    void __fastcall UpdateChatDisplay();
    void __fastcall SendMessage();
    void __fastcall BuildAndSendRequest();
    void __fastcall AddUserMessage(const AnsiString& text);
    void __fastcall AddAssistantMessage();
    void __fastcall UpdateStatus(const AnsiString& status, bool connected);
    void __fastcall RefreshModels();
    
public:
    __fastcall TMainForm(TComponent* Owner);
};

extern PACKAGE TMainForm* MainForm;

#endif
