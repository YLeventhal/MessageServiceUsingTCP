#include "stdafx.h"
#include "afxsock.h"
#include <rpc.h>
#include <atlstr.h>
#include <string>
#include "../GenComm/constants.h"
#include "../GenComm/IMessageFactory.h"
#include "../IMComm/CMessageFactory_WhatsApp.h"
#include "../IMComm/structsAndConstants.h"
#include "../GenComm/IMessage.h"
#include "../IMComm/MTextMessage.h"
#include "../IMComm/MGroupCreateUpdate.h"
#include "../IMComm/MAcknowledgeMessage.h"
#include "Cdiag.h"

#include "CCommunication_Client.h"


//CCommunication_Client::CCommunication_Client(){}
CCommunication_Client* CCommunication_Client::s_pCCommunicationClient = nullptr;


// Hard coded cient number but should be dynamic
CCommunication_Client::CCommunication_Client()
{
	int a = (rand() % 10);
	m_sSocketName = "CLIENT" + a;
	m_pMessageFactory = new CMessageFactory_WhatsApp();
	Register();

	//Debugging
	CString sName(this->m_sSocketName.c_str());
	::AfxMessageBox(sName + L" constructed");
}

CCommunication_Client::~CCommunication_Client()
{
	//Debugging
	CString sName(this->m_sSocketName.c_str());
	::AfxMessageBox(sName + L" destroyed");


	delete m_pMessageFactory;
	this->Close();
}


// Callbacks, on receipt of messages. Pushes message object to a message queue. 
void CCommunication_Client::OnTextMessageReceived(IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueTextMessages.Push((MTextMessage*)pMessage);
}

/*void CCommunication_Client::OnGroupCreateUpdateReceived(IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueGroupCreateUpdateMessages.Push((MGroupCreateUpdate*)pMessage);
}*/

void CCommunication_Client::OnAcknowledgeReceived( IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueAcknowledge.Push((MAcknowledgeMessage*)pMessage);
}


// This wrapper method is called in the CCommunication_client's constructor, filling the callback map
void CCommunication_Client::Register()
{
	RegisterCallback(TEXT_MESSAGE, OnTextMessageReceived);
	//RegisterCallback(EMessageType::CREATE_UPDATE_GROUP, CCommunication_Client::GetInstance()->OnGroupCreateUpdateReceived);
	RegisterCallback(ACKNOWLEDGE, OnAcknowledgeReceived);
}

// Actual registration of callbacks to map
void CCommunication_Client::RegisterCallback(EMessageType eMessageType, void* pfnCallback)// (*pfnCallback)(IMessage*))
{
	//m_hashCallbacks.insert({ eMessageType, pfnCallback });
	m_hashCallbacks[eMessageType] = pfnCallback;
	//m_hashCallbacks.insert(std::pair<EMessageType, void*>/*(*)(IMessage*)>*/(eMessageType, pfnCallback));
}

void CCommunication_Client::RemoveCallback(EMessageType eMessageType)
{
	m_hashCallbacks.erase(eMessageType);
}


// Pulls message objects from the respective queue and posts reletvant fields to the window
void CCommunication_Client::HandleIncomingMessages()
{
	if (!(GetTextMessagesQueue().Empty()))
	{
		//Debugging
		::AfxMessageBox(L"popping from message queue");

		MTextMessage* pMessageToHandle ;  // pointer to text message obj
		pMessageToHandle = dynamic_cast<MTextMessage*>(GetTextMessagesQueue().Pop());
		
		
		std::string text = pMessageToHandle->GetTextMessage().m_sText;
		std::string sender = pMessageToHandle->GetSendingSocketName();
		CString textMessage(text.c_str());
		::AfxMessageBox(textMessage);

		CString sentFrom(sender.c_str());
		::AfxMessageBox(sentFrom);


		//Sleep(5000);

		//Debugging
		CString sName(this->m_sSocketName.c_str());
		::AfxMessageBox(sName + L" sendicg Ack");

		// Send Acknowledgement of received and read message
		SendAck(*pMessageToHandle);


		// while runs if test is not 0 even if value is negative; a pointer holds a number ie. address, and so long as it is not null it evals to true
		// This leaves me with an Imessage obj (if the queue wasn't empty). We then dynamic cast it to a text message
		//while (!(GetTextMessagesQueue().Empty()))
		//while (pMessageToHandle = dynamic_cast<MTextMessage*>(CCommunication_Client::GetInstance()->GetTextMessagesQueue().Pop()))
		//{
			// Update window (and DB); In Primitive version just post a message box with the text received
			//OnTextMessageReceived((dynamic_cast<MTextMessage*>pMessageToHandle)->GetTextMessage()));
	}
	if (!(GetAcknowledgeMessagesQueue().Empty()))
	{	
		//Debugging
		CString sName(this->m_sSocketName.c_str());
		::AfxMessageBox(sName + "popping from Ack queue");

		MAcknowledgeMessage* pAckToHandle = dynamic_cast<MAcknowledgeMessage*>(GetAcknowledgeMessagesQueue().Pop());

		// Post GUID of original message to the window
		CString msg1;
		msg1.Format(_T("%d"), pAckToHandle->GetOriginalMessageGUID());
		AfxMessageBox(msg1); 

		// Post acknowledgement message to the window
		CString textMessage((pAckToHandle->GetAckMessage()).c_str());
		::AfxMessageBox(textMessage);
	}
}

