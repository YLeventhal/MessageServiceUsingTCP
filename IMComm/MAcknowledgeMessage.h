#pragma once
#include "structsAndConstants.h"
#include "structsAndConstants.cpp"
#include "../GenComm/IMessage.h"

class MAcknowledgeMessage :
	public IMessage
{
private:
	int m_guidOriginalMessage;
	std::string ackMessage;
public:
	MAcknowledgeMessage();
	// Consider also calling IMessage's non-default constructor and passing it arguments, like i did in MTextMessage.h
	MAcknowledgeMessage(int guid, int guidOriginalMessage);
   ~MAcknowledgeMessage();

   const int GetOriginalMessageGUID() { return m_guidOriginalMessage; }
   const std::string& GetAckMessage() { return ackMessage; }

   virtual int Size();
   virtual bool ToBuffer(char* cBuffer);
   virtual bool FromBuffer(char* pBuffer);
};


