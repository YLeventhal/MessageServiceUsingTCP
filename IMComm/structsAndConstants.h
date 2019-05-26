#pragma once
#include "../GenComm/Constants.h"
#include <atlstr.h>
#include <rpc.h>
#include <vector>
#include <string>

/* //Had linking problems with STringToBuffer()

char* StringToBuffer(char* pBuffer, CString str)
{
	memcpy(pBuffer, str.GetBuffer(), str.GetLength() * 2);
	pBuffer = pBuffer + (str.GetLength() * 2);
	return pBuffer;
}


// Represents one user
struct TUser 
{
	int guid; // "Global unique id"  a unique id that identifies this user globally
	CString sName;
	CString sPhoneNumber;
	char* ToBuffer(char* pBuffer)
	{
		//char* temp;

		*(int*)pBuffer = guid;
		pBuffer = StringToBuffer((pBuffer + sizeof(int)), sName);
		pBuffer = StringToBuffer(pBuffer, sPhoneNumber);

		return pBuffer;

		//memcpy((pBuffer + sizeof(int)), sName.GetBuffer(), sName.GetLength() * 2);
		//memcpy((pBuffer + sizeof(int)+(sName.GetLength() * 2)), sPhoneNumber.GetBuffer(), sPhoneNumber.GetLength() * 2);
		//pBuffer = (pBuffer + sizeof(int) + (sName.GetLength() * 2) + (sPhoneNumber.GetLength() * 2));

		//*(CString*)(pBuffer + sizeof(int)) = sName;
		//*(CString*)(pBuffer + sizeof(int) + sizeof(sName)) = sPhoneNumber;
		//pBuffer = (pBuffer + sizeof(int) + sizeof(sName) + sizeof(sPhoneNumber));
	}
};


// Represents a list of users
struct TGroup 
{
	int guid;
	// Simple implementation: For every change in group we send all numbers.
	std::vector<TUser> nlistUsers;
	char* ToBuffer(char* pBuffer)
	{
		//char* p_temp = pBuffer;
		*(int*)pBuffer = guid;
		pBuffer = (pBuffer + sizeof(int));
		for (auto it = nlistUsers.begin(); it != nlistUsers.end(); ++it)
		{
			pBuffer = (it->ToBuffer(pBuffer));
		}
		return pBuffer;
	}
};


// Represents one message
struct TTextMessage 
{
	CString m_sText; // message to send
	TUser m_userDestination; // to which user we are sending, in event we are sending to individual user/s
	TGroup m_groupDestination; // to which group, in event where sending to whole group 
	void ToBuffer(char* pBuffer)
	{

		// CString is not a primitive type and therefor we can't typecast and simply copy, rather we must access the 
		//buffer containing the CString; the string is in unicode and therefor is alloted 2 bytes for each letter;
		// GetBuffer() returns the number of letters and therfor we must multi by 2 to get the bytes.
		pBuffer = StringToBuffer(pBuffer , m_sText);
		pBuffer = m_userDestination.ToBuffer(pBuffer);
		m_groupDestination.ToBuffer(pBuffer);

		//memcpy(pBuffer,m_sText.GetBuffer(), m_sText.GetLength()*2);
		//pBuffer = pBuffer + (m_sText.GetLength() * 2);
	}
};
*/

//static const int SIZE_GUID = sizeof(int);
//static const int SIZE_INT = sizeof(int);


struct TUser
{
	int guid; // "Global unique id"  a unique id that identifies this user globally
	std::string sName;
	std::string sPhoneNumber;
	char* ToBuffer(char* pBuffer)
	{
		int nameLength = sName.length();// Returns # of BYTES used(1 for each char) not inc null terminator
		int phoneNumberLength = sPhoneNumber.length();//

		*((int*)pBuffer) = SIZE_GUID;
		*((int*)(pBuffer + SIZE_INT)) = guid;
		*((int*)(pBuffer + SIZE_INT + SIZE_GUID)) = nameLength;

		pBuffer = (pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT);
		sName.copy(pBuffer, nameLength);// When extracting from the buffer need to add \0 to end of extracted chars
		
		*((int*)(pBuffer + nameLength)) = phoneNumberLength;

		pBuffer = (pBuffer + nameLength + SIZE_INT);
		sPhoneNumber.copy(pBuffer, phoneNumberLength);
		
		pBuffer = (pBuffer + phoneNumberLength);

		return pBuffer;
	}

