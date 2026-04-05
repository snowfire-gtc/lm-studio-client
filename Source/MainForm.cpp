//---------------------------------------------------------------------------
// LM Studio LAN Client - Main Form Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "SettingsForm.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
    httpClient = NULL;
    streamParser = new TStreamParser();
    markdownRenderer = new TMarkdownRenderer();
    codeExporter = new TCodeExporter();
    visionProcessor = new TVisionProcessor();
    docProcessor = new TDocProcessor();
    config = TConfigStore::GetInstance();
    
    messages = new TList();
    attachments = new TList();
    currentResponse = "";
    isStreaming = false;
    scrollPos = 0;
    
    streamParser->OnToken = OnTokenReceived;
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    for (int i = 0; i < messages->Count; i++)
        delete (TMessage*)messages->Items[i];
    delete messages;
    
    for (int i = 0; i < attachments->Count; i++)
        delete (TAttachment*)attachments->Items[i];
    delete attachments;
    
    delete streamParser;
    delete markdownRenderer;
    delete codeExporter;
    delete visionProcessor;
    delete docProcessor;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject* Sender)
{
    // Load configuration
    config->Load();
    
    // Set initial UI state
    cbModel->Text = config->GetDefaultModel();
    pnlAttachments->Visible = false;
    btnStop->Enabled = false;
    
    // Configure dialogs
    dlgOpenFile->Filter = "Text Files|*.txt|CSV Files|*.csv|Markdown|*.md|RTF Files|*.rtf|All Files|*.*";
    dlgOpenImage->Filter = "JPEG Images|*.jpg;*.jpeg|PNG Images|*.png|Bitmap|*.bmp|All Files|*.*";
    
    // Set export settings
    codeExporter->SetDefaultFolder(config->GetExportFolder());
    codeExporter->SetFilenameTemplate(config->GetFilenameTemplate());
    
    // Set processor limits
    visionProcessor->SetMaxDimension(config->GetMaxImageRes());
    docProcessor->SetMaxTokens(config->GetMaxContextTokens());
    
    UpdateStatus("Ready", false);
    
    // Double-buffer chat panel to prevent flicker
    pnlChat->DoubleBuffered = true;
    pbChat->DoubleBuffered = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject* Sender)
{
    if (httpClient && httpClient->GetIsConnected())
        httpClient->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateStatus(const AnsiString& status, bool connected)
{
    lblStatus->Caption = status;
    shpStatus->Brush->Color = connected ? clGreen : clRed;
    
    StatusBar->SimpleText = status;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddUserMessage(const AnsiString& text)
{
    TMessage* msg = new TMessage();
    msg->role = "user";
    msg->content = text;
    msg->timestamp = Now();
    
    // Add attachments
    for (int i = 0; i < attachments->Count; i++)
    {
        TAttachment* att = (TAttachment*)attachments->Items[i];
        msg->attachments->Add(new TAttachment(att->type, att->filePath));
    }
    
    messages->Add(msg);
    
    // Clear attachments after sending
    for (int i = 0; i < attachments->Count; i++)
        delete (TAttachment*)attachments->Items[i];
    attachments->Clear();
    lbFiles->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddAssistantMessage()
{
    TMessage* msg = new TMessage();
    msg->role = "assistant";
    msg->content = "";
    msg->isStreaming = true;
    msg->timestamp = Now();
    
    messages->Add(msg);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnTokenReceived(const AnsiString& token)
{
    currentResponse += token;
    
    // Update the last message (assistant)
    if (messages->Count > 0)
    {
        TMessage* lastMsg = (TMessage*)messages->Items[messages->Count - 1];
        lastMsg->content = currentResponse;
    }
    
    // Trigger UI update via timer (thread-safe)
    tmrStream->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::tmrStreamTimer(TObject* Sender)
{
    tmrStream->Enabled = false;
    UpdateChatDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateChatDisplay()
{
    // Repaint the chat area
    pbChat->Invalidate();
    
    // Update scrollbar
    sbChat->Max = pbChat->Height;
    sbChat->Position = sbChat->Max;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pbChatPaint(TObject* Sender)
{
    TCanvas* canvas = pbChat->Canvas;
    canvas->Brush->Color = clWhite;
    canvas->FillRect(pbChat->ClientOrigin);
    
    int yPos = 10;
    int width = pbChat->Width - 20;
    
    // Render all messages
    for (int i = 0; i < messages->Count; i++)
    {
        TMessage* msg = (TMessage*)messages->Items[i];
        
        // Draw role label
        canvas->Font->Style = fsBold;
        canvas->Font->Color = msg->role == "user" ? clBlue : clGreen;
        canvas->TextOut(10, yPos, "[" + msg->role + "]:");
        yPos += canvas->TextHeight("Xy");
        
        // Parse and render content
        if (!msg->content.IsEmpty())
        {
            markdownRenderer->Parse(msg->content);
            canvas->Font->Style = fsNormal;
            canvas->Font->Color = clBlack;
            markdownRenderer->Render(canvas, yPos, width);
        }
        
        yPos += 15; // Spacing between messages
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnSendClick(TObject* Sender)
{
    if (mmoInput->Text.Trim().IsEmpty())
        return;
    
    if (isStreaming)
        return;
    
    // Add user message
    AddUserMessage(mmoInput->Text);
    
    // Clear input
    mmoInput->Clear();
    
    // Add assistant message placeholder
    AddAssistantMessage();
    
    // Build and send request
    BuildAndSendRequest();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BuildAndSendRequest()
{
    isStreaming = true;
    currentResponse = "";
    btnSend->Enabled = false;
    btnStop->Enabled = true;
    
    // Build JSON payload
    AnsiString json = "{\n";
    json += "  \"model\": \"" + cbModel->Text + "\",\n";
    json += "  \"messages\": [\n";
    
    for (int i = 0; i < messages->Count; i++)
    {
        TMessage* msg = (TMessage*)messages->Items[i];
        
        if (i > 0) json += ",\n";
        json += "    {\n";
        json += "      \"role\": \"" + msg->role + "\",\n";
        json += "      \"content\": \"" + StringReplace(msg->content, "\"", "\\\"", true, true) + "\"\n";
        json += "    }";
    }
    
    json += "\n  ],\n";
    json += "  \"stream\": true\n";
    json += "}\n";
    
    // Create HTTP client
    httpClient = new THttpClient(config->GetServerHost(), config->GetServerPort());
    
    // Send request in a thread (simplified - in real app use TThread)
    try
    {
        // BCB6 doesn't support lambdas, use a static callback or member function
        // For now, we'll call SendRequest with NULL callback and handle streaming differently
        httpClient->SendRequest("/v1/chat/completions", json, true, NULL);
    }
    catch (...)
    {
        isStreaming = false;
        btnSend->Enabled = true;
        btnStop->Enabled = false;
        UpdateStatus("Error sending request", false);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnStopClick(TObject* Sender)
{
    if (httpClient)
        httpClient->Stop();
    
    isStreaming = false;
    btnSend->Enabled = true;
    btnStop->Enabled = false;
    
    // Mark streaming as complete
    if (messages->Count > 0)
    {
        TMessage* lastMsg = (TMessage*)messages->Items[messages->Count - 1];
        lastMsg->isStreaming = false;
    }
    
    UpdateStatus("Stopped", false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnAddFileClick(TObject* Sender)
{
    if (dlgOpenFile->Execute())
    {
        TAttachment* att = new TAttachment(TAttachment::atDocument, dlgOpenFile->FileName);
        attachments->Add(att);
        lbFiles->Items->Add(ExtractFileName(dlgOpenFile->FileName));
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnAddImageClick(TObject* Sender)
{
    if (dlgOpenImage->Execute())
    {
        TAttachment* att = new TAttachment(TAttachment::atImage, dlgOpenImage->FileName);
        attachments->Add(att);
        lbFiles->Items->Add(ExtractFileName(dlgOpenImage->FileName));
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnRemoveFileClick(TObject* Sender)
{
    if (lbFiles->ItemIndex >= 0)
    {
        delete (TAttachment*)attachments->Items[lbFiles->ItemIndex];
        attachments->Delete(lbFiles->ItemIndex);
        lbFiles->Items->Delete(lbFiles->ItemIndex);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::sbChatChange(TObject* Sender)
{
    pbChat->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnSettingsClick(TObject* Sender)
{
    SettingsForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewChat1Click(TObject* Sender)
{
    // Clear messages
    for (int i = 0; i < messages->Count; i++)
        delete (TMessage*)messages->Items[i];
    messages->Clear();
    
    markdownRenderer->Clear();
    pbChat->Repaint();
    
    UpdateStatus("New chat", false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Exit1Click(TObject* Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::About1Click(TObject* Sender)
{
    ShowMessage("LM Studio LAN Client\nVersion 1.0\nWindows XP / Borland C++ Builder 6");
}
//---------------------------------------------------------------------------
