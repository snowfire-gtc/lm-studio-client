//---------------------------------------------------------------------------
// LM Studio LAN Client - Code Exporter Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CodeExporter.h"
#include "Utils.h"

//---------------------------------------------------------------------------
__fastcall TCodeExporter::TCodeExporter()
{
    SaveDialog = new TSaveDialog(NULL);
    DefaultFolder = "";
    FilenameTemplate = "{model}_{timestamp}_{index}";
}
//---------------------------------------------------------------------------
__fastcall TCodeExporter::~TCodeExporter()
{
    delete SaveDialog;
}
//---------------------------------------------------------------------------
AnsiString TCodeExporter::GetExtensionForLanguage(const AnsiString& language)
{
    if (language == "cpp" || language == "c++" || language == "c") return ".cpp";
    if (language == "h" || language == "hpp" || language == "h++") return ".h";
    if (language == "python" || language == "py") return ".py";
    if (language == "javascript" || language == "js") return ".js";
    if (language == "typescript" || language == "ts") return ".ts";
    if (language == "java") return ".java";
    if (language == "csharp" || language == "cs") return ".cs";
    if (language == "sql") return ".sql";
    if (language == "html") return ".html";
    if (language == "css") return ".css";
    if (language == "xml") return ".xml";
    if (language == "json") return ".json";
    if (language == "markdown" || language == "md") return ".md";
    if (language == "bash" || language == "sh") return ".sh";
    if (language == "powershell" || language == "ps1") return ".ps1";
    if (language == "ruby" || language == "rb") return ".rb";
    if (language == "go") return ".go";
    if (language == "rust") return ".rs";
    if (language == "php") return ".php";
    if (language == "swift") return ".swift";
    if (language == "kotlin") return ".kt";
    if (language == "scala") return ".scala";
    if (language == "r") return ".r";
    if (language == "matlab") return ".m";
    if (language == "lua") return ".lua";
    if (language == "perl") return ".pl";
    if (language == "vb") return ".vb";
    if (language == "pascal") return ".pas";
    if (language == "fortran") return ".f90";
    if (language == "cobol") return ".cob";
    if (language == "assembly" || language == "asm") return ".asm";
    
    return ".txt"; // Default
}
//---------------------------------------------------------------------------
AnsiString TCodeExporter::ApplyTemplate(const AnsiString& template, 
                                        const AnsiString& language,
                                        const AnsiString& model)
{
    AnsiString result = template;
    
    // Replace {model}
    result = ReplaceAll(result, "{model}", model);
    
    // Replace {timestamp}
    result = ReplaceAll(result, "{timestamp}", GetTimestamp());
    
    // Replace {lang}
    result = ReplaceAll(result, "{lang}", language);
    
    // Replace {index} with sequential number (simplified - just use 1)
    result = ReplaceAll(result, "{index}", "1");
    
    // Add extension if not present
    AnsiString ext = GetExtensionForLanguage(language);
    if (result.Pos(ext) == 0)
        result += ext;
    
    return result;
}
//---------------------------------------------------------------------------
bool TCodeExporter::ExportCodeBlock(const AnsiString& code, 
                                    const AnsiString& language,
                                    const AnsiString& suggestedName)
{
    try
    {
        // Set initial directory
        if (!DefaultFolder.IsEmpty() && DirectoryExists(DefaultFolder))
            SaveDialog->InitialDir = DefaultFolder;
        
        // Apply template to get filename
        SaveDialog->FileName = ApplyTemplate(FilenameTemplate, language, "local-model");
        
        // Set filter based on language
        AnsiString ext = GetExtensionForLanguage(language);
        SaveDialog->Filter = "*" + ext + " files|*" + ext + "|All files|*.*";
        
        if (SaveDialog->Execute())
        {
            WriteTextFile(SaveDialog->FileName, code);
            ShowMessage("Code exported to: " + SaveDialog->FileName);
            return true;
        }
        
        return false;
    }
    catch (Exception& e)
    {
        ShowMessage("Error exporting code: " + e.Message);
        LogError("Export", e.Message);
        return false;
    }
}
//---------------------------------------------------------------------------