	char* FromBuffer(char* pBuffer)
	{
		// Variables to store sizes of the data entered into the buffer
		int sizeOfGuid;
		int sizeOfName;
		int sizeOfPhoneNum;
		// Used when looping over the buffer to store the data that's read, and then converting to a string type
		char* tempString;

		// Extract the # of BYTEs of the guid
		sizeOfGuid = *((int*)pBuffer);
		guid = *((int*)(pBuffer + SIZE_INT));
		// Extract the # of BYTEs of the name
		sizeOfName = *((int*)(pBuffer + SIZE_INT + sizeOfGuid ));
		// Move pointer over by the # of BYTEs we have already read from the buffer
		pBuffer = (pBuffer + SIZE_INT + sizeOfGuid  + sizeOfName);

		// Loop over the buffer to extract the name field
		tempString = new char[sizeOfName + 1];
		for (int i = 0; i < sizeOfName; i++)
		{
			tempString[i] = *(pBuffer + i);
		}
		tempString[sizeOfName] = '\0';
		sName = tempString;
		delete tempString;
		
		/*
			// For Debugging
			CString textMessage(sName.c_str());
			::AfxMessageBox(textMessage);
		*/

		// Move pointer over to end of Name
		pBuffer = (pBuffer + sizeOfName);
		// Get size of phone number
		sizeOfPhoneNum = *((int*)pBuffer);
		// Move pointer over to point at phone number 
		pBuffer = (pBuffer + SIZE_INT);

		// Loop over the buffer to extract the phone number field
		tempString = new char[sizeOfPhoneNum + 1];
		for (int i = 0; i < sizeOfPhoneNum; i++)
		{
			tempString[i] = *(pBuffer + i);
		}
		tempString[sizeOfPhoneNum] = '\0';
		sPhoneNumber = tempString;
		delete tempString;
		
		/*
			// For Debugging
			CString textMessage2(sPhoneNumber.c_str());
			::AfxMessageBox(textMessage2);
		*/

		pBuffer = (pBuffer + sizeOfPhoneNum);
		return pBuffer;
	}
};


// Represents a list of users
struct TGroup
{
	int guid;
	// Simple implementation: For every change in group we send all numbers.
	std::vector<TUser> nlistUsers;
	char* ToBuffer(char* pBuffer)
	{
		*((int*)pBuffer) = SIZE_GUID;
		*((int*)(pBuffer + SIZE_INT)) = guid;

		pBuffer = (pBuffer + SIZE_GUID + SIZE_INT);
		for (auto it = nlistUsers.begin(); it != nlistUsers.end(); ++it)
		{
			pBuffer = (it->ToBuffer(pBuffer));
		}

		return pBuffer;
	}

	void FromBuffer(char* pBuffer)
	{
		int sizeOfNext;// the size of this variable itself is SIZE_INT

		sizeOfNext = *((int*)pBuffer);
		guid = *((int*)(pBuffer + sizeOfNext));
		pBuffer = (pBuffer + SIZE_INT + SIZE_GUID);
		// sizeOfNext's value is probably no longer same size as an int therefor move pointer SIZE_INT instead of sizeOfNext
		for (auto it = nlistUsers.begin(); it != nlistUsers.end(); ++it)
		{
			pBuffer = (it->FromBuffer(pBuffer));
		}

		return;
	}
};


// Represents one message
struct TTextMessage
{
	std::string m_sText; // message to send
	TUser m_userDestination; // to which user we are sending, in event we are sending to individual user/s
	TGroup m_groupDestination; // to which group, in event where sending to whole group 
	void ToBuffer(char* pBuffer)
	{
		int textLength = m_sText.length();
		*((int*)pBuffer) = textLength;
		
		pBuffer = (pBuffer + SIZE_INT);
		m_sText.copy(pBuffer, textLength);

		//((pBuffer + SIZE_INT), m_sText.GetBuffer(), textLength);
		// Move buffer pointer over and pass on the buffer to TUser struct to deal with
		pBuffer = (pBuffer + textLength);
		pBuffer = m_userDestination.ToBuffer(pBuffer);
		pBuffer = m_groupDestination.ToBuffer(pBuffer);
	}

	void FromBuffer(char* pBuffer)
	{
		// This value  indicates the number of BYTEs to read to  extract the current information, filling one field at a time
		// In this case, it should give me the num of BYtes m_sText takes
		int sizeOfNext;
		
		sizeOfNext = *((int*)pBuffer);
		
		
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		pBuffer = (pBuffer + SIZE_INT);

		char* tempString = new char[sizeOfNext+1];
		for (int i = 0; i < sizeOfNext; i++)//************************************************
		{
			tempString[i] = *(pBuffer + i);
		}
		tempString[sizeOfNext] = '\0';
		m_sText = tempString;
		delete tempString;
		// For Debugging
		CString textMessage(m_sText.c_str());
		::AfxMessageBox(textMessage);

		pBuffer = (pBuffer + sizeOfNext);

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		
		
		pBuffer = m_userDestination.FromBuffer(pBuffer);
		m_groupDestination.FromBuffer(pBuffer);
	}

};

enum EMessageType
{
	TEXT_MESSAGE,
	CREATE_UPDATE_GROUP,
	ACKNOWLEDGE 
};

//int test1 = 401;
//int test2 = 404;

// convert int to CString
/* auto a = std::to_string(cBuffer[i]);
*	CString Ca(a.c_str());
*	AfxMessageBox(Ca);
*/

/*int ea = *((int *)pBuffer);
		wchar_t eb = *((wchar_t *)(pBuffer + SIZE_INT));
		wchar_t ec = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t)));
		wchar_t ed = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t ee = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t ef = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t eg = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t eh = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t ei = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t ej = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t ek = *((wchar_t *)(pBuffer + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));*/