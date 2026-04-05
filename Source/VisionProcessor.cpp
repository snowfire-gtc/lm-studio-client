//---------------------------------------------------------------------------
// LM Studio LAN Client - Vision/Image Processor Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VisionProcessor.h"
#include "Utils.h"
#include <jpeg.hpp>

//---------------------------------------------------------------------------
__fastcall TVisionProcessor::TVisionProcessor()
{
    MaxDimension = 1024;
    CompressionQuality = 80;
}
//---------------------------------------------------------------------------
AnsiString TVisionProcessor::PrepareImage(const AnsiString& filePath)
{
    TGraphic* img = LoadGraphic(filePath);
    try
    {
        // Resize if needed
        if (img->Width > MaxDimension || img->Height > MaxDimension)
        {
            ResizeImage(img, MaxDimension);
        }
        
        // Save to memory stream
        TMemoryStream* stream = new TMemoryStream();
        try
        {
            AnsiString ext = ExtractFileExt(filePath).LowerCase();
            
            if (ext == ".jpg" || ext == ".jpeg")
            {
                TJPEGImage* jpeg = dynamic_cast<TJPEGImage*>(img);
                if (jpeg)
                {
                    jpeg->CompressionQuality = CompressionQuality;
                    jpeg->SaveToStream(stream);
                }
                else
                {
                    // Convert to JPEG
                    TJPEGImage* newJpeg = new TJPEGImage();
                    try
                    {
                        newJpeg->Assign(img);
                        newJpeg->CompressionQuality = CompressionQuality;
                        newJpeg->SaveToStream(stream);
                    }
                    __finally
                    {
                        delete newJpeg;
                    }
                }
            }
            else if (ext == ".png")
            {
                img->SaveToStream(stream);
            }
            else
            {
                // Convert to JPEG for other formats
                TJPEGImage* jpeg = new TJPEGImage();
                try
                {
                    jpeg->Assign(img);
                    jpeg->CompressionQuality = CompressionQuality;
                    jpeg->SaveToStream(stream);
                }
                __finally
                {
                    delete jpeg;
                }
            }
            
            // Base64 encode
            stream->Position = 0;
            return TBase64::Encode(stream);
        }
        __finally
        {
            delete stream;
        }
    }
    __finally
    {
        delete img;
    }
}
//---------------------------------------------------------------------------
