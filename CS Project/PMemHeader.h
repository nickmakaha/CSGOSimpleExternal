

#ifndef PainMem_H //If Not Defined
#define PainMem_H //Define Now

#define WIN32_LEAN_AND_MEAN //Excludes Headers We Wont Use (Increase Compile Time)

#include <assert.h>
#include <windows.h> //Standard Windows Functions/Data Types
#include <iostream> //Constains Input/Output Functions (cin/cout etc..)
#include <TlHelp32.h> //Contains Read/Write Functions
#include <string> //Support For Strings
#include <sstream> //Supports Data Conversion

using namespace std;

class PainMem {
protected:

	//STORAGE
	HANDLE hProcess;
	DWORD dwPID, dwProtection, dwCaveAddress;

	//MISC
	BOOL bPOn, bIOn, bProt;

public:

	//MISC FUNCTIONS
	PainMem();
	~PainMem();

#pragma region TEMPLATE MEMORY FUNCTIONS

									 //REMOVE READ/WRITE PROTECTION
	template <class cData>
	void Protection(DWORD dwAddress)
	{
		if (!bProt)
			VirtualProtectEx(hProcess, (LPVOID)dwAddress, sizeof(cData), PAGE_EXECUTE_READWRITE, &dwProtection); //Remove Read/Write Protection By Giving It New Permissions
		else
			VirtualProtectEx(hProcess, (LPVOID)dwAddress, sizeof(cData), dwProtection, &dwProtection); //Restore The Old Permissions After You Have Red The dwAddress

		bProt = !bProt;
	}

	//READ MEMORY 
	template <class cData>
	cData Read(DWORD dwAddress)
	{
		cData cRead; //Generic Variable To Store Data
		ReadProcessMemory(hProcess, (LPVOID)dwAddress, &cRead, sizeof(cData), NULL); //Win API - Reads Data At Specified Location 
		return cRead; //Returns Value At Specified dwAddress
	}

	//READ MEMORY - Pointer
	template <class cData>
	cData Read(DWORD dwAddress, char *Offset, BOOL Type)
	{
		//Variables
		int iSize = iSizeOfArray(Offset) - 1; //Size Of *Array Of Offsets 
		dwAddress = Read<DWORD>(dwAddress); //HEX VAL

											//Loop Through Each Offset & Store Hex Value (Address)
		for (int i = 0; i < iSize; i++)
			dwAddress = Read<DWORD>(dwAddress + Offset[i]);

		if (!Type)
			return dwAddress + Offset[iSize]; //FALSE - Return Address
		else
			return Read<cData>(dwAddress + Offset[iSize]); //TRUE - Return Value
	}

	// WRITE MEMORY
	template <class cData>
	void Write(DWORD dwAddress, cData Value)
	{
		WriteProcessMemory(hProcess, (LPVOID)dwAddress, &Value, sizeof(cData), NULL);
	}

	// WRITE MEMORY - Pointer
	template <class cData>
	void Write(DWORD dwAddress, char *Offset, cData Value)
	{
		Write<cData>(Read<cData>(dwAddress, Offset, false), Value);
	}

	// Base read
	template <typename TYPE>
	TYPE RPM(LPVOID lpBaseAddress, SIZE_T nSize)
	{
		TYPE data = TYPE();
		ReadProcessMemory(hProcess, lpBaseAddress, &data, nSize, NULL);
		return data;
	}

	// Base write
	void WPM(LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize)
	{
		WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, NULL);
	}

	// MEMORY FUNCTION PROTOTYPES
	virtual bool Process(char* ProcessName); // Return Handle To The Process

	virtual bool Module(LPSTR ModuleName, DWORD &output); // Return Module Base Address

#pragma endregion	

};
#endif