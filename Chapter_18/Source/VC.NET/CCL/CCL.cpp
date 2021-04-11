/*------------------------------------------------------------------ 
<FILE> 
  <NAME> CCL.cpp
  <PRJ > CANalyzer/CANoe C Library API (CCL)
  <REV > 1.6.3
  <DATE> 2015-08-15
  <DESC> 
   The module implements the CANalyzer/CANoe C Library API
  <COPYRIGHT>(c) 2011 by Vector Informatik GmbH.  All rights reserved.
  <HISTORY> 
   1.0  2011-01-24  First version of the API created.
   1.1  2011-02-11  cclPrintf added
   1.2  2014-04-04  Signal access added
   1.3  2014-06-12  cclGetUserFilePath added
   1.4  2014-06-25  Structures for system variables supported
   1.5  2014-12-09  cclSysVarGetType added
   1.6  2015-08-15  System variable (struct members) physical values supported
</FILE> 
----------------------------------------------------------------- */


#include "CCL.h"

#include <windows.h>
#define _BUILDNODELAYERDLL
#include "VIA.h"
#include "VIA_CAN.h"
#include "VIA_LIN.h"
#include "VIA_SignalAccess.h"

#include <vector>
#include <memory.h>
#include <string.h>
#include <malloc.h>


namespace CANalyzerCANoeCLibrary
{
  class VNodeLayerModule;
  class VNodeLayer;

  // ============================================================================
  // global Variables
  // ============================================================================

  HINSTANCE           gDLLInstanceHandle;
  VNodeLayerModule*   gModule      = NULL;
  VIAService*         gVIAService  = NULL;
  int                 gVIAMinorVersion = 0;
  VIANamespace*       gSysVarRoot  = NULL;
  VNodeLayer*         gMasterLayer = NULL;
  VIASignalAccessApi* gSignalAccessAPI = NULL;

  const int cMaxChannel = 32;

  struct CanBusContext
  {
    VIACan*       mBus;
    VIANode*      mNode;
    VNodeLayer*   mLayer;
  } gCanBusContext[cMaxChannel+1];

  struct LinBusContext
  {
    VIALin*       mBus;
    VIANode*      mNode;
    VNodeLayer*   mLayer;
  } gLinBusContext[cMaxChannel+1];


  // ============================================================================
  // State 
  // ============================================================================
  
  enum State
  {
    eNotInitialized  =  1,
    eLoaded          =  2,
    eInitMeasurement =  4,
    eRunMeasurement  =  8,
    eStopMeasurement = 16,
    eUnloaded        = 32,
  } gState = eNotInitialized;


  inline bool IsInState(int states)
  {
    return (gState & states) != 0;
  }

#define CCL_STATECHECK(states)  if (!IsInState(states)) { return CCL_WRONGSTATE; }

  // ============================================================================
  // VTimer
  // ============================================================================


  class VTimer : public VIAOnTimerSink
  {
  public:
    VIASTDDECL OnTimer (VIATime nanoseconds);

    VIATimer*  mViaTimer;
    int        mTimerID;
    void (*mCallbackFunction)(long long time, int ID);
  };


  // ============================================================================
  // VSysVar
  // ============================================================================


  class VSysVar : public VIAOnSysVar, public VIAOnSysVarMember
  {
  public:
    VIASTDDECL OnSysVar(VIATime nanoseconds, VIASystemVariable* ev);
    VIASTDDECL OnSysVarMember(VIATime nanoseconds, VIASystemVariableMember* ev, VIASysVarClientHandle origin);

    static int LookupVariable(int sysVarID, VSysVar*& sysVar);
    static int LookupVariable(int sysVarID, VSysVar*& sysVar, 
                              VIASysVarType t, bool writable);

    int CheckWriteable();

    VIASystemVariable*        mViaSysVar;
    VIASystemVariableMember*  mViaSysVarMember;
    std::string               mVariableName;
    std::string               mMemberName;
    int                       mSysVarID;
    void (*mCallbackFunction)(long long time, int sysVarID);
  };


  // ============================================================================
  // VSignal
  // ============================================================================


  class VSignal : public VIAOnSignal
  {
  public:
    VIASTDDECL OnSignal(VIASignal* signal, void* userData);

    static int LookupSignal(int signalID, VSignal*& signal);

    VIASignal* mViaSignal;
    int        mSignalID;
    void (*mCallbackFunction)(int sysVarID);
  };


  // ============================================================================
  // VCanMessageRequest
  // ============================================================================


  class VCanMessageRequest: public VIAOnCanMessage
  {
  public:
    VIASTDDECL OnMessage(VIATime     time,
                         VIAChannel  channel,
                         uint8       dir,
                         uint32      id,
                         uint32      flags,
                         uint8       dlc,
                         const uint8 data[8]);

    VIARequestHandle  mHandle;
    CanBusContext*    mContext;
    void (*mCallbackFunction) (cclCanMessage* message);
  };


  // ============================================================================
  // VLinMessageRequest
  // ============================================================================


  class VLinMessageRequest: public VIAOnLinMessage2
  {
  public:
    VIASTDDECL OnMessage(VIATime  timeFrameEnd,
                         VIATime  timeFrameStart,
                         VIATime     timeHeaderEnd,
                         VIATime     timeSynchBreak,
                         VIATime     timeSynchDel,
                         VIAChannel  channel,
                         uint8       dir,
                         uint32      id,
                         uint8       dlc,
                         uint8       simulated,
                         const uint8 data[8] );

    VIARequestHandle  mHandle;
    LinBusContext*    mContext;
    void (*mCallbackFunction) (cclLinFrame* frame);
  };


  // ============================================================================
  // VNodeLayerModule
  // ============================================================================


