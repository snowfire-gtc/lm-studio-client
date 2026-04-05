//---------------------------------------------------------------------------
// LM Studio LAN Client - Vision/Image Processor
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef VisionProcessorH
#define VisionProcessorH

#include <Classes.hpp>
#include <Graphics.hpp>
#include "Base64.h"

class TVisionProcessor
{
private:
    int MaxDimension;
    int CompressionQuality;
    
public:
    __fastcall TVisionProcessor();
    
    AnsiString PrepareImage(const AnsiString& filePath);
    void SetMaxDimension(int dim) { MaxDimension = dim; }
    void SetCompressionQuality(int quality) { CompressionQuality = quality; }
};

#endif
