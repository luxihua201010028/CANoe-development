/*----------------------------------------------------------------------------
|
| File Name: capldll.cpp
|
|            Example of a capl DLL implementation module and using CAPLLbacks.
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
|   Author             Initials
|   ------             --------
|   Thomas  Riegraf    Ri              Vector Informatik GmbH
|   Hans    Quecke     Qu              Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
| Date         Ver  Author  Description
| ----------   ---  ------  --------------------------------------------------
| 2003-10-07   1.0  As      Created
| 2007-03-26   1.1  Ej      Export of the DLL function table as variable
|                           Use of CAPL_DLL_INFO3
|                           Support of long name CAPL function calls
| 2017-08-20   2.0  Jason   Adapt for CAPL DLL demo
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c) 1994 - 2003 by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
 

#define USECDLL_FEATURE
#define _BUILDNODELAYERDLL

#pragma warning( disable : 4786 )

#include "..\Includes\cdll.h"
#include "..\Includes\via.h"
#include "..\Includes\via_CDLL.h"

#include <stdio.h>
#include <stdlib.h>
#include <map>

//for MAC read
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class CaplInstanceData;
typedef std::map<uint32, CaplInstanceData*> VCaplMap;
typedef std::map<uint32, VIACapl*> VServiceMap;


// ============================================================================
// global variables
// ============================================================================

static unsigned long data = 0;
static char dlldata[100];

static unsigned long mac1 = 0; //e.g. F037FE
static unsigned long mac2 = 0; //e.g. 807C3F

char        gModuleName[_MAX_FNAME];  // filename of this  dll 
HINSTANCE   gModuleHandle;            // windows instance handle of this DLL
VCaplMap    gCaplMap;   
VServiceMap gServiceMap;


// ============================================================================
// CaplInstanceData
// 
// Data local for a single CAPL Block. 
//
// A CAPL-DLL can be used by more than one CAPL-Block, so every piece of 
// information thats like a globale variable in CAPL, must now be wraped into 
// an instance of an object.
// ============================================================================
class CaplInstanceData
{
public:
  CaplInstanceData(VIACapl* capl);
  
  void GetCallbackFunctions();
  void ReleaseCallbackFunctions();

  // Definition of the class function. 
  // This class function will call the 
  // CAPL callback functions 
  uint32 ShowValue(uint32 x);
  uint32 ShowDates(int16 x, uint32 y, int16 z);
  void   DllInfo(char* x);
  void   ArrayValues(uint32 flags, uint32 numberOfDatabytes, uint8 databytes[], uint8 controlcode);
  void DllVersion(char* y);

private:  

  // Pointer of the CAPL callback functions 
  VIACaplFunction*  mShowValue;
  VIACaplFunction*  mShowDates;
  VIACaplFunction*  mDllInfo;
  VIACaplFunction*  mArrayValues;
  VIACaplFunction*  mDllVersion;

  VIACapl*          mCapl;
};


CaplInstanceData::CaplInstanceData(VIACapl* capl) 
  // This function will initialize the CAPL callback function 
  // with the NLL Pointer 
 : mCapl(capl), 
   mShowValue(NULL),
   mShowDates(NULL),
   mDllInfo(NULL),
   mArrayValues(NULL),
   mDllVersion(NULL)
{}

static bool sCheckParams(VIACaplFunction* f, char rtype, char* ptype)
{
  char      type;
  int32     pcount;
  VIAResult rc;

  // check return type
  rc = f->ResultType(&type);
  if (rc!=kVIA_OK || type!=rtype)
  {
    return false;
  }

  // check number of parameters
  rc = f->ParamCount(&pcount);
  if (rc!=kVIA_OK || strlen(ptype)!=pcount )
  {
    return false;
  }

  // check type of parameters
  for (int i=0; i<pcount; ++i)
  {
    rc = f->ParamType(&type, i);
    if (rc!=kVIA_OK || type!=ptype[i])
    {
      return false;
    }
  }

  return true;
}

static VIACaplFunction* sGetCaplFunc(VIACapl* capl, const char * fname, char rtype, char* ptype)
{
  VIACaplFunction* f; 

  // get capl function object
  VIAResult rc =  capl->GetCaplFunction(&f, fname);
  if (rc!=kVIA_OK || f==NULL) 
  {
    return NULL;
  }

  // check signature of function
  if ( sCheckParams(f, rtype, ptype) )
  {
     return f;
  }
  else
  {
    capl->ReleaseCaplFunction(f);
    return NULL;
  }
}

void CaplInstanceData::GetCallbackFunctions()
{
  // Get a CAPL function handle. The handle stays valid until end of
  // measurement or a call of ReleaseCaplFunction. 
  mShowValue   = sGetCaplFunc(mCapl, "CALLBACK_ShowValue", 'D', "D");
  mShowDates   = sGetCaplFunc(mCapl, "CALLBACK_ShowDates", 'D', "IDI");
  mDllInfo     = sGetCaplFunc(mCapl, "CALLBACK_DllInfo", 'V', "C");
  mArrayValues = sGetCaplFunc(mCapl, "CALLBACK_ArrayValues", 'V', "DBB");
  mDllVersion  = sGetCaplFunc(mCapl, "CALLBACK_DllVersion", 'V', "C");
}

void CaplInstanceData::ReleaseCallbackFunctions()
{
  // Release all the requested Callback functions 
  mCapl->ReleaseCaplFunction(mShowValue);
  mShowValue = NULL;
  mCapl->ReleaseCaplFunction(mShowDates);
  mShowDates = NULL;
  mCapl->ReleaseCaplFunction(mDllInfo);
  mDllInfo = NULL;
  mCapl->ReleaseCaplFunction(mArrayValues);
  mArrayValues = NULL;
  mCapl->ReleaseCaplFunction(mDllVersion);
  mDllVersion = NULL;
}

void CaplInstanceData::DllVersion(char* y)
{
  // Prepare the parameters for the call stack of CAPL. 
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  int32 sizeX = strlen(y)+1;

  uint8 params[8];               // parameters for call stack, 8 Bytes total    
  memcpy(params+0, &sizeX, 4);   // array size    of first parameter, 4 Bytes
  memcpy(params+4, &y,     4);   // array pointer of first parameter, 4 Bytes
  
  if(mDllVersion!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mDllVersion->Call(&result, params);
  }
}


uint32 CaplInstanceData::ShowValue(uint32 x)
{
  void* params = &x;   // parameters for call stack

  uint32 result;

  if(mShowValue!=NULL)
  {
    VIAResult rc =  mShowValue->Call(&result, params);
    if (rc==kVIA_OK)
    {
       return result;
    }
  }    
  return -1;
}

uint32 CaplInstanceData::ShowDates(int16 x, uint32 y, int16 z)
{
  // Prepare the parameters for the call stack of CAPL. The stack grows
  // from top to down, so the first parameter in the parameter list is the last 
  // one in memory. CAPL uses also a 32 bit alignment for the parameters. 
  uint8 params[12];         // parameters for call stack, 12 Bytes total    
  memcpy(params+0, &z, 2);  // third  parameter, offset 0, 2 Bytes
  memcpy(params+4, &y, 4);  // second parameter, offset 4, 4 Bytes
  memcpy(params+8, &x, 2);  // first  parameter, offset 8, 2 Bytes 

  uint32 result;

  if(mShowDates!=NULL)
  {
    VIAResult rc =  mShowDates->Call(&result, params);
    if (rc==kVIA_OK)
    {
       return rc;   // call successful
    }
  } 
    
  return -1; // call failed
}

void CaplInstanceData::DllInfo(char* x)
{
  // Prepare the parameters for the call stack of CAPL. 
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  int32 sizeX = strlen(x)+1;

  uint8 params[8];               // parameters for call stack, 8 Bytes total    
  memcpy(params+0, &sizeX, 4);   // array size    of first parameter, 4 Bytes
  memcpy(params+4, &x,     4);   // array pointer of first parameter, 4 Bytes
  
  if(mDllInfo!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mDllInfo->Call(&result, params);
  }
}

void CaplInstanceData::ArrayValues(uint32 flags, uint32 numberOfDatabytes, uint8 databytes[], uint8 controlcode)
{
  // Prepare the parameters for the call stack of CAPL. The stack grows
  // from top to down, so the first parameter in the parameter list is the last 
  // one in memory. CAPL uses also a 32 bit alignment for the parameters.
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  
  uint8 params[16];                           // parameters for call stack, 16 Bytes total    
  memcpy(params+ 0, &controlcode,       1);   // third parameter,                  offset  0, 1 Bytes
  memcpy(params+ 4, &numberOfDatabytes, 4);   // second parameter (array size),    offset  4, 4 Bytes
  memcpy(params+ 8, &databytes,         4);   // second parameter (array pointer), offset  8, 4 Bytes
  memcpy(params+12, &flags,             4);   // first  parameter,                 offset 12, 4 Bytes 

  if(mArrayValues!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mArrayValues ->Call(&result, params);
  }

}

CaplInstanceData* GetCaplInstanceData(uint32 handle)
{
  VCaplMap::iterator lSearchResult(gCaplMap.find(handle));
  if ( gCaplMap.end()==lSearchResult )
  {
    return NULL;
  } else {
    return lSearchResult->second;
  }
}

// ============================================================================
// CaplInstanceData
// 
// Data local for a single CAPL Block. 
//
// A CAPL-DLL can be used by more than one CAPL-Block, so every piece of 
// information thats like a global variable in CAPL, must now be wrapped into 
// an instance of an object.
// ============================================================================

void CAPLEXPORT far CAPLPASCAL appInit (uint32 handle)
{
  CaplInstanceData* instance = GetCaplInstanceData(handle);
  if ( NULL==instance )
  {
    VServiceMap::iterator lSearchService(gServiceMap.find(handle));
    if ( gServiceMap.end()!=lSearchService )
    {
      VIACapl* service = lSearchService->second;
      try 
      {
        instance = new CaplInstanceData(service);
      }
      catch ( std::bad_alloc& )
      {
        return; // proceed without change
      }
      instance->GetCallbackFunctions();
      gCaplMap[handle] = instance;
    }
  }
}    

void CAPLEXPORT far CAPLPASCAL appEnd (uint32 handle)
{
  CaplInstanceData* inst = GetCaplInstanceData(handle);
  if (inst==NULL)
  {
    return;
  }
  inst->ReleaseCallbackFunctions();

  delete inst;
  inst = NULL; 
  gCaplMap.erase(handle);
}    


// ============================================================================
// VIARegisterCDLL
// ============================================================================

VIACLIENT(void) VIARegisterCDLL (VIACapl* service)
{
  uint32    handle;
  VIAResult result;

  if (service==NULL)
  {
    return;
  }

  result = service->GetCaplHandle(&handle);
  if(result!=kVIA_OK)
  {
    return;
  }

  // appInit (internal) resp. "DllInit" (CAPL code) has to follow
  gServiceMap[handle] = service;
}

void ClearAll()
{
  // destroy objects created by this DLL
  // may result from forgotten DllEnd calls
  VCaplMap::iterator lIter=gCaplMap.begin();
  const long cNumberOfEntries = gCaplMap.size();
  long i = 0;
  while ( lIter!=gCaplMap.end() && i<cNumberOfEntries )
  {
    appEnd( (*lIter).first );
    lIter = gCaplMap.begin(); // first element should have vanished
    i++; // assure that no more erase trials take place than the original size of the map
  }

  // just for clarity (would be done automatically)
  gCaplMap.clear();
  gServiceMap.clear();
}

//-----------CRC高位字节值表-------------------------------
unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
//-----------CRC低位字节值表 ------------------------------
unsigned char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//-------------输入字符串 得到相应的CRC校验值--------------
long CAPLEXPORT far CAPLPASCAL appCrc16(unsigned char puchMsg[], long usDataLen)
{
	unsigned char uchCRCHi = 0xFF; // 高CRC字节初始化
	unsigned char uchCRCLo = 0xFF; // 低CRC 字节初始化
	unsigned int uIndex;           // CRC循环中的索引
	long result;
	while (usDataLen--)     // 传输消息缓冲区
	{
		uIndex = uchCRCHi ^ *puchMsg++; // 计算CRC
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	result = uchCRCHi << 8 | uchCRCLo;
	return result;
}

//return 0 - success;  1 - failed
int appGetMAC(void)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;
	byte firstMAC[6] = { 00, 00, 00, 00, 00, 00 };
	int sIndex = 0;
	//errno_t error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return 1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return 1;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			if (sIndex == 0)
				sIndex = pAdapter->ComboIndex;
			if (sIndex >= (int)pAdapter->ComboIndex)
			{
				sIndex = pAdapter->ComboIndex;
				for (i = 0; i < pAdapter->AddressLength; i++) {
					firstMAC[i] = pAdapter->Address[i];
					printf("%.2X", (int)pAdapter->Address[i]);
				}
				printf("\tIndex: \t%d\n", pAdapter->Index);
			}
			pAdapter = pAdapter->Next;
		}
	}
	else
	{
		return 1;
	}
	mac1 = 0;
	for (i = 0; i < 3; i++) {
		mac1 = mac1 * 0x100 + (int)firstMAC[i];
	}
	mac2 = 0;
	for (i = 0; i < 3; i++) {
		mac2 = mac2 * 0x100 + (int)firstMAC[i + 3];
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
	return 0;
}

//get the MAC part-1
long CAPLEXPORT far CAPLPASCAL appGetMac1(void)
{
	if (!appGetMAC())
	{
		return mac1;
	}
	else
		return 0;
}

//get the MAC part-2
long CAPLEXPORT far CAPLPASCAL appGetMac2(void)
{
	if (!appGetMAC())
	{
		return mac2;
	}
	else
		return 0;
}
long CAPLEXPORT far CAPLPASCAL appSubtract(long x, long y)
{
	long z = x - y;

	return z;
}


// ============================================================================
// CAPL_DLL_INFO_LIST : list of exported functions
//   The first field is predefined and mustn't be changed!
//   The list has to end with a {0,0} entry!
// New struct supporting function names with up to 50 characters
// ============================================================================
CAPL_DLL_INFO4 table[] = {
{CDLL_VERSION_NAME, (CAPL_FARCALL)CDLL_VERSION, "", "", CAPL_DLL_CDECL, 0xabcd, CDLL_EXPORT },
  {"dllInit",           (CAPL_FARCALL)appInit,          "CAPL_DLL","This function will initialize all callback functions in the CAPLDLL",'V', 1, "D", "", {"handle"}},
  {"dllEnd",            (CAPL_FARCALL)appEnd,           "CAPL_DLL","This function will release the CAPL function handle in the CAPLDLL",'V', 1, "D", "", {"handle"}}, 
  {"dllSubtract",		(CAPL_FARCALL)appSubtract,		"CAPL_DLL", "This function will substract two values. The return value is the result", 'L', 2, "LL", "", { "x", "y" }},
  {"dllCrc16",			(CAPL_FARCALL)appCrc16,			"CAPL_DLL", "This function will calculate CRC16. The return value is CRC16 value", 'L', 2, "BL", "\001\000", { "puchMsg","usDataLen"} },
  {"dllGetMac1",		(CAPL_FARCALL)appGetMac1,		"CAPL_DLL", "This function will read MAC address part1", 'L', 0, "V", "", { "" }},
  {"dllGetMac2",		(CAPL_FARCALL)appGetMac2,		"CAPL_DLL", "This function will read MAC address part2", 'L', 0, "V", "", { "" }},
  {0, 0}
};
CAPLEXPORT CAPL_DLL_INFO4 far * caplDllTable4 = table;

// ============================================================================
// DllMain, entry Point of DLL  
// ============================================================================

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, void*)
{     
  switch (reason) 
  {
    case DLL_PROCESS_ATTACH:
    {
      gModuleHandle = handle;
         
      // Get full filename of module
      char path_buffer[_MAX_PATH];
      DWORD result = GetModuleFileName(gModuleHandle, path_buffer,_MAX_PATH);

      // split filename into parts
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];
#if _MSC_VER>=1400 // >= Visual Studio 2005
      _splitpath_s( path_buffer, drive, dir, fname, ext );
      strcpy_s(gModuleName, fname);
#else
      _splitpath( path_buffer, drive, dir, fname, ext );
      strcpy(gModuleName, fname);
#endif
      
      return 1;   // Indicate that the DLL was initialized successfully.
    }

    case DLL_PROCESS_DETACH:                                              
    {
      ClearAll();
      return 1;   // Indicate that the DLL was detached successfully.
    }
  }
  return 1;
}