  class VNodeLayerModule
    : public VIAModuleApi
  {
  public:
    VNodeLayerModule();

    VIASTDDECL Init();
    VIASTDDECL GetVersion (char* buffer, int32 bufferLength );
    VIASTDDECL GetModuleParameters (char* pathBuff, int32 pathBuffLength,
      char* versionBuff, int32 versionBuffLength); 
    VIASTDDECL CreateObject  (VIANodeLayerApi** object,
      VIANode* node,
      int32 argc, char* argv[]);
    VIASTDDECL ReleaseObject (VIANodeLayerApi*  object);
    VIASTDDECL GetNodeInfo   (const char*  nodename,
      char* shortNameBuf, int32 shortBufLength,
      char* longNameBuf,  int32 longBufLength );
    VIASTDDECL InitMeasurement ();
    VIASTDDECL EndMeasurement  ();
    VIASTDDECL GetNodeInfoEx (VIDBNodeDefinition* nodeDefinition,
      char* shortNameBuf, int32 shortBufLength,
      char* longNameBuf, int32 longBufLength );

    void (*mMeasurementPreStartHandler)();
    void (*mMeasurementStartHandler)();
    void (*mMeasurementStopHandler)();
    void (*mDllUnloadHandler)();

    std::vector<VTimer*>             mTimer;
    std::vector<VSysVar*>            mSysVar;
    std::vector<VSignal*>            mSignal;
    std::vector<VCanMessageRequest*> mCanMessageRequests;
    std::vector<VLinMessageRequest*> mLinMessageRequests;
  };


  // ============================================================================
  // VNodeLayer
  // ============================================================================


  class VNodeLayer
    : public VIANodeLayerApi
  {
  public:
    VNodeLayer(VIANode* node, uint32 busType, VIAChannel channel, VIABus* bus);
    virtual ~VNodeLayer();
    VIASTDDECL GetNode(VIANode **node);
    VIASTDDECL InitMeasurement();
    VIASTDDECL StartMeasurement();
    VIASTDDECL StopMeasurement();
    VIASTDDECL EndMeasurement();
    VIASTDDECL InitMeasurementComplete();
  public:

    VIANode*      mNode;
    uint32        mBusType;
    VIAChannel    mChannel;
    VIABus*       mBus;
  };

  // ============================================================================
  // Implementation of class VTimer
  // ============================================================================

  VIASTDDEF VTimer::OnTimer(VIATime nanoseconds)
  {
    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(nanoseconds, mTimerID);
    }
    return kVIA_OK;
  }


  // ============================================================================
  // Implementation of class VSysVar
  // ============================================================================

  VIASTDDEF VSysVar::OnSysVar(VIATime nanoseconds, VIASystemVariable* ev)
  {
    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(nanoseconds, mSysVarID);
    }
    return kVIA_OK;
  }


  VIASTDDEF VSysVar::OnSysVarMember(VIATime nanoseconds, VIASystemVariableMember* ev, VIASysVarClientHandle origin)
  {
    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(nanoseconds, mSysVarID);
    }
    return kVIA_OK;
  }


  int VSysVar::LookupVariable(int sysVarID, VSysVar*& sysVar)
  {
    if (gModule==NULL)
    {
      return CCL_INTERNALERROR;
    }

    if (sysVarID < 0  ||  sysVarID >= static_cast<int>(gModule->mSysVar.size()) )
    {
      return CCL_INVALIDSYSVARID;
    }

    sysVar = gModule->mSysVar[sysVarID];
    if (sysVar==NULL || sysVar->mSysVarID!=sysVarID)
    {
      return CCL_INVALIDSYSVARID;
    }

    return CCL_SUCCESS;
  }


  int VSysVar::LookupVariable(int sysVarID, VSysVar*& sysVar, VIASysVarType t, bool writable)
  {
    int result = LookupVariable(sysVarID, sysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }

    // check type of system variable
    VIASysVarType varType;
    VIAResult rc;
    if (sysVar->mViaSysVarMember!=NULL)
    {
      rc = sysVar->mViaSysVarMember->GetType(&varType);
    }
    else
    {
      rc = sysVar->mViaSysVar->GetType(&varType);
    }
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (! ( (t==varType) || 
            (t==kVIA_SVData && sysVar->mViaSysVarMember!=NULL && varType==kVIA_SVGenericArray) || 
            (t==kVIA_SVData && varType==kVIA_SVStruct) ||
            (t==kVIA_SVIntegerArray && sysVar->mViaSysVarMember!=NULL && varType==kVIA_SVGenericArray ) ) )
    {
      return CCL_WRONGTYPE;
    }

    // check, that variable is not readonly 
    if (writable)
    {
      int result = sysVar->CheckWriteable();
      if (result!=CCL_SUCCESS)
      {
        return result;
      }
    }

    return CCL_SUCCESS;
  }


  int VSysVar::CheckWriteable()
  {
    // check, that variable is not readonly 
    VIAResult rc;
    int32 readOnly;
    rc = mViaSysVar->IsReadOnly(&readOnly);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (readOnly)
    {
      return CCL_SYSVARISREADONLY;
    }
    
    return CCL_SUCCESS;
  }


  // ============================================================================
  // Implementation of class VSignal
  // ============================================================================

  VIASTDDEF VSignal::OnSignal(VIASignal* signal, void* userData)
  {
    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(mSignalID);
    }
    return kVIA_OK;
  }

  int VSignal::LookupSignal(int signalID, VSignal*& signal)
  {
    if (gModule==NULL)
    {
      return CCL_INTERNALERROR;
    }

    if (signalID < 0  ||  signalID >= static_cast<int>(gModule->mSignal.size()) )
    {
      return CCL_INVALIDSIGNALID;
    }

    signal = gModule->mSignal[signalID];
    if (signal==NULL || signal->mSignalID!=signalID)
    {
      return CCL_INVALIDSIGNALID;
    }

    return CCL_SUCCESS;
  }


  // ============================================================================
  // Implementation of class VCanMessageRequest
  // ============================================================================

  VIASTDDEF VCanMessageRequest::OnMessage(VIATime time, 
                                          VIAChannel channel,
                                          uint8 dir,
                                          uint32 id,
                                          uint32 flags,
                                          uint8 dlc,
                                          const uint8 data[8])
  {
    cclCanMessage message;
    message.time = time;
    message.channel = channel;
    message.id = id;
    message.flags = flags;
    message.dir = dir;
    message.dlc = dlc;
    memset(message.data, 0, 8);
    int size = (flags&CCL_CANFLAGS_RTR) ? 0 : ((dlc<=8) ? dlc : 8);
    memcpy_s(message.data, 8, data, size);

    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(&message);
    }
    return kVIA_OK;
  }

  // ============================================================================
  // Implementation of class VLinMessageRequest
  // ============================================================================

  VIASTDDEF VLinMessageRequest::OnMessage(VIATime     timeFrameEnd,
                                          VIATime     timeFrameStart,
                                          VIATime     timeHeaderEnd,
                                          VIATime     timeSynchBreak,
                                          VIATime     timeSynchDel,
                                          VIAChannel  channel,
                                          uint8       dir,
                                          uint32      id,
                                          uint8       dlc,
                                          uint8       simulated,
                                          const uint8 data[8])
  {
    cclLinFrame frame;
    frame.timestampEOF = timeFrameEnd;
    frame.timestampSOF = timeFrameStart;
    frame.timestampEOH = timeHeaderEnd;
    frame.timeSyncBreak = timeSynchBreak;
    frame.timeSyncDel = timeSynchDel;
    frame.channel = channel;
    frame.id = id;
    frame.dir = dir;
    frame.dlc = dlc;
    memset(frame.data, 0, 8);
    int size = (dlc<=8) ? dlc : 8;
    memcpy_s(frame.data, 8, data, size);

    if (mCallbackFunction!=NULL)
    {
      mCallbackFunction(&frame);
    }
    return kVIA_OK;
  }

  // ============================================================================
  // Implementation of class VNodeLayer
  // ============================================================================


  VNodeLayer::VNodeLayer(VIANode* node, uint32 busType, VIAChannel channel, VIABus* bus)
    : mNode(node)
    , mBusType(busType)
    , mChannel(channel)
    , mBus(bus)
  {}



  VNodeLayer::~VNodeLayer() 
  {}



  VIASTDDEF  VNodeLayer::GetNode(VIANode **node)
  {
    *node = mNode;
    return kVIA_OK;
  }



  VIASTDDEF  VNodeLayer::InitMeasurement()
  {
    if (gState==eLoaded)
    {
      VIAResult rc = gVIAService->GetSignalAccessApi(&gSignalAccessAPI, mNode, 2, 10);

      gState = eInitMeasurement;
      if (gModule->mMeasurementPreStartHandler!=NULL)
      {
        gModule->mMeasurementPreStartHandler();
      }
    }
    return kVIA_OK;
  }



  VIASTDDEF  VNodeLayer::StartMeasurement()
  {
    if (gState==eInitMeasurement)
    {
      gState=eRunMeasurement;
      if (gModule->mMeasurementStartHandler!=NULL)
      {
        gModule->mMeasurementStartHandler();
      }
    }
    return kVIA_OK;
  }



  VIASTDDEF  VNodeLayer::StopMeasurement()
  {
    if (gState==eRunMeasurement)
    {
      gState=eStopMeasurement;
      if (gModule->mMeasurementStopHandler!=NULL)
      {
        gModule->mMeasurementStopHandler();
      }
    }
    return kVIA_OK;
  }



  VIASTDDEF VNodeLayer::EndMeasurement()
  {
    return kVIA_OK;
  }



  VIASTDDEF VNodeLayer::InitMeasurementComplete()
  {
    return kVIA_OK;
  }

