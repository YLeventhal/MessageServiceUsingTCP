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
	size += ackMessage.length();

	return size;
}


bool MAcknowledgeMessage::ToBuffer(char* cBuffer)
{
	*((int*)cBuffer) = m_guid;
	*((int*)(cBuffer + SIZE_GUID)) = m_nMessageType;
	*((int*)(cBuffer + SIZE_GUID + SIZE_INT)) = m_guidOriginalMessage;

	std::string response = "Got Your Message!!";
	int responseLength = response.length();

	// Add size of response to the buffer to indicate how many bytes to extract
	*((int*)(cBuffer + SIZE_GUID + SIZE_INT + SIZE_INT)) = responseLength;

	cBuffer = (cBuffer + SIZE_GUID + SIZE_INT + SIZE_INT + SIZE_INT);
	// When extracting from the buffer need to add \0 to end of extracted chars
	response.copy(cBuffer, responseLength);

	return true;
}



bool MAcknowledgeMessage::FromBuffer(char* pBuffer)
{
	int i;
	int responseLength;
	char* tempString;

	m_guid = *((int*)pBuffer);
	m_nMessageType = *((int*)(pBuffer + SIZE_GUID));
	m_guidOriginalMessage = *((int*)(pBuffer + SIZE_GUID + SIZE_INT));

	responseLength = *((int*)(pBuffer + SIZE_GUID + SIZE_INT + SIZE_INT));

	// Update pointer to buffer
	pBuffer = (pBuffer + SIZE_GUID + SIZE_INT + SIZE_INT + SIZE_INT);

	tempString = new char[responseLength + 1];
	for (i = 0; i < responseLength; i++)
	{
		tempString[i] = *(pBuffer + i);
	}
	tempString[responseLength] = '\0';
	ackMessage = tempString;
	delete tempString;

	return true;
}