// Tick function for main to call
void CCommunication_Client::Tick()
{
	this->HandleIncomingMessages();
}


// Creates acknowledgement message object, has it fill a buffer and sends the buffer
void CCommunication_Client::SendAck(const MTextMessage& textMessageToAck)
{
	int receivedMessageGuid = textMessageToAck.GetGuid();

	//Create a message object to fill and call its ToBuffer() method
	MAcknowledgeMessage* pAckMessage = new MAcknowledgeMessage(5779, receivedMessageGuid);
	// Buffer to hold message object details which are being sent 
	char cBuffer[100];
	// Filling the Buffer with the message objects details
	pAckMessage->ToBuffer(cBuffer);

	//Debugging
	CString sName(this->m_sSocketName.c_str());
	::AfxMessageBox(sName + L" really sending Ack");

	//Sending the Buffer to server
	this->Send(cBuffer, 100);

};

//void CCommunication_Client::SendGroupCreateUpdate(const TGroup& group) {};
/*int CCommunication_Client::SendTextMessage(const TTextMessage& text)
{
	// Buffer to hold message details which are being sent
	char cBuffer[100];
	//Create a message object to fill and call its ToBuffer() method
	MTextMessage* pMTextmessage = new MTextMessage(1, text);
	// Filling the Buffer with the message objects details
	pMTextmessage->ToBuffer(cBuffer);
	//Sending the Buffer to server
	int nRet = CAsyncSocket::Send(cBuffer, sizeof(cBuffer));
	//TODO: define meaninful values for return values
	return nRet;
}*/

void CCommunication_Client::SendTextMessage(const TTextMessage& text)
{
	//Create a message object to fill and call its ToBuffer() method
	MTextMessage* pMTextmessage = new MTextMessage(613, text);
	// Buffer to hold message object details which are being sent 
	char cBuffer[100];
	// Filling the Buffer with the message objects details
	pMTextmessage->ToBuffer(cBuffer);
	//Sending the Buffer to server
	this->Send(cBuffer, 100);
};

// This function is called when the client is connected (V):
void CCommunication_Client::OnConnect(int nErrorCode)
{
	CString sName(this->m_sSocketName.c_str());
	::AfxMessageBox(sName + L" is connected to the server");
	
	// Hardcoded message to send. In real implementation should post
	// an input box
	/*
	TTextMessage text;
	text.m_sText = ("test text");
	text.m_userDestination.guid = 17;
	text.m_userDestination.sName = ("dave");
	text.m_userDestination.sPhoneNumber = ("058");
	text.m_groupDestination.guid = 12;
	*/
	

	//SendTextMessage(text);
}


void CCommunication_Client::OnMessageReceived(char pBuffer[])
{
	// Get message type from buffer (notice that in all messsages, first two
	// members are the GUID (in this implementation its an int and SIZE_GUID=sizeof(int)) and then type (see IMessage class) 
	//ie. this will move pointer over to point at type variable in buffer array. 
	//This explains the folowing line:
	//Move pointer over till reach type;cast to pointer to enum;get content of pointer
	EMessageType type = *((EMessageType*)(pBuffer + SIZE_GUID));
	// 1. Create Message object by the type.
	IMessage* pMessage = m_pMessageFactory->CreateMessage(type); 
	// Calling mssg obj.'s FromBuffer method which Fills the message obj.'s fields 
	pMessage->FromBuffer(pBuffer);
	// 2. Call callback
	void* callbacks = m_hashCallbacks[type]; 
	((void(*)(IMessage*))callbacks)(pMessage);
}

//Called by the framework to notify this socket that there is data in 
//the buffer that can be retrieved by calling the Receive() member function.
void CCommunication_Client::OnReceive(int nErrorCode)
{
	//Debugging
	CString Ca(this->m_sSocketName.c_str());
	::AfxMessageBox(L"text message received by " + Ca);

	// Create a buffer to received the message:
	const int RECEIVE_BUFFER_SIZE = 100;
	char arrBuffer[RECEIVE_BUFFER_SIZE] = { 0 };
	// Receive the message:
	int nNumBytesReceived = CAsyncSocket::Receive(arrBuffer, RECEIVE_BUFFER_SIZE);
	// - If error code returned, do not continue:
	if (nNumBytesReceived == SOCKET_ERROR || nNumBytesReceived == 0)
	{
		return;
	}

	// Call onMessageReceived() to extract the message object and call its callback
	OnMessageReceived(arrBuffer);

	//Debugging
	::AfxMessageBox(Ca + L" calling handleIncomingMessage()");
	this->Tick();
}


void CCommunication_Client::OnClose(int nErrorCode)
{
	AfxMessageBox(L"Connection closed!!");
}