// ============================================================================
// Implementation of class VNodeLayerModule
// ============================================================================

  VNodeLayerModule::VNodeLayerModule()
   : mMeasurementPreStartHandler(NULL)
   , mMeasurementStartHandler(NULL)
   , mMeasurementStopHandler(NULL)
   , mDllUnloadHandler(NULL)
  {}


  VIASTDDEF VNodeLayerModule::Init()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetVersion (char* buffer, int32 bufferLength ) 
  {
    if (buffer!=NULL && bufferLength>0)
    {
      strncpy_s(buffer, bufferLength, "CANaylzer/CANoe C Library Version 1.4", _TRUNCATE);
    }
    return kVIA_OK; 
  }


  VIASTDDEF VNodeLayerModule::GetModuleParameters (char* pathBuff,    int32 pathBuffLength,
                                                   char* versionBuff, int32 versionBuffLength)
  {
    ::GetModuleFileNameA( gDLLInstanceHandle, pathBuff, pathBuffLength);
    GetVersion(versionBuff, versionBuffLength);
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::CreateObject(VIANodeLayerApi** object, VIANode* node, 
                                           int32 argc, char* argv[])
  {
    uint32 busType;
    node->GetBusType(&busType);

    VIAChannel channel;
    node->GetChannel(&channel);

    VIABus* bus = NULL;
    if (busType==kVIA_CAN)
    {
      gVIAService->GetBusInterface(&bus, node, kVIA_CAN, VIACANMajorVersion, VIACANMinorVersion);
    }
    else if (busType==kVIA_LIN)
    {
      gVIAService->GetBusInterface(&bus, node, kVIA_LIN, VIALINMajorVersion, VIALINMinorVersion);
    }

    VNodeLayer* layer = new VNodeLayer(node, busType, channel, bus);
    *object = layer;

    if (busType==kVIA_CAN && channel>=1 && channel<=cMaxChannel)
    {
      gCanBusContext[channel].mBus   = static_cast<VIACan*>(bus);;
      gCanBusContext[channel].mNode  = node;
      gCanBusContext[channel].mLayer = layer;
    } 
    else if (busType==kVIA_LIN && channel>=1 && channel<=cMaxChannel)
    {
      gLinBusContext[channel].mBus   = static_cast<VIALin*>(bus);;
      gLinBusContext[channel].mNode  = node;
      gLinBusContext[channel].mLayer = layer;
    }

    if (gMasterLayer==NULL)
    {
      gMasterLayer=layer;
    }

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::ReleaseObject (VIANodeLayerApi* object)
  {
    VNodeLayer* layer=static_cast<VNodeLayer*>(object);
    uint32 busType = layer->mBusType;
    VIAChannel channel = layer->mChannel;

    if (gMasterLayer==layer)
    {
      gMasterLayer=NULL;
      for (unsigned int i=0; i<mSignal.size(); i++)
      {
         if (mSignal[i]!=NULL)
         {
           gSignalAccessAPI->ReleaseSignal(mSignal[i]->mViaSignal);
           delete mSignal[i];
           mSignal[i] = NULL;
         }
      }
    }

    if (busType==kVIA_CAN && channel>=1 && channel<=cMaxChannel)
    {
      gCanBusContext[channel].mBus   = NULL;
      gCanBusContext[channel].mNode  = NULL;
      gCanBusContext[channel].mLayer = NULL;
    }  
    else if (busType==kVIA_LIN && channel>=1 && channel<=cMaxChannel)
    {
      gLinBusContext[channel].mBus   = NULL;
      gLinBusContext[channel].mNode  = NULL;
      gLinBusContext[channel].mLayer = NULL;
    }

    if (layer->mBus!=NULL)
    {
      gVIAService->ReleaseBusInterface(layer->mBus);
    }

    delete layer;

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetNodeInfo  (const char * nodename, 
                                            char* shortNameBuf, int32 shortBufLength,
                                            char* longNameBuf,  int32 longBufLength )
  {
    if (shortNameBuf!=NULL && shortBufLength>0)
    {
      shortNameBuf[0] = char(0);
    }

    if (longNameBuf!=NULL && longBufLength>0)
    {
      longNameBuf[0] = char(0);
    }

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::InitMeasurement()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::EndMeasurement()
  {
    gState = eLoaded;

    // delete Timers
    for (unsigned int i=0; i<mTimer.size(); i++)
    {
      VTimer* timer = mTimer[i];
      if (timer!=NULL)
      {
        if (timer->mViaTimer!=NULL)
        {
          VIAResult rc = gVIAService->ReleaseTimer(timer->mViaTimer);
          timer->mViaTimer = NULL;
        }
        delete timer;
        mTimer[i] = NULL;
      }
    }
    mTimer.clear();

    // delete system variables
    for (unsigned int i=0; i<mSysVar.size(); i++)
    {
      VSysVar* sysVar = mSysVar[i];
      if (sysVar!=NULL)
      {
        if (sysVar->mViaSysVarMember!=NULL)
        {
          VIAResult rc = sysVar->mViaSysVarMember->Release();
          sysVar->mViaSysVarMember=NULL;
        }
        if (sysVar->mViaSysVar!=NULL)
        {
          VIAResult rc = sysVar->mViaSysVar->Release();
          sysVar->mViaSysVar = NULL;
        }
        delete sysVar;
        mSysVar[i] = NULL;
      }
    }
    mSysVar.clear();

    // delete message requests for CAN
    for (unsigned int i=0; i<mCanMessageRequests.size(); i++)
    {
      VCanMessageRequest* request = mCanMessageRequests[i];
      if (request!=NULL)
      {
        if (request->mHandle!=NULL && request->mContext!=NULL)
        {
          request->mContext->mBus->ReleaseRequest(request->mHandle);
          request->mHandle = NULL;
        }
        delete request;
        mCanMessageRequests[i] = NULL;
      }
    }
    mCanMessageRequests.clear();

    // delete message requests for LIN
    for (unsigned int i=0; i<mLinMessageRequests.size(); i++)
    {
      VLinMessageRequest* request = mLinMessageRequests[i];
      if (request!=NULL)
      {
        if (request->mHandle!=NULL && request->mContext!=NULL)
        {
          request->mContext->mBus->ReleaseRequest(request->mHandle);
          request->mHandle = NULL;
        }
        delete request;
        mLinMessageRequests[i] = NULL;
      }
    }
    mLinMessageRequests.clear();

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetNodeInfoEx (VIDBNodeDefinition* nodeDefinition,
                                             char* shortNameBuf, int32 shortBufLength,
                                             char* longNameBuf, int32 longBufLength )
  {
    if (shortNameBuf!=NULL && shortBufLength>0)
    {
      shortNameBuf[0] = char(0);
    }

    if (longNameBuf!=NULL && longBufLength>0)
    {
      longNameBuf[0] = char(0);
    }

    return kVIA_OK;
  }

}


using namespace CANalyzerCANoeCLibrary;


// ============================================================================
// Define the entry point for the DLL application.
// ============================================================================


BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved )
{
  if ( DLL_PROCESS_ATTACH == ul_reason_for_call )
  {
    gDLLInstanceHandle = static_cast<HINSTANCE>(hModule);
  }

  return TRUE;
}


// ============================================================================
// NodeLayer DLL Interface Section
// ============================================================================



VIACLIENT(void) VIARequiredVersion (int32* majorversion, int32* minorversion)
{
  *majorversion = VIAMajorVersion;
  *minorversion = 40;  // VIAMinorVersion;
}



VIACLIENT(void) VIADesiredVersion (int32* majorversion, int32* minorversion)
{
  *majorversion = VIAMajorVersion;
  *minorversion = VIAMinorVersion;
}



VIACLIENT(void) VIASetService (VIAService* service)
{
  VIAResult rc;
  gVIAService = service;
  rc = gVIAService->GetSystemVariablesRootNamespace(gSysVarRoot);

  {
    int32 major, minor, patchlevel;
    rc = gVIAService->GetVersion (&major, &minor, &patchlevel);
    if (rc==kVIA_OK)
    {
      gVIAMinorVersion = minor;
    }
  }
}



VIACLIENT(VIAModuleApi*) VIAGetModuleApi(int32 argc, char* argv[])
{
  gState = eLoaded;

  if (gModule==NULL)
  {
    gModule = new VNodeLayerModule();
  }

  cclOnDllLoad();

  return gModule;
} // VIAGetModuleApi



VIACLIENT(void) VIAReleaseModuleApi (VIAModuleApi* api)
{
  gState = eUnloaded;

  if (gModule!=NULL && gModule->mDllUnloadHandler!=NULL)
  {
    gModule->mDllUnloadHandler();
  }

  if (gVIAService!=NULL && gSysVarRoot!=NULL)
  {
    gSysVarRoot->Release();
  }
  gSysVarRoot = NULL;
  
  gVIAService = NULL;
  

  delete gModule;
  gModule = NULL;
} // VIAReleaseModuleApi



extern "C" __declspec(dllexport) int32 __stdcall NLGetModuleOptions(int32 option)
{
  if (option==kVIA_ModuleOption_LoadOption)  // Load Option (0 default, 1 permanent, 2 defer)
  {
    return kVIA_ModuleOption_LoadOption_Defer;
  }
  else if (option==kVIA_GetModuleOption_DLLType) // DLL Type Option (0 default, 1 standard node layer, 2 C library)
  {
    return kVIA_ModuleOption_DLLType_CLibrary; // DLL Type C Library
  }
  else
  {
    return 0;
  }
}



// ============================================================================
// CCL API functions
// ============================================================================



void cclSetMeasurementPreStartHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementPreStartHandler = function;
  }
}



void cclSetMeasurementStartHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementStartHandler = function;
  }
}



void cclSetMeasurementStopHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementStopHandler = function;
  }
}



void cclSetDllUnloadHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mDllUnloadHandler = function;
  }
}



void cclWrite(const char* text)
{
  if (gVIAService!=NULL)
  {
    VIAResult rc = gVIAService->WriteString(text);
  }
}



void cclPrintf(const char* format, ...)
{
  char buffer[4096];
  va_list arg;
  int result;

  va_start(arg, format);
  result = _vsnprintf_s( buffer, 4096, _TRUNCATE, format, arg);
  va_end(arg);


  if (result<0)
  {
    return;
  }

  if (gVIAService!=NULL)
  {
    VIAResult rc = gVIAService->WriteString(buffer);
  }
};



int cclGetUserFilePath(const char* filename,
                       char* pathBuffer,
                       int pathBufferLength)
{
  if (gVIAService==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (gVIAMinorVersion<41)
  {
    return CCL_NOTIMPLEMENTED;
  }
  if (filename==NULL && strlen(filename)==0)
  {
    return CCL_INVALIDNAME;
  }

  VIAResult rc = gVIAService->GetUserFilePath(filename, pathBuffer, pathBufferLength);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectNotFound:
    return CCL_USERFILENOTDEFINED;
  case kVIA_ParameterInvalid:
    return CCL_PARAMETERINVALID;
  case kVIA_BufferToSmall:
    return CCL_BUFFERTOSMALL;
  case kVIA_ServiceNotRunning:
    return CCL_WRONGSTATE;
  case kVIA_Failed:
    return CCL_INTERNALERROR;
  default:
    return CCL_INTERNALERROR;
  }
}


