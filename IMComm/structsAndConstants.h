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
		int guidLength = sizeof(guid);
		int nameLength = sName.length();// Returns # of BYTES used(1 for each char) not inc null terminator
		int phoneNumberLength = sPhoneNumber.length();//

		*((int*)pBuffer) = guidLength;
		//int ea = *((int *)pBuffer);

		*((int*)(pBuffer + SIZE_INT)) = guid;
		//int eb = *((int *)(pBuffer + SIZE_INT));

		*((int*)(pBuffer + SIZE_INT + SIZE_GUID)) = nameLength;
		//int ec = *((int *)(pBuffer + SIZE_INT + SIZE_GUID));


		pBuffer = (pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT);
		sName.copy(pBuffer, nameLength);// When extracting from the buffer need to add \0 to end of extracted chars
		//memcpy((pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT), sName., nameLength);//*********************************************************************
		//char ed = *((char *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT));
		//char ee = *((char *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + sizeof(char)));
		//wchar_t ef = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t)));
		//wchar_t eg = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		

		*((int*)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength)) = phoneNumberLength;
		//int el = *((int *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength));

		pBuffer = (pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT);
		sPhoneNumber.copy(pBuffer, phoneNumberLength);
		//memcpy((pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT), sPhoneNumber.GetBuffer(), phoneNumberLength);//*******************
		/*wchar_t em = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT));
		wchar_t en = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + sizeof(wchar_t)));
		wchar_t eq = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) ));
		wchar_t er = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) ));
		wchar_t es = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		wchar_t et = *((wchar_t *)(pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t) + sizeof(wchar_t)));
		*/
		
		pBuffer = (pBuffer + SIZE_INT + SIZE_GUID + SIZE_INT + nameLength + SIZE_INT + phoneNumberLength);

		return pBuffer;
	}

	char* FromBuffer(char* pBuffer)
	{
		int sizeOfNext1;// the size of this variable itself is SIZE_INT
		int sizeOfNext2;
		int sizeOfNext3;
		int sizeOfNext4;
		char* tempString;

		sizeOfNext1 = *((int*)pBuffer);
		guid = *((int*)(pBuffer + SIZE_INT));
		sizeOfNext2 = *((int*)(pBuffer + SIZE_INT + sizeOfNext1));
		// sizeOfNext's value is probably no longer same size as an int therefor move pointer SIZE_INT instead of sizeOfNext
		pBuffer = (pBuffer + SIZE_INT + sizeOfNext1 + sizeOfNext2);

		// Get size of name
		sizeOfNext3 = *((int*)pBuffer);
		// Move pointer over to point at name so loop can be kept simple
		pBuffer = (pBuffer + SIZE_INT);

		tempString = new char[sizeOfNext3 + 1];
		for (int i = 0; i < sizeOfNext3; i++)//************************************************
		{
			tempString[i] = *(pBuffer + i);
		}
		tempString[sizeOfNext3] = '\0';
		sName = tempString;
		delete tempString;
		// For Debugging
		CString textMessage(sName.c_str());
		::AfxMessageBox(textMessage);
		
		// Move pointer over to end of sName
		pBuffer = (pBuffer + sizeOfNext3);


		// Get size of phone number
		sizeOfNext4 = *((int*)pBuffer);
		// Move pointer over to point at phone number so loop can be kept simple
		pBuffer = (pBuffer + SIZE_INT);
		tempString = new char[sizeOfNext4 + 1];
		for (int i = 0; i < sizeOfNext4; i++)//*******************************************************************************************************
		{
			tempString[i] = *(pBuffer + i);
		}
		tempString[sizeOfNext3] = '\0';
		sPhoneNumber = tempString;
		delete tempString;
		// For Debugging
		CString textMessage2(sPhoneNumber.c_str());
		::AfxMessageBox(textMessage2);

		pBuffer = (pBuffer + sizeOfNext4);

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
		//char* p_temp = pBuffer;
		*((int*)pBuffer) = guid;
		//int ea = *((int*)pBuffer);

		pBuffer = (pBuffer + SIZE_GUID);
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
		// CString is not a primitive type and therefor we can't typecast and simply copy, rather we must access the 
		//buffer containing the CString; the string is in unicode and therefor is alloted 2 bytes for each letter;
		// GetBuffer() returns the number of letters and therfor we must multi by 2 to get the bytes.
		//pBuffer = StringToBuffer(pBuffer, m_sText);
		
		int textLength = m_sText.length();
		*((int*)pBuffer) = textLength;
		//int ev = *((int *)(pBuffer + SIZE_GUID));
		// Add text message(CString) to the buffer

		pBuffer = (pBuffer + SIZE_INT);
		m_sText.copy(pBuffer, textLength);

		//((pBuffer + SIZE_INT), m_sText.GetBuffer(), textLength);
		// Move buffer pointer over and pass on the buffer to TUser struct to deal with
		pBuffer = (pBuffer + SIZE_INT + textLength);
		pBuffer = m_userDestination.ToBuffer(pBuffer);
		pBuffer = m_groupDestination.ToBuffer(pBuffer);
	}

	void FromBuffer(char* pBuffer)
	{
		int sizeOfNext;// the size of this variable itself is SIZE_INT
		
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

		pBuffer = (pBuffer + SIZE_INT + sizeOfNext);

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