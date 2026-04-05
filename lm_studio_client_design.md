# LM Studio LAN Client - Design Document
## Windows XP / Borland C++ Builder 6 Implementation

**Version:** 2.0 (Final)  
**Target Platform:** Windows XP (SP3)  
**IDE:** Borland C++ Builder 6 (BCB6)  
**Network Strategy:** Option B - WinSocket + Manual HTTP  
**Key Features:** Streaming, Syntax Highlighting, LaTeX Rendering, Code Export, Vision, Documents

---

## 1. Executive Summary

This document specifies the architecture and implementation details for a lightweight LM Studio API client designed to run on legacy Windows XP systems using Borland C++ Builder 6. The client connects to LM Studio servers on a LAN, supporting text chat, document attachments, vision tasks, real-time token streaming, syntax-highlighted code blocks with export capabilities, and LaTeX mathematical rendering.

### 1.1 Critical Constraints
- **OS:** Windows XP (No Unicode API, limited TLS support)
- **Compiler:** Borland C++ Builder 6 (2002), VCL framework only
- **Memory:** Max 64MB RAM usage
- **Network:** Raw WinSocket API (No Indy, no external DLLs)
- **Rendering:** GDI+ for syntax highlighting and LaTeX (No WebBrowser control)
- **Deployment:** Single EXE + optional GDI+ redistributable

### 1.2 Core Capabilities
1. **Chat Interface:** Real-time streaming response display
2. **Multi-modal Input:** Text + Document Attachments + Images
3. **Rich Output:** 
   - Markdown parsing
   - Syntax highlighted code blocks (C++, Python, SQL, etc.)
   - LaTeX math rendering ($...$ and $$...$$)
4. **Code Management:** One-click export of code blocks to disk
5. **Vision:** Image analysis via base64 encoding
6. **Document Processing:** Text extraction from PDF/DOC/TXT

---

## 2. System Architecture

### 2.1 High-Level Diagram

```
+---------------------+      +----------------------+      +------------------+
|   Presentation Layer|      |   Business Logic     |      |   Data Access    |
|   (VCL Forms)       |<---->|   (Controllers)      |<---->|   (WinSocket)    |
|                     |      |                      |      |                  |
| - TMainForm         |      | - TChatController    |      | - THttpClient    |
| - TSettingsForm     |      | - TStreamParser      |      | - TJsonParser    |
| - TExportDialog     |      | - TMarkdownRenderer  |      | - TFileHandler   |
| - TPreviewPanel     |      | - TLaTeXEngine       |      | - TConfigStore   |
+---------------------+      | - TCodeExporter      |      +------------------+
                             | - TVisionProcessor   |
                             | - TDocProcessor      |
                             +----------------------+
                                      ^
                                      |
                              +------------------+
                              |   Utilities      |
                              |                  |
                              | - TBase64        |
                              | - TSyntaxHighlight|
                              | - TGDIPlusWrap   |
                              +------------------+
```

### 2.2 Module Responsibilities

#### 2.2.1 Presentation Layer (VCL)
- **TMainForm:** Main chat window, input area, attachment panel, streaming display
- **TSettingsForm:** Server config, model selection, export paths, rendering options
- **TExportDialog:** File naming templates, extension mapping, folder selection
- **TPreviewPanel:** Document/Image previews before sending

#### 2.2.2 Business Logic
- **TChatController:** Manages conversation state, message history, context building
- **TStreamParser:** Incremental JSON parsing for SSE (Server-Sent Events) streaming
- **TMarkdownRenderer:** Converts Markdown tokens to VCL drawing commands
- **TLaTeXEngine:** Parses LaTeX expressions and renders via GDI+
- **TCodeExporter:** Handles file I/O for code blocks with naming conventions
- **TVisionProcessor:** Prepares image data (resize, encode) for API
- **TDocProcessor:** Extracts text from attached documents

#### 2.2.3 Data Access
- **THttpClient:** Raw WinSocket implementation for HTTP/1.1 with chunked transfer
- **TJsonParser:** Lightweight recursive descent parser for JSON responses
- **TFileHandler:** File I/O abstraction for attachments and exports
- **TConfigStore:** Registry-based configuration persistence

#### 2.2.4 Utilities
- **TBase64:** Encoding/decoding for images and binary data
- **TSyntaxHighlight:** Language-specific token coloring rules
- **TGDIPlusWrap:** Wrapper for GDI+ API (anti-aliased text, gradients)

---

## 3. Network Implementation (WinSocket + Manual HTTP)

### 3.1 Socket Management

```cpp
// THttpClient Header Snippet
class THttpClient {
private:
    SOCKET hSocket;
    AnsiString host;
    int port;
    bool isConnected;
    
    bool ConnectSocket();
    void DisconnectSocket();
    AnsiString BuildHttpRequest(const AnsiString& method, 
                                const AnsiString& path,
                                const AnsiString& body,
                                bool isStreaming);
    AnsiString ReadResponse(bool streaming, TStreamCallback callback);
    
public:
    __fastcall THttpClient(const AnsiString& host, int port);
    __fastcall ~THttpClient();
    
    bool SendRequest(const AnsiString& endpoint, 
                     const AnsiString& jsonBody,
                     bool streaming,
                     TStreamCallback onChunk);
};
```

### 3.2 HTTP Protocol Details

#### 3.2.1 Request Format (Streaming)
```http
POST /v1/chat/completions HTTP/1.1
Host: <LAN_IP>:1234
Content-Type: application/json
Accept: text/event-stream
Cache-Control: no-cache
Content-Length: <length>

{
  "model": "local-model",
  "messages": [...],
  "stream": true
}
```

#### 3.2.2 Response Parsing (SSE)
- Server sends chunks prefixed with `data: `
- Each chunk is a complete JSON object
- Stream ends with `data: [DONE]`
- Parser must handle partial TCP packets (buffer accumulation)

### 3.3 Streaming Callback Mechanism

```cpp
// Callback type definition
typedef void (__fastcall *TStreamCallback)(const AnsiString& jsonChunk);

// In THttpClient::ReadResponse
while (true) {
    int bytes = recv(hSocket, buffer, BUFFER_SIZE, 0);
    if (bytes <= 0) break;
    
    AppendToBuffer(buffer, bytes);
    
    while (HasCompleteLine()) {
        AnsiString line = ExtractLine();
        if (line.Pos("data: ") == 1) {
            AnsiString jsonData = line.SubString(7, line.Length() - 6);
            if (jsonData == "[DONE]") {
                SignalComplete();
                break;
            }
            if (onChunk != NULL) {
                onChunk(jsonData); // Fire event to UI thread
            }
        }
    }
}
```

---

## 4. User Interface Design

### 4.1 Main Form Layout (TMainForm)

```
+-----------------------------------------------------------------------+
|  MenuBar: File  Edit  View  Settings  Help                            |
+-----------------------------------------------------------------------+
|  Toolbar: [New Chat] [Load] [Save] [Settings] [Stop]                  |
+-----------------------------------------------------------------------+
|  Model Selector: [Dropdown: local-model ▼]   Status: [Connected ●]    |
+-----------------------------------------------------------------------+
|                                                                       |
|  +-----------------------------------------------------------------+  |
|  |  Chat Display Area (TMemo custom draw / TPaintBox)              |  |
|  |                                                                 |  |
|  |  [User]: Hello, explain quantum entanglement.                   |  |
|  |                                                                 |  |
|  |  [Assistant]: Quantum entanglement is a phenomenon where...     |  |
|  |                                                                 |  |
|  |  Here is the Schrödinger equation:                              |  |
|  |                                                                 |  |
|  |  +-----------------------------------------------------------+  |  |
|  |  | iℏ ∂/∂t Ψ(r,t) = Ĥ Ψ(r,t)                                 |  |  |
|  |  +-----------------------------------------------------------+  |  |
|  |                                                                 |  |
|  |  Example code in Python:                                        |  |
|  |  +-----------------------------------------------------------+  |  |
|  |  | def entangle(q1, q2):                                     |  |  |
|  |  |     return BellState(q1, q2)                              |  |  |
|  |  +-----------------------------------------------------------+  |  |
|  |  [Export Code...]                                               |  |
|  |                                                                 |  |
|  +-----------------------------------------------------------------+  |
|                                                                       |
+-----------------------------------------------------------------------+
|  Attachment Panel (Collapsible)                                       |
|  +-----------------------------------------------------------------+  |
|  | Files: [doc.pdf] [image.png] [notes.txt]  [+Add] [-Remove]      |  |
|  +-----------------------------------------------------------------+  |
+-----------------------------------------------------------------------+
|  Input Area                                                           |
|  +-----------------------------------------------------------------+  |
|  | [Multiline Edit: Type your message here...]                     |  |
|  |                                                                 |  |
|  +-----------------------------------------------------------------+  |
|  [Send] [Attach File] [Attach Image]                                |
+-----------------------------------------------------------------------+
|  Status Bar: Tokens: 128 | Time: 1.2s | Memory: 24MB                 |
+-----------------------------------------------------------------------+
```

### 4.2 Component Specification

| Component Name | Type | Purpose |
|----------------|------|---------|
| `pnlChat` | TPanel | Container for chat display (double-buffered) |
| `pbChat` | TPaintBox | Custom drawing surface for rich text |
| `mmoInput` | TMemo | Multi-line input box |
| `pnlAttachments` | TPanel | Collapsible attachment list |
| `lbFiles` | TListBox | List of attached files |
| `btnAddFile` | TButton | Open file dialog for documents |
| `btnAddImage` | TButton | Open picture dialog for vision |
| `cbModel` | TComboBox | Model selection dropdown |
| `btnSend` | TButton | Submit message |
| `btnStop` | TButton | Cancel streaming |
| `dlgOpenFile` | TOpenDialog | Document selection |
| `dlgOpenImage` | TOpenPictureDialog | Image selection |
| `dlgExport` | TSaveDialog | Code export destination |
| `tmrStream` | TTimer | UI update timer for streaming |

### 4.3 Settings Form (TSettingsForm)

- **Server Tab:** Host IP, Port, Timeout, SSL toggle (disabled for XP)
- **Models Tab:** Refresh button, default model selection
- **Export Tab:** Default folder, filename template (`{model}_{timestamp}_{index}`), extension mapping
- **Rendering Tab:** Font choices, color themes, LaTeX enable/disable, max image resolution
- **Advanced Tab:** Max context size, temperature, top_p, stream chunk size

---

## 5. Rich Text Rendering Engine

### 5.1 Markdown Parsing Strategy

Since BCB6 lacks modern markdown libraries, implement a state-machine parser:

```cpp
enum TMarkdownState {
    msText,
    msBold,
    msItalic,
    msCodeInline,
    msCodeBlockStart,
    msCodeBlock,
    msMathInline,
    msMathBlock,
    msLink
};

class TMarkdownParser {
private:
    TList* tokens; // List of TToken objects
    TMarkdownState currentState;
    
    void Parse(const AnsiString& input);
    TToken* CreateToken(TTokenType type, const AnsiString& content);
    
public:
    void FeedChunk(const AnsiString& chunk); // For streaming
    void Render(TCanvas* canvas, int& yPos);
};
```

### 5.2 Syntax Highlighting

#### 5.2.1 Language Detection
- Detect from markdown fence: ```cpp, ```python, ```sql
- Default to "text" if unspecified

#### 5.2.2 Tokenization Rules (Per Language)
Store rules in an INI file or hardcoded struct:

```cpp
struct TSyntaxRule {
    AnsiString pattern; // Regex-like simple pattern
    TColor color;
    TFontStyle style;
};

// Example for C++
TSyntaxRule CppRules[] = {
    {"//", clGreen, fsItalic}, // Comments
    {"#", clNavy, fsBold},     // Preprocessor
    {"int|void|class|return", clBlue, fsBold}, // Keywords
    {"\".*\"", clRed, fsNormal}, // Strings
    {"\\b\\d+\\b", clMaroon, fsNormal} // Numbers
};
```

#### 5.2.3 Rendering Process
1. Tokenize code block line by line
2. Calculate word widths using `Canvas->TextWidth()`
3. Draw each token with appropriate color/style
4. Use `ExtTextOut` for better GDI performance

### 5.3 LaTeX Rendering (GDI+)

#### 5.3.1 Supported Subset
- Inline math: `$ ... $`
- Display math: `$$ ... $$`
- Basic symbols: `\alpha`, `\beta`, `\sum`, `\int`, `\frac{}{}`, `^{}`, `_{} `
- Greek letters, operators, brackets

#### 5.3.2 Implementation Approach
**Option A: Simple Substitution (Recommended for XP)**
- Map common LaTeX commands to Unicode symbols
- Use GDI+ to render Unicode text with anti-aliasing
- Example: `\alpha` → `α` (U+03B1), `\sum` → `∑` (U+2211)

**Option B: Bitmap Generation (Complex)**
- Use a lightweight LaTeX-to-bitmap converter (if available for XP)
- Embed as PNG in chat flow

**Decision:** Implement Option A with a lookup table for 200+ common symbols.

```cpp
class TLaTeXEngine {
private:
    TStringList* symbolMap; // LaTeX -> Unicode
    
    AnsiString SubstituteSymbols(const AnsiString& latex);
    void RenderFraction(TCanvas* canvas, const AnsiString& num, 
                        const AnsiString& den, int x, int y);
    
public:
    __fastcall TLaTeXEngine();
    void RenderInline(TCanvas* canvas, const AnsiString& latex, 
                      int& x, int y, int lineHeight);
    void RenderDisplay(TCanvas* canvas, const AnsiString& latex, 
                       int& x, int& y);
};
```

### 5.4 Code Block Export

#### 5.4.1 Export Dialog
- Pre-fill filename based on template
- Allow user to override name and location
- Show language detection result

#### 5.4.2 Filename Template Variables
- `{model}`: Current model name
- `{timestamp}`: YYYYMMDD_HHMMSS
- `{index}`: Sequential block number in session
- `{lang}`: Detected language

#### 5.4.3 Extension Mapping
```ini
[Extensions]
cpp=.cpp
c=.c
h=.h
python=.py
js=.js
sql=.sql
txt=.txt
default=.txt
```

#### 5.4.4 Export Function
```cpp
void TCodeExporter::ExportCodeBlock(const AnsiString& code, 
                                    const AnsiString& language,
                                    const AnsiString& suggestedName) {
    dlgExport->FileName = ApplyTemplate(suggestedName, language);
    if (dlgExport->Execute()) {
        SaveToFile(dlgExport->FileName, code);
        ShowMessage("Code exported to: " + dlgExport->FileName);
    }
}
```

---

## 6. Multi-modal Support

### 6.1 Document Attachment

#### 6.1.1 Supported Formats
| Format | Extraction Method | Max Size |
|--------|------------------|----------|
| TXT | Direct read | 5MB |
| CSV | Direct read | 5MB |
| MD | Direct read | 5MB |
| RTF | TRichText load | 3MB |
| DOC | External converter (if available) or skip | 2MB |
| PDF | Text layer extraction (limited) | 2MB |

#### 6.1.2 Processing Pipeline
1. User selects file via `TOpenDialog`
2. Validate extension and size
3. Read content into memory (or stream for large files)
4. Convert to UTF-8 (BCB6 AnsiString is system locale)
5. Truncate if exceeds token limit (configurable)
6. Append to message context as system prompt or user content

```cpp
AnsiString TDocProcessor::ExtractText(const AnsiString& filePath) {
    AnsiString ext = ExtractFileExt(filePath).LowerCase();
    
    if (ext == ".txt" || ext == ".csv" || ext == ".md") {
        return ReadTextFile(filePath);
    }
    else if (ext == ".rtf") {
        TRichText* rt = new TRichText(NULL);
        try {
            rt->Lines->LoadFromFile(filePath);
            return rt->Lines->Text;
        }
        __finally {
            delete rt;
        }
    }
    else if (ext == ".pdf") {
        // Limited support: try to read raw text streams
        return ExtractPDFText(filePath); 
    }
    throw Exception("Unsupported format: " + ext);
}
```

### 6.2 Vision (Image Attachment)

#### 6.2.1 Image Processing
- Load via `TJPEGImage`, `TPNGImage`, or native `TBitmap`
- Resize to max dimension (configurable, default 1024px)
- Compress to JPEG (quality 80%) if original is large
- Encode to Base64

```cpp
AnsiString TVisionProcessor::PrepareImage(const AnsiString& filePath) {
    TGraphic* img = LoadGraphic(filePath);
    try {
        // Resize if needed
        if (img->Width > MaxDimension || img->Height > MaxDimension) {
            ResizeImage(img, MaxDimension);
        }
        
        // Save to memory stream
        TMemoryStream* stream = new TMemoryStream();
        try {
            if (filePath.LowerCase().EndsWith(".jpg") || 
                filePath.LowerCase().EndsWith(".jpeg")) {
                ((TJPEGImage*)img)->SaveToStream(stream);
            } else {
                // Convert to JPEG for smaller size
                TJPEGImage* jpeg = new TJPEGImage();
                try {
                    jpeg->Assign(img);
                    jpeg->CompressionQuality = 80;
                    jpeg->SaveToStream(stream);
                }
                __finally {
                    delete jpeg;
                }
            }
            
            // Base64 encode
            stream->Position = 0;
            return TBase64::Encode(stream);
        }
        __finally {
            delete stream;
        }
    }
    __finally {
        delete img;
    }
}
```

#### 6.2.2 API Payload Construction
```json
{
  "model": "vision-model",
  "messages": [
    {
      "role": "user",
      "content": [
        {
          "type": "text",
          "text": "What is in this image?"
        },
        {
          "type": "image_url",
          "image_url": {
            "url": "data:image/jpeg;base64,/9j/4AAQSkZJRg..."
          }
        }
      ]
    }
  ],
  "stream": true
}
```

---

## 7. Streaming Implementation Details

### 7.1 Thread Model
- **Main Thread:** UI rendering, user input
- **Worker Thread:** Network I/O, JSON parsing
- **Synchronization:** Critical sections for shared buffers, `Synchronize()` for UI updates

### 7.2 Incremental Rendering
As chunks arrive:
1. Parse JSON delta
2. Extract `choices[0].delta.content`
3. Append to internal buffer
4. Trigger `InvalidateRect()` on chat paintbox
5. PaintBox repaints entire visible area (double-buffered)

### 7.3 Stop Mechanism
- Global flag `bStopRequested`
- Worker thread checks flag after each chunk
- On stop: close socket, discard pending data, show "[Stopped]" marker

---

## 8. Data Structures

### 8.1 Message Object
```cpp
struct TMessage {
    AnsiString role; // "user", "assistant", "system"
    AnsiString content;
    TList* attachments; // List of TAttachment*
    bool isStreaming;
    TDateTime timestamp;
    
    __fastcall TMessage() {
        attachments = new TList();
    }
    
    __fastcall ~TMessage() {
        for (int i = 0; i < attachments->Count; i++) {
            delete (TAttachment*)attachments->Items[i];
        }
        delete attachments;
    }
};
```

### 8.2 Attachment Object
```cpp
struct TAttachment {
    enum Type { atDocument, atImage };
    Type type;
    AnsiString filePath;
    AnsiString mimeType;
    AnsiString base64Data; // Populated on send
    int fileSize;
    
    __fastcall TAttachment(Type t, const AnsiString& path) {
        type = t;
        filePath = path;
        fileSize = GetFileSize(path);
    }
};
```

### 8.3 Code Block Object
```cpp
struct TCodeBlock {
    AnsiString language;
    AnsiString content;
    int startIndex; // Position in full text
    int endIndex;
    TRect renderRect; // For click detection
    
    void Export(const AnsiString& folder, const AnsiString& template);
};
```

---

## 9. Configuration Storage

### 9.1 Registry Structure
```
HKEY_CURRENT_USER\Software\LMStudioClient
├── Server
│   ├── Host (string)
│   ├── Port (integer)
│   └── Timeout (integer)
├── Models
│   ├── Default (string)
│   └── LastUsed (string)
├── Export
│   ├── DefaultFolder (string)
│   ├── FilenameTemplate (string)
│   └── AutoOverwrite (boolean)
├── Rendering
│   ├── FontName (string)
│   ├── FontSize (integer)
│   ├── Theme (string)
│   ├── EnableLaTeX (boolean)
│   └── MaxImageRes (integer)
└── Advanced
    ├── MaxContextTokens (integer)
    ├── Temperature (float)
    └── StreamChunkSize (integer)
```

### 9.2 INI File Fallback
If registry access fails, use `lmstudio.ini` in application directory.

---

## 10. Error Handling & Logging

### 10.1 Error Categories
- **Network:** Connection refused, timeout, disconnect
- **Parsing:** Invalid JSON, malformed markdown
- **File:** Access denied, not found, too large
- **Rendering:** Font missing, GDI+ failure

### 10.2 Logging Mechanism
- Write to `lmstudio.log` in app directory
- Rotate log at 1MB
- Include timestamp, thread ID, error code, description

```cpp
void LogError(const AnsiString& category, const AnsiString& message) {
    TFileStream* log = OpenLogAppender();
    try {
        AnsiString entry = FormatDateTime("yyyy-mm-dd hh:nn:ss", Now()) +
                          " [" + category + "] " + message + "\r\n";
        log->Write(entry.c_str(), entry.Length());
    }
    __finally {
        delete log;
    }
}
```

---

## 11. Performance Optimization

### 11.1 Memory Management
- Reuse buffers for network I/O
- Clear chat history when exceeding 100 messages
- Use `TMemoryStream` for temporary storage
- Avoid string concatenation in loops (use `TStringList`)

### 11.2 Rendering Optimization
- Double-buffer all custom painting
- Cache syntax-highlighted code bitmaps
- Lazy-load images in chat history
- Limit visible messages to 50 (virtual scrolling if needed)

### 11.3 Network Optimization
- Use non-blocking sockets with select()
- Buffer small chunks before UI update (batch 3-5 tokens)
- Compress request bodies if server supports it

---

## 12. Security Considerations

### 12.1 File Attachment Security
- Block executable extensions (.exe, .bat, .scr, .vbs)
- Validate MIME types against file headers
- Sanitize filenames (remove path components)
- Max file size enforcement

### 12.2 Network Security
- No SSL/TLS (XP limitations, LAN-only assumption)
- Validate server IP format
- Timeout on unresponsive connections
- No credential storage (LAN trust model)

### 12.3 Code Export Security
- Warn before overwriting existing files
- Validate export path (prevent directory traversal)
- Sanitize code content (remove null bytes)

---

## 13. Testing Strategy

### 13.1 Unit Tests
- JSON parser with edge cases (empty arrays, nested objects)
- Base64 encoder/decoder round-trip
- Markdown state machine transitions
- LaTeX symbol substitution table

### 13.2 Integration Tests
- End-to-end streaming with mock server
- File attachment processing pipeline
- Code export file system operations
- Multi-message conversation context

### 13.3 Stress Tests
- 100+ message conversation
- 5MB document attachment
- Rapid start/stop streaming
- Low memory conditions (simulate with VirtualLock)

### 13.4 Compatibility Tests
- Windows XP SP2, SP3
- Different screen resolutions (800x600 to 1920x1080)
- Various LAN speeds (10Mbps to 1Gbps)

---

## 14. Deployment

### 14.1 Distribution Package
- `LMStudioClient.exe` (main application)
- `gmplus.dll` (GDI+ redistributable if needed)
- `README.txt` (installation instructions)
- `sample_config.ini` (example configuration)

### 14.2 Installation Steps
1. Copy EXE to target folder
2. Register GDI+ if not present (check OS version)
3. Create desktop shortcut
4. First-run wizard for server configuration

### 14.3 Uninstallation
- Remove EXE and INI files
- Clean registry keys
- Preserve user exports (in separate folder)

---

## 15. Appendix A: Sample Code Templates

### 15.1 WinSocket Connection
```cpp
bool THttpClient::ConnectSocket() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LogError("Network", "WSAStartup failed");
        return false;
    }
    
    hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        LogError("Network", "Socket creation failed");
        return false;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
    
    if (connect(hSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        LogError("Network", "Connection failed: " + IntToStr(WSAGetLastError()));
        DisconnectSocket();
        return false;
    }
    
    isConnected = true;
    return true;
}
```

### 15.2 Streaming JSON Parser
```cpp
void TStreamParser::ParseChunk(const AnsiString& json) {
    // Quick validation
    if (json.Pos("\"choices\"") == 0) return;
    
    // Extract delta content manually (avoid full parse)
    int start = json.Pos("\"content\":\"");
    if (start == 0) return;
    start += 11;
    
    int end = json.PosFrom(start, "\"");
    if (end == 0) return;
    
    AnsiString delta = json.SubString(start, end - start);
    delta = UnescapeJSON(delta);
    
    // Fire callback
    if (OnTokenReceived != NULL) {
        OnTokenReceived(delta);
    }
}
```

### 15.3 Syntax Highlighter
```cpp
void TSyntaxHighlighter::DrawCodeBlock(TCanvas* canvas, 
                                       const AnsiString& code,
                                       const AnsiString& language,
                                       TRect rect) {
    TStringList* lines = new TStringList();
    try {
        lines->Text = code;
        int y = rect.Top;
        int lineHeight = canvas->TextHeight("Xy");
        
        for (int i = 0; i < lines->Count; i++) {
            AnsiString line = lines->Strings[i];
            TList* tokens = TokenizeLine(line, language);
            
            int x = rect.Left + 10; // Padding
            for (int j = 0; j < tokens->Count; j++) {
                TToken* tok = (TToken*)tokens->Items[j];
                canvas->Font->Color = tok->color;
                canvas->Font->Style = tok->style;
                canvas->TextOut(x, y, tok->text);
                x += canvas->TextWidth(tok->text);
                delete tok;
            }
            delete tokens;
            y += lineHeight;
        }
    }
    __finally {
        delete lines;
    }
}
```

---

## 16. Appendix B: LaTeX Symbol Table (Partial)

| LaTeX | Unicode | Description |
|-------|---------|-------------|
| `\alpha` | α | Alpha |
| `\beta` | β | Beta |
| `\gamma` | γ | Gamma |
| `\delta` | δ | Delta |
| `\sum` | ∑ | Summation |
| `\prod` | ∏ | Product |
| `\int` | ∫ | Integral |
| `\frac{a}{b}` | a/b | Fraction (rendered specially) |
| `\sqrt{x}` | √x | Square root |
| `\infty` | ∞ | Infinity |
| `\neq` | ≠ | Not equal |
| `\leq` | ≤ | Less or equal |
| `\geq` | ≥ | Greater or equal |
| `\times` | × | Multiply |
| `\div` | ÷ | Divide |
| `\pm` | ± | Plus-minus |
| `\pi` | π | Pi |
| `\theta` | θ | Theta |
| `\lambda` | λ | Lambda |
| `\mu` | μ | Mu |
| `\sigma` | σ | Sigma |
| `\omega` | ω | Omega |
| `\Delta` | Δ | Capital Delta |
| `\Sigma` | Σ | Capital Sigma |
| `\Omega` | Ω | Capital Omega |
| `\forall` | ∀ | For all |
| `\exists` | ∃ | Exists |
| `\emptyset` | ∅ | Empty set |
| `\in` | ∈ | Element of |
| `\notin` | ∉ | Not element of |
| `\subset` | ⊂ | Subset |
| `\cup` | ∪ | Union |
| `\cap` | ∩ | Intersection |

---

## 17. Appendix C: Project File Structure

```
LMStudioClient/
├── Source/
│   ├── Main.cpp
│   ├── MainForm.cpp
│   ├── MainForm.h
│   ├── SettingsForm.cpp
│   ├── SettingsForm.h
│   ├── HttpClient.cpp
│   ├── HttpClient.h
│   ├── StreamParser.cpp
│   ├── StreamParser.h
│   ├── MarkdownRenderer.cpp
│   ├── MarkdownRenderer.h
│   ├── SyntaxHighlight.cpp
│   ├── SyntaxHighlight.h
│   ├── LaTeXEngine.cpp
│   ├── LaTeXEngine.h
│   ├── CodeExporter.cpp
│   ├── CodeExporter.h
│   ├── VisionProcessor.cpp
│   ├── VisionProcessor.h
│   ├── DocProcessor.cpp
│   ├── DocProcessor.h
│   ├── Base64.cpp
│   ├── Base64.h
│   ├── ConfigStore.cpp
│   ├── ConfigStore.h
│   └── Utils.cpp
├── Forms/
│   ├── MainForm.dfm
│   ├── SettingsForm.dfm
│   └── ExportDialog.dfm
├── Resources/
│   ├── AppIcon.ico
│   ├── SyntaxThemes.ini
│   └── LaTeXSymbols.ini
├── LMStudioClient.bpr
├── LMStudioClient.cfg
└── README.md
```

---

## 18. Conclusion

This design document provides a complete blueprint for implementing an LM Studio API client on Windows XP using Borland C++ Builder 6. By leveraging raw WinSocket for networking, GDI+ for rich rendering, and careful memory management, the application will deliver modern features (streaming, multi-modal, syntax highlighting, LaTeX) within the constraints of legacy hardware and software.

All modules are designed for immediate implementation with no phased timeline. Development can begin immediately following this specification.
