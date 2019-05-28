#include "stdafx.h"
//#include <rpc.h>
#include <string>
#include "../GenComm/constants.h"
#include "../GenComm/IMessage.h"
#include "structsAndConstants.h"

#include "MAcknowledgeMessage.h"


MAcknowledgeMessage::MAcknowledgeMessage()
{
}

MAcknowledgeMessage::MAcknowledgeMessage(int guid, int guidOriginalMessage): IMessage(guid, ACKNOWLEDGE)
{
	m_guidOriginalMessage = guidOriginalMessage;
}


MAcknowledgeMessage::~MAcknowledgeMessage()
{
}


int MAcknowledgeMessage::Size()
{
	int size = 0;

	size += (2 * SIZE_GUID);// GUID of this message and of text message that is being acknowledged
	size += SIZE_INT;

	return size;
}

bool MAcknowledgeMessage::ToBuffer(char* cBuffer)
{
	*((int*)cBuffer) = m_guid;
	*((int*)(cBuffer + SIZE_GUID)) = m_nMessageType;
	*((int*)(cBuffer + SIZE_GUID + SIZE_INT)) = m_guidOriginalMessage;

	return cBuffer;
}

bool MAcknowledgeMessage::FromBuffer(char* pBuffer)
{
	m_guid = *((int*)pBuffer);
	m_nMessageType = *((int*)(pBuffer + SIZE_GUID));
	m_guidOriginalMessage = *((int*)(pBuffer + SIZE_GUID + SIZE_INT));

	return true;
}