// ============================================================================
// Time & Timer 
// ============================================================================


long long cclTimeSeconds(long long seconds)
{
  return seconds * 1000000000LL;
}


long long cclTimeMilliseconds(long long milliseconds)
{
  return milliseconds * 1000000LL;
}


long long cclTimeMicroseconds(long long microseconds)
{
  return microseconds * 1000LL;
}


int cclTimerCreate( void (*function)(long long time, int timerID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement)
  if (gModule==NULL || gVIAService==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  VTimer* cclTimer = new VTimer;
  VIATimer* viaTimer = NULL;
  
  VIAResult rc = gVIAService->CreateTimer(&viaTimer, gMasterLayer->mNode, cclTimer, "C-Library Timer");
  if (rc!=kVIA_OK)
  {
    delete cclTimer;
    return CCL_INTERNALERROR;
  }

  cclTimer->mCallbackFunction = function;
  cclTimer->mViaTimer = viaTimer;
  cclTimer->mTimerID = static_cast<int>(gModule->mTimer.size());
  gModule->mTimer.push_back(cclTimer);

  return cclTimer->mTimerID;
}



int cclTimerSet(int timerID, long long nanoseconds)
{
  CCL_STATECHECK(eRunMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (nanoseconds<=0)
  {
    return CCL_INVALIDTIME;
  }

  if (timerID < 0  ||  timerID >= static_cast<int>(gModule->mTimer.size()))
  {
    return CCL_INVALIDTIMERID;
  }

  VTimer* cclTimer = gModule->mTimer[timerID];
  if (cclTimer==NULL || cclTimer->mTimerID!=timerID)
  {
    return CCL_INVALIDTIMERID;
  }

  VIAResult rc = cclTimer->mViaTimer->SetTimer(nanoseconds);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}



int cclTimerCancel(int timerID)
{
  CCL_STATECHECK(eRunMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }

  if (timerID < 0  ||  timerID >= static_cast<int>(gModule->mTimer.size()))
  {
    return CCL_INVALIDTIMERID;
  }

  VTimer* cclTimer = gModule->mTimer[timerID];
  if (cclTimer==NULL || cclTimer->mTimerID!=timerID)
  {
    return CCL_INVALIDTIMERID;
  }

  VIAResult rc = cclTimer->mViaTimer->CancelTimer();
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}



// ============================================================================
// System Variables
// ============================================================================


int cclSysVarGetID(char* systemVariableName)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  if (gSysVarRoot==NULL || gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (systemVariableName==NULL && strlen(systemVariableName)==0)
  {
    return CCL_INVALIDNAME;
  }

  bool isStructMember = false;
  char* varName = NULL;
  char* memberName = NULL;
  {
    size_t pos = strcspn(systemVariableName, ".[");
    size_t len = strlen(systemVariableName);
    if (pos<len)
    {
      isStructMember = true;
      if(systemVariableName[pos]=='.')
      {
        memberName = systemVariableName+pos+1; // exclude the '.'
      }
      else
      {
        memberName = systemVariableName+pos; // include the '['
      }
      
      size_t varNameLength = pos;
      if (varNameLength==0)
      {
        return CCL_INVALIDNAME;
      }
      if (strlen(memberName)==0)
      {
        return CCL_INVALIDNAME;
      }
      if (gVIAMinorVersion<50)
      {
        return CCL_NOTIMPLEMENTED;
      }
      varName = (char*)malloc(varNameLength+1);
      if (varName==NULL)
      {
        return CCL_INTERNALERROR;
      }
      strncpy_s(varName, varNameLength+1, systemVariableName, varNameLength);
    }
    else
    {
      isStructMember = false;
      varName = systemVariableName;
      memberName = NULL;
    }
  }

  VIASystemVariable* viaSysVar=NULL;
  VIAResult rc = gSysVarRoot->GetVariable( varName, viaSysVar);
  if (isStructMember)
  {
    free(varName);
    varName=NULL;
  }
  if (rc!=kVIA_OK || viaSysVar==NULL)
  {
    if (rc==kVIA_ObjectNotFound)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  VIASystemVariableMember* viaSysVarMember=NULL;
  if (isStructMember)
  {
    rc = viaSysVar->GetMember(memberName, &viaSysVarMember);
    if (rc!=kVIA_OK || viaSysVar==NULL)
    {
      viaSysVar->Release();
      viaSysVar=NULL;
      if (rc==kVIA_ObjectNotFound)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  VSysVar* cclSysVar = new VSysVar;
  cclSysVar->mViaSysVar = viaSysVar;
  cclSysVar->mViaSysVarMember = viaSysVarMember;
  cclSysVar->mVariableName = (varName != NULL) ? varName : "";
  cclSysVar->mMemberName = (memberName != NULL) ? memberName : "";
  cclSysVar->mCallbackFunction = NULL;
  cclSysVar->mSysVarID = static_cast<int>(gModule->mSysVar.size());
  gModule->mSysVar.push_back(cclSysVar);

  return cclSysVar->mSysVarID;
}


int cclSysVarGetType(int sysVarID)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIASysVarType type;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIAResult rc;
    rc = cclSysVar->mViaSysVarMember->GetType(&type);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectNotFound)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    VIAResult rc;
    rc = cclSysVar->mViaSysVar->GetType(&type);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  switch(type)
  {
  case kVIA_SVInteger:
    return CCL_SYSVAR_INTEGER;
  case  kVIA_SVFloat:
    return CCL_SYSVAR_FLOAT;
  case  kVIA_SVString:
    return CCL_SYSVAR_STRING;
  case  kVIA_SVIntegerArray:
    return CCL_SYSVAR_INTEGERARRAY;
  case  kVIA_SVFloatArray:
    return CCL_SYSVAR_FLOATARRAY;
  case  kVIA_SVData:
    return CCL_SYSVAR_DATA;
  case  kVIA_SVStruct:
    return CCL_SYSVAR_STRUCT;
  case  kVIA_SVGenericArray:
    return CCL_SYSVAR_GENERICARRAY;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSysVarSetHandler(int sysVarID, void (*function)(long long time, int sysVarID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  
  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  cclSysVar->mCallbackFunction = function;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIAOnSysVarMember* sink = (cclSysVar->mCallbackFunction == NULL) ? NULL : cclSysVar;
    rc = cclSysVar->mViaSysVarMember->SetSink(sink);
  }
  else
  {
    VIAOnSysVar* sink = (cclSysVar->mCallbackFunction == NULL) ? NULL : cclSysVar;
    rc = cclSysVar->mViaSysVar->SetSink(sink);
  }

  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclSysVarGetArraySize(int sysVarID)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  // check that system variable has an array type
  VIASysVarType varType;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetType(&varType);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (varType!=kVIA_SVIntegerArray && varType!=kVIA_SVFloatArray && varType!=kVIA_SVData && varType!=kVIA_SVGenericArray)
  {
    return CCL_WRONGTYPE;
  }

  int32 size;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray && size > 0)
    {
      std::string elementName = cclSysVar->mMemberName;
      elementName += "[0]";
      
      VIASystemVariableMember* arrayElement;
      rc = cclSysVar->mViaSysVar->GetMember(elementName.c_str(), &arrayElement);
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }

      int32 bitLength = 0;
      rc = arrayElement->GetBitLength(&bitLength);
      arrayElement->Release();
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      size = ( 8 * size ) / bitLength;
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }

    if (varType==kVIA_SVGenericArray && size > 0)
    {
      std::string elementName = cclSysVar->mMemberName;
      elementName += "[0]";

      VIASystemVariableMember* arrayElement;
      rc = cclSysVar->mViaSysVar->GetMember(elementName.c_str(), &arrayElement);
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }

      int32 bitLength = 0;
      rc = arrayElement->GetBitLength(&bitLength);
      arrayElement->Release();
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      size = ( 8 * size ) / bitLength;
    }
  }

  return size;
}


int cclSysVarSetInteger(int sysVarID, long x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetInteger(static_cast<::int64>(x), NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetInteger(x, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetInteger(int sysVarID, long* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  
  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    ::int64 y;
    rc = cclSysVar->mViaSysVarMember->GetInteger(&y);
    *x = static_cast<long>(y);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetInteger(x);
  }

  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  
  return CCL_SUCCESS;
}


int cclSysVarSetFloat(int sysVarID, double x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloat, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetFloat(x, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetFloat(x, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetFloat(int sysVarID, double* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloat, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetFloat(x);
  }
  else
  {
    rc = rc = cclSysVar->mViaSysVar->GetFloat(x);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetString(int sysVarID, const char* text)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVString, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetString(text, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetString(text, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetString(int sysVarID, char* buffer, int bufferLenght)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVString, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetString(buffer, bufferLenght);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetString(buffer, bufferLenght);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else if (rc==kVIA_BufferToSmall)
    {
      return CCL_BUFFERTOSMALL;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetIntegerArray(int sysVarID, const long* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVIntegerArray, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  int32 size;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    { 
      VIAResult rc = cclSysVar->mViaSysVarMember->GetType(&varType);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray)
    {
      size = cclSysVarGetArraySize(sysVarID);
      if (size<0)
      {
        return size;
      }
      rc = kVIA_OK;
    }
    else
    {
      rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (length!=size)
  {
    return CCL_WRONGARRAYSIZE;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }

    if (varType==kVIA_SVGenericArray)
    {
      char memberName[1024];
      char arryIndex[24];
      rc = cclSysVar->mViaSysVarMember->GetName(memberName, 1000);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
      size_t memberNameLength = strlen(memberName);

      for (int i=0; i<length; i++)
      {
        sprintf_s(arryIndex, 24, "[%d]", i);
        strcpy_s(memberName+memberNameLength, 24, arryIndex);

        VIASystemVariableMember* element = NULL;
        cclSysVar->mViaSysVar->GetMember(memberName, &element);
        if (rc!=kVIA_OK)
        {
          return CCL_INTERNALERROR;
        }

        VIASysVarType elementType;
        rc = element->GetType(&elementType);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        if (elementType!=kVIA_SVInteger)
        {
          element->Release();
          return CCL_WRONGTYPE;
        }

        ::int64 elementValue = static_cast<int64>(x[i]);;
        rc = element->SetInteger(elementValue, NULL);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }

        element->Release();
      }
    }
    else if (varType==kVIA_SVIntegerArray)
    {
      ::int64* buffer = static_cast<::int64*>( alloca (length * sizeof(::int64) ) );
      for (int i=0; i<length; i++)
      {
        buffer[i] = static_cast<::int64>(x[i]);
      }
      rc = cclSysVar->mViaSysVarMember->SetIntegerArray(buffer, length, NULL);
      if (rc!=kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
    }
    else
    {
      return CCL_WRONGTYPE;
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetIntegerArray(x, length, NULL);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetIntegerArray(int sysVarID, long* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVIntegerArray, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  if (length<0)
  {
    return CCL_BUFFERTOSMALL;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    { 
      VIAResult rc = cclSysVar->mViaSysVarMember->GetType(&varType);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray)
    {
      int32 arraySize = cclSysVarGetArraySize(sysVarID);
      if (arraySize < 0)
      {
        return arraySize;
      }
      if (arraySize>length)
      {
        return CCL_BUFFERTOSMALL;
      }

      char memberName[1024];
      char arryIndex[24];
      VIAResult rc = cclSysVar->mViaSysVarMember->GetName(memberName, 1000);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
      size_t memberNameLength = strlen(memberName);

      for (int i=0; i<arraySize; i++)
      {
        sprintf_s(arryIndex, 24, "[%d]", i);
        strcpy_s(memberName+memberNameLength, 24, arryIndex);

        VIASystemVariableMember* element = NULL;
        cclSysVar->mViaSysVar->GetMember(memberName, &element);
        if (rc!=kVIA_OK)
        {
          return CCL_INTERNALERROR;
        }
        
        VIASysVarType elementType;
        rc = element->GetType(&elementType);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        if (elementType!=kVIA_SVInteger)
        {
          element->Release();
          return CCL_WRONGTYPE;
        }

        ::int64 elementValue;
        rc = element->GetInteger(&elementValue);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        x[i] = static_cast<long>(elementValue);

        element->Release();
      }
    }
    else if (varType==kVIA_SVIntegerArray)
    {
      ::int64* buffer = static_cast<::int64*>( alloca (length * sizeof(::int64) ) );
      VIAResult rc = cclSysVar->mViaSysVarMember->GetIntegerArray(buffer, length);
      if (rc!=kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else if (rc==kVIA_BufferToSmall)
        {
          return CCL_BUFFERTOSMALL;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      for (int i=0; i<length; i++)
      {
        x[i] = static_cast<long>(buffer[i]);
      }
    }
    else
    {
      return CCL_WRONGTYPE;
    }
  }
  else
  {
    VIAResult rc = cclSysVar->mViaSysVar->GetIntegerArray(x, length);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else if (rc==kVIA_BufferToSmall)
      {
        return CCL_BUFFERTOSMALL;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetFloatArray(int sysVarID, const double* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloatArray, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  int32 size;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (length!=size)
  {
    return CCL_WRONGARRAYSIZE;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetFloatArray(x, length, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetFloatArray(x, length, NULL);
  }
  
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetFloatArray(int sysVarID, double* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloatArray, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetFloatArray(x, length);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetFloatArray(x, length);
  }

  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else if (rc==kVIA_BufferToSmall)
    {
      return CCL_BUFFERTOSMALL;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetData(int sysVarID, const unsigned char* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  if (gVIAMinorVersion<42)
  {
    return CCL_NOTIMPLEMENTED;
  }

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVData, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetData(x, length, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetData(x, length, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetData(int sysVarID, unsigned char* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  if (gVIAMinorVersion<42)
  {
    return CCL_NOTIMPLEMENTED;
  }

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVData, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  int32 bytesCopied;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    int32 arraySize=0;
    VIAResult rc = cclSysVar->mViaSysVarMember->GetArraySize(&arraySize);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (arraySize>length)
    {
      return CCL_BUFFERTOSMALL;
    }
    rc = cclSysVar->mViaSysVarMember->GetData(x, arraySize);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    bytesCopied = arraySize;
  }
  else
  {
    VIAResult rc = cclSysVar->mViaSysVar->GetData(x, length, &bytesCopied);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else if (rc==kVIA_BufferToSmall)
      {
        return CCL_BUFFERTOSMALL;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return bytesCopied;
}



int cclSysVarSetPhysical(int sysVarID, double x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
    result = cclSysVar->CheckWriteable();
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }
    if (varType!=kVIA_SVInteger && varType!=kVIA_SVFloat)
    {
      return CCL_WRONGTYPE;
    }

    rc = cclSysVar->mViaSysVarMember->SetPhysicalValue(x, NULL);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    return CCL_NOTIMPLEMENTED;  // physical value is only implemented for member of struct 
  }

  return CCL_SUCCESS;
}


int cclSysVarGetPhysical(int sysVarID, double* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }
    if (varType!=kVIA_SVInteger && varType!=kVIA_SVFloat)
    {
      return CCL_WRONGTYPE;
    }

    rc = cclSysVar->mViaSysVarMember->GetPhysicalValue(x);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    return CCL_NOTIMPLEMENTED;  // physical value is only implemented for member of struct 
  }

  return CCL_SUCCESS;
}



// ============================================================================
// Signal
// ============================================================================

int cclSignalGetID(char* signalname)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (gSignalAccessAPI==NULL)
  {
    return CCL_NOTIMPLEMENTED;
  }
  if (signalname==NULL && strlen(signalname)==0)
  {
    return CCL_INVALIDNAME;
  }

  VIASignal* viaSignal=NULL;
  VIAResult rc = gSignalAccessAPI->GetSignalByName(&viaSignal, signalname);
  if (rc!=kVIA_OK || viaSignal==NULL)
  {
    if (rc==kVIA_ObjectNotFound)
    {
      return CCL_SIGNALNOTDEFINED;
    }
    else if (rc==kVIA_SignalAmbiguous)
    {
      return CCL_SIGNALAMBIGUOUS;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  VSignal* cclSignal = new VSignal;
  cclSignal->mViaSignal = viaSignal;
  cclSignal->mSignalID = static_cast<int>(gModule->mSignal.size());
  gModule->mSignal.push_back(cclSignal);

  return cclSignal->mSignalID;
}


extern int cclSignalSetHandler(int signalID, void (*function)(int signalID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  cclSignal->mCallbackFunction = function;
  VIAOnSignal* sink = (cclSignal->mCallbackFunction == NULL) ? NULL : cclSignal;
  VIAResult rc = cclSignal->mViaSignal->SetSinkOnChangeOnly(sink, NULL);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclSignalGetRxPhysDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetPhysicalValue(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetRxRawInteger(int signalID, long long* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetRxRawDouble (int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxPhysDouble(int signalID, double value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetPhysicalValue(value);
  switch (rc)
  {  
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal is read only or has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxRawInteger(int signalID, long long value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxRawDouble(int signalID, double value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}



int cclSignalGetTxPhysDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxPhysicalValue(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal is read only or has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetTxRawInteger(int signalID, long long* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetTxRawDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


// ============================================================================
// CAN 
// ============================================================================


int cclCanOutputMessage(int channel, unsigned int identifier, unsigned int flags, int dlc, const unsigned char data[])
{
  CCL_STATECHECK(eRunMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gCanBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  flags = flags & (CCL_CANFLAGS_RTR|CCL_CANFLAGS_WAKEUP);

  VIAResult rc = gCanBusContext[channel].mBus->OutputMessage(channel, identifier, flags, dlc, data);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclCanSetMessageHandler(int channel, unsigned int identifier, void (*function) (cclCanMessage* message) )
{
  CCL_STATECHECK(eInitMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gCanBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  uint8 requestType = (identifier==CCL_CAN_ALLMESSAGES) ? kVIA_AllId : kVIA_OneId;

  VCanMessageRequest* request = new VCanMessageRequest;
  request->mCallbackFunction = function;
  request->mContext = &gCanBusContext[channel];
  request->mHandle = NULL;

  VIAResult rc = gCanBusContext[channel].mBus->CreateMessageRequest(&(request->mHandle), request, requestType, identifier, channel, 0);
  if (rc!=kVIA_OK)
  {
    delete request;
    if (rc==kVIA_ServiceNotRunning)
    {
      return CCL_WRONGSTATE;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  gModule->mCanMessageRequests.push_back(request);
  return CCL_SUCCESS;
}



unsigned int cclCanMakeExtendedIdentifier(unsigned int identifier)
{
  return identifier | 0x80000000;
}


unsigned int cclCanMakeStandardIdentifier(unsigned int identifier)
{
  return identifier & 0x7FFFFFFF;
}


unsigned int cclCanValueOfIdentifier(unsigned int identifier)
{
  return identifier & 0x7FFFFFFF;
}


int cclCanIsExtendedIdentifier(unsigned int identifier)
{
  return ((identifier & 0x80000000) == 0x80000000) ? 1 : 0;
}


int cclCanIsStandardIdentifier(unsigned int identifier)
{
  return ((identifier & 0x80000000) == 0x00000000) ? 1 : 0;
}


// ============================================================================
// LIN 
// ============================================================================


int cclLinSetFrameHandler(int channel, 
                          unsigned int identifier, 
                          void (*function) (struct cclLinFrame* frame) )
{
  CCL_STATECHECK(eInitMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  uint8 requestType = (identifier==CCL_LIN_ALLMESSAGES) ? kVIA_AllId : kVIA_OneId;

  VLinMessageRequest* request = new VLinMessageRequest;
  request->mCallbackFunction = function;
  request->mContext = &gLinBusContext[channel];
  request->mHandle = NULL;

  VIAResult rc = gLinBusContext[channel].mBus->CreateMessageRequest2(&(request->mHandle), request, requestType, identifier, channel);
  if (rc!=kVIA_OK)
  {
    delete request;
    if (rc==kVIA_ServiceNotRunning)
    {
      return CCL_WRONGSTATE;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  gModule->mLinMessageRequests.push_back(request);
  return CCL_SUCCESS;
}


int cclLinUpdateResponseData(int           channel,
                             unsigned int  id,
                             unsigned char dlc,
                             unsigned char data[8])
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  uint32 flags = kVIA_ReconfigureData;
  int32 response = -2;
  VIAResult rc = gLinBusContext[channel].mBus->OutputMessage(channel, id, flags, dlc, data, response);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


extern int cclLinSendHeader(int channel, unsigned int id)
{
  CCL_STATECHECK(eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  uint32 flags = kVIA_ApplyHeader;
  int32 response = 0;
  uint8 dlc =0;
  uint8 data[8];
  VIAResult rc = gLinBusContext[channel].mBus->OutputMessage(channel, id, flags, dlc, data, response);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinStartScheduler(int channel)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINStartScheduler(channel);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinStopScheduler(int channel)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINStopScheduler(channel);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinChangeSchedtable(int channel, unsigned int tableIndex)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINChangeSchedtableEx(channel, tableIndex);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}