# LM Studio LAN Client

A lightweight LM Studio API client designed for Windows XP using Borland C++ Builder 6.

## Features

- **Real-time Streaming**: Token-by-token response display
- **Multi-modal Support**: Text, document attachments (TXT, CSV, MD, RTF, PDF), and images
- **Rich Output**: 
  - Markdown parsing with syntax highlighting
  - LaTeX math rendering ($...$ and $$...$$)
  - Code block export to disk
- **Vision**: Image analysis via base64 encoding
- **Document Processing**: Text extraction from various formats

## Requirements

- Windows XP SP3
- Borland C++ Builder 6
- GDI+ (included in Windows XP SP3)
- Network access to LM Studio server on LAN

## Building

1. Open `LMStudioClient.bpf` in Borland C++ Builder 6
2. Build the project (Ctrl+F9)
3. The executable will be created in the `Build` folder

## Configuration

### Server Settings
- **Host**: LM Studio server IP address (default: 127.0.0.1)
- **Port**: LM Studio server port (default: 1234)
- **Timeout**: Connection timeout in seconds (default: 30)

### Export Settings
- **Default Folder**: Where code exports are saved
- **Filename Template**: `{model}_{timestamp}_{index}`

### Rendering Settings
- **Font Name**: Code block font
- **Font Size**: Base font size
- **Enable LaTeX**: Toggle math rendering
- **Max Image Resolution**: Maximum dimension for image uploads

## Supported Languages for Syntax Highlighting

- C/C++
- Python
- SQL
- JavaScript
- Java
- C#
- HTML/CSS
- And many more...

## Supported LaTeX Symbols

Greek letters, operators, logic symbols, arrows, and mathematical notation. See `LaTeXEngine.cpp` for the complete list.

## Project Structure

```
LMStudioClient/
├── Source/              # C++ source files
│   ├── Main.cpp        # Application entry point
│   ├── MainForm.*      # Main chat window
│   ├── SettingsForm.*  # Settings dialog
│   ├── HttpClient.*    # WinSocket HTTP client
│   ├── StreamParser.*  # JSON streaming parser
│   ├── MarkdownRenderer.* # Markdown parsing & rendering
│   ├── SyntaxHighlight.*  # Code syntax highlighting
│   ├── LaTeXEngine.*   # LaTeX to Unicode conversion
│   ├── CodeExporter.*  # Code block export
│   ├── VisionProcessor.* # Image processing
│   ├── DocProcessor.*  # Document text extraction
│   ├── Base64.*        # Base64 encoding
│   ├── ConfigStore.*   # Registry/INI configuration
│   └── Utils.*         # Utility functions
├── Forms/              # VCL form definitions (.dfm)
├── Resources/          # Icons and other resources
├── Build/              # Compiled output
└── LMStudioClient.bpf  # Project file
```

## Usage

1. Launch the application
2. Configure server settings in Settings dialog
3. Select model from dropdown
4. Type your message in the input area
5. Optionally attach files or images
6. Click Send to start the conversation
7. Responses stream in real-time with syntax highlighting
8. Click "Export Code" buttons to save code blocks

## License

This project is provided as-is for educational and legacy system support purposes.

## Acknowledgments

- LM Studio team for the API
- Borland/Embarcadero for C++ Builder
- Windows XP community for keeping legacy systems alive
