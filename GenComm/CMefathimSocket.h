#pragma once
#include <map>
#include "../IMComm/structsAndConstants.h"

#include "IMessageFactory.h"
#include "IMessage.h"
#ifdef DLL_IMPORT
#define DLL __declspec(dllimport)
#else
#define DLL __declspec(dllexport)
#endif

#include <list>

class CMefathimSocket : public CAsyncSocket
{
	
protected:
	IMessageFactory* m_pMessageFactory;// This is to create a message of specific type upon receipt of buffer
	// In this implementation we are going to simply push the mssg object into a queue
	// TODO: Change value to vector/list of function pointers
public:
	CMefathimSocket(IMessageFactory* pMessageFactory, std::string sSocketName);
	~CMefathimSocket();

	void OnMessageReceived(char pBuffer[]);// Called in OnReceive()

	virtual void OnConnect(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};