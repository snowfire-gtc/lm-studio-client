//---------------------------------------------------------------------------
// LM Studio LAN Client - HTTP Client using WinSocket (Raw API)
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#ifndef HttpClientH
#define HttpClientH

#include <Classes.hpp>
#include <winsock2.h>

// Callback type for streaming responses
typedef void (__fastcall *TStreamCallback)(const AnsiString& jsonChunk);

class THttpClient
{
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
    
    void Stop();
    bool GetIsConnected() { return isConnected; }
};

#endif
