//---------------------------------------------------------------------------
// LM Studio LAN Client - HTTP Client using WinSocket Implementation
// Windows XP / Borland C++ Builder 6
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HttpClient.h"
#include "Utils.h"

#define BUFFER_SIZE 4096

static bool bStopRequested = false;

//---------------------------------------------------------------------------
__fastcall THttpClient::THttpClient(const AnsiString& host, int port)
{
    hSocket = INVALID_SOCKET;
    this->host = host;
    this->port = port;
    isConnected = false;
}
//---------------------------------------------------------------------------
__fastcall THttpClient::~THttpClient()
{
    DisconnectSocket();
}
//---------------------------------------------------------------------------
bool THttpClient::ConnectSocket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        LogError("Network", "WSAStartup failed");
        return false;
    }
    
    hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET)
    {
        LogError("Network", "Socket creation failed");
        return false;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
    
    // Set timeout
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    
    if (connect(hSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        LogError("Network", "Connection failed: " + IntToStr(WSAGetLastError()));
        DisconnectSocket();
        return false;
    }
    
    isConnected = true;
    return true;
}
//---------------------------------------------------------------------------
void THttpClient::DisconnectSocket()
{
    if (hSocket != INVALID_SOCKET)
    {
        closesocket(hSocket);
        hSocket = INVALID_SOCKET;
    }
    isConnected = false;
    WSACleanup();
}
//---------------------------------------------------------------------------
AnsiString THttpClient::BuildHttpRequest(const AnsiString& method, 
                                         const AnsiString& path,
                                         const AnsiString& body,
                                         bool isStreaming)
{
    AnsiString request = "";
    request += method + " " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + ":" + IntToStr(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    
    if (isStreaming)
    {
        request += "Accept: text/event-stream\r\n";
        request += "Cache-Control: no-cache\r\n";
    }
    
    request += "Content-Length: " + IntToStr(body.Length()) + "\r\n";
    request += "Connection: keep-alive\r\n";
    request += "\r\n";
    request += body;
    
    return request;
}
//---------------------------------------------------------------------------
AnsiString THttpClient::ReadResponse(bool streaming, TStreamCallback callback)
{
    char buffer[BUFFER_SIZE];
    AnsiString responseBuffer = "";
    AnsiString lineBuffer = "";
    
    while (true)
    {
        if (bStopRequested)
        {
            break;
        }
        
        int bytes = recv(hSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0)
        {
            break;
        }
        
        buffer[bytes] = '\0';
        responseBuffer += AnsiString(buffer);
        
        if (streaming && callback != NULL)
        {
            // Process SSE format
            for (int i = 1; i <= bytes; i++)
            {
                char c = buffer[i - 1];
                if (c == '\n')
                {
                    if (lineBuffer.Pos("data: ") == 1)
                    {
                        AnsiString jsonData = lineBuffer.SubString(7, lineBuffer.Length() - 6);
                        if (jsonData == "[DONE]")
                        {
                            return responseBuffer;
                        }
                        if (!jsonData.IsEmpty())
                        {
                            callback(jsonData);
                        }
                    }
                    lineBuffer = "";
                }
                else if (c != '\r')
                {
                    lineBuffer += c;
                }
            }
        }
    }
    
    return responseBuffer;
}
//---------------------------------------------------------------------------
bool THttpClient::SendRequest(const AnsiString& endpoint, 
                              const AnsiString& jsonBody,
                              bool streaming,
                              TStreamCallback onChunk)
{
    bStopRequested = false;
    
    if (!ConnectSocket())
    {
        return false;
    }
    
    AnsiString request = BuildHttpRequest("POST", endpoint, jsonBody, streaming);
    
    int sent = send(hSocket, request.c_str(), request.Length(), 0);
    if (sent == SOCKET_ERROR)
    {
        LogError("Network", "Send failed: " + IntToStr(WSAGetLastError()));
        DisconnectSocket();
        return false;
    }
    
    ReadResponse(streaming, onChunk);
    DisconnectSocket();
    
    return !bStopRequested;
}
//---------------------------------------------------------------------------
void THttpClient::Stop()
{
    bStopRequested = true;
    DisconnectSocket();
}
//---------------------------------------------------------------------------
