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

#include "CCommunication_Client.h"


//CCommunication_Client::CCommunication_Client(){}
CCommunication_Client* CCommunication_Client::s_pCCommunicationClient = nullptr;


// Hard coded cient number but should be dynamic
CCommunication_Client::CCommunication_Client()
{
	m_sSocketName = "CLIENT";
	m_pMessageFactory = new CMessageFactory_WhatsApp();
	Register();
}

CCommunication_Client::~CCommunication_Client()
{
	delete m_pMessageFactory;
	this->Close();
}

void CCommunication_Client::OnTextMessageReceived(IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueTextMessages.Push((MTextMessage*)pMessage);
}

/*void CCommunication_Client::OnGroupCreateUpdateReceived(IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueGroupCreateUpdateMessages.Push((MGroupCreateUpdate*)pMessage);
}

void CCommunication_Client::OnAcknowledgeReceived( IMessage* pMessage)
{
	CCommunication_Client::GetInstance()->m_queueAcknowledge.Push((MAcknowledgeMessage*)pMessage);
}*/

/*void CMefathimSocket::RegisterCallback(EMessageType eMessageType, void* pfnCallback)
{
	m_hashCallbacks.insert(std::pair<EMessageType, void*>(eMessageType, pfnCallback));
}*/

/*std::map<EMessageType, void*> m_hashCallbacks;
void CMefathimSocket::RegisterCallback(EMessageType eMessageType, void(*pfnCallback)(IMessage*))
{
	m_hashCallbacks.insert(std::pair<EMessageType, void(*)(IMessage*)>(eMessageType, pfnCallback));
}*/
void CCommunication_Client::Register()
{
	//void(*fptr)(IMessage*) = this->OnTextMessageReceived;
	//since RegisterCallback is an inherited method, using "this->" makes it clear that this method exists in this object(even though it is technically unnecessary)
	this->RegisterCallback(TEXT_MESSAGE, OnTextMessageReceived);// &(this->OnTextMessageReceived));
	//this->RegisterCallback(EMessageType::CREATE_UPDATE_GROUP, CCommunication_Client::GetInstance()->OnGroupCreateUpdateReceived);
	//this->RegisterCallback(EMessageType::ACKNOWLEDGE, CCommunication_Client::GetInstance()->OnAcknowledgeReceived);
}



void CCommunication_Client::HandleIncomingMessages()
{
	if (!(GetTextMessagesQueue().Empty()))
	{
		MTextMessage* pMessageToHandle = NULL;  // pointer to text message obj
		pMessageToHandle = dynamic_cast<MTextMessage*>(GetTextMessagesQueue().Pop());
		
		
		std::string text = pMessageToHandle->GetTextMessage().m_sText;
		std::string sender = pMessageToHandle->GetSendingSocketName();
		CString textMessage(text.c_str());
		::AfxMessageBox(textMessage);

		CString sentFrom(sender.c_str());
		::AfxMessageBox(textMessage);


		Sleep(5000);
		// while runs if test is not 0 even if value is negative; a pointer holds a number ie. address, and so long as it is not null it evals to true
		// This leaves me with an Imessage obj (if the queue wasn't empty). We then dynamic cast it to a text message
		//while (!(GetTextMessagesQueue().Empty()))
		//while (pMessageToHandle = dynamic_cast<MTextMessage*>(CCommunication_Client::GetInstance()->GetTextMessagesQueue().Pop()))
		//{
			// Update window (and DB); In Primitive version just post a message box with the text received
			//OnTextMessageReceived((dynamic_cast<MTextMessage*>pMessageToHandle)->GetTextMessage()));
	}
}

// Tick function for main to call
void CCommunication_Client::Tick()
{
	this->HandleIncomingMessages();
}



//void CCommunication_Client::SendGroupCreateUpdate(const TGroup& group) {};
//void CCommunication_Client::SendAck(const TTextMessage& textMessageToAck) {};
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

	Sleep(10000);

	TTextMessage text;
	text.m_sText = ("test text");
	text.m_userDestination.guid = 17;
	text.m_userDestination.sName = ("dave");
	text.m_userDestination.sPhoneNumber = ("058");
	text.m_groupDestination.guid = 12;
	//AfxMessageBox(L"about to send text");
	SendTextMessage(text);

}


void CCommunication_Client::OnMessageReceived(char pBuffer[])
{
	// Get message type from buffer (notice that in all messsages, first two
	// members are the GUID (in this implementation its an int and SIZE_GUID=sizeof(int)) and then type (see IMessage class) 
	//ie. this will move pointer over to point at type variable in buffer array. 
	//This explains the folowing line:
	EMessageType type = *((EMessageType*)(pBuffer + SIZE_GUID));//move pointer over till reach type;cast to pointer to enum;get content of pointer
	// 1. Create Message object by the type.
	IMessage* pMessage = m_pMessageFactory->CreateMessage(type); // 'pMessage' : Message obj
	pMessage->FromBuffer(pBuffer);// Calling mssg obj.'s FromBuffer method which Fills the message obj.'s fields 
	// 2. Call callback
	void* callbacks = m_hashCallbacks[type]; // returns a pointer to a function
	((void(*)(IMessage*))callbacks)(pMessage);
}

//Called by the framework to notify this socket that there is data in 
//the buffer that can be retrieved by calling the Receive() member function.
void CCommunication_Client::OnReceive(int nErrorCode)
{
	CString Ca(this->m_sSocketName.c_str());
	//AfxMessageBox(Ca);
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
	//TODO: Post buffer to message box
	// Call onMessageReceived() to get the message object and call its callbac
	OnMessageReceived(arrBuffer);

	::AfxMessageBox(L"Lets handle the message...calling handleIncomingMessage()");
	this->Tick();
}


void CCommunication_Client::OnClose(int nErrorCode)
{
	AfxMessageBox(L"Wow - connection closed...");
}