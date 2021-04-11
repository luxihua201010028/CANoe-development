/*------------------------------------------------------------------ 
<FILE> 
  <NAME> CCL.h
  <PRJ > CANalyzer/CANoe C Library API (CCL)
  <REV > 1.6.3
  <DATE> 2015-09-17
  <DESC> 
   The header file declares the API of an CANalyzer/CANoe C Library (CCL).
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

#ifndef CCL_H
#define CCL_H

#if defined __cplusplus
extern "C" {
#endif

// ============================================================================
// Error Codes
// ============================================================================

#define CCL_SUCCESS                      0
#define CCL_WRONGSTATE                  -1
#define CCL_INTERNALERROR               -2
#define CCL_INVALIDCHANNEL              -3
#define CCL_INVALIDTIMERID              -4
#define CCL_INVALIDFUNCTIONPOINTER      -5
#define CCL_INVALIDTIME                 -6
#define CCL_INVALIDNAME                 -7
#define CCL_SYSVARNOTDEFINED            -8
#define CCL_INVALIDSYSVARID             -9
#define CCL_WRONGTYPE                  -10
#define CCL_SYSVARISREADONLY           -11
#define CCL_BUFFERTOSMALL              -12
#define CCL_WRONGARRAYSIZE             -13
#define CCL_NOTIMPLEMENTED             -14
#define CCL_SIGNALNOTDEFINED           -15
#define CCL_SIGNALAMBIGUOUS            -16
#define CCL_INVALIDSIGNALID            -17
#define CCL_USERFILENOTDEFINED         -18
#define CCL_PARAMETERINVALID           -19

// ============================================================================
// Common defines 
// ============================================================================

#define CCL_DIR_RX                       0
#define CCL_DIR_TX                       1
#define CCL_DIR_TXRQ                     2

// ============================================================================
// General Functions
// ============================================================================

extern void cclOnDllLoad();

extern void cclSetMeasurementPreStartHandler(void (*function)());
extern void cclSetMeasurementStartHandler(void (*function)());
extern void cclSetMeasurementStopHandler(void (*function)());
extern void cclSetDllUnloadHandler(void (*function)());

extern void cclWrite(const char* text);
extern void cclPrintf(const char* format, ...);

extern int cclGetUserFilePath(const char* filename,
                              char* pathBuffer,
                              int pathBufferLength);

// ============================================================================
// Time & Timer 
// ============================================================================

extern long long cclTimeSeconds(long long seconds);
extern long long cclTimeMilliseconds(long long milliseconds);
extern long long cclTimeMicroseconds(long long microseconds);

extern int cclTimerCreate( void (*function)(long long time, int timerID) );
extern int cclTimerSet(int timerID, long long nanoseconds);
extern int cclTimerCancel(int timerID);

// ============================================================================
// System Variables
// ============================================================================
#define CCL_SYSVAR_INTEGER      1
#define CCL_SYSVAR_FLOAT        2
#define CCL_SYSVAR_STRING       3
#define CCL_SYSVAR_DATA         4
#define CCL_SYSVAR_INTEGERARRAY 5
#define CCL_SYSVAR_FLOATARRAY   6
#define CCL_SYSVAR_STRUCT       7
#define CCL_SYSVAR_GENERICARRAY 8

extern int cclSysVarGetID(char* systemVariablename);
extern int cclSysVarGetType(int sysVarID);
extern int cclSysVarSetHandler(int sysVarID,
                               void (*function)(long long time, int sysVarID) );
extern int cclSysVarGetArraySize(int sysVarID);
extern int cclSysVarSetInteger(int sysVarID, long x);
extern int cclSysVarGetInteger(int sysVarID, long* x);
extern int cclSysVarSetFloat(int sysVarID, double x);
extern int cclSysVarGetFloat(int sysVarID, double* x);
extern int cclSysVarSetString(int sysVarID, const char* text);
extern int cclSysVarGetString(int sysVarID, char* buffer, int bufferLenght);
extern int cclSysVarSetIntegerArray(int sysVarID, const long* x, int length);
extern int cclSysVarGetIntegerArray(int sysVarID, long* x, int length);
extern int cclSysVarSetFloatArray(int sysVarID, const double* x, int length);
extern int cclSysVarGetFloatArray(int sysVarID, double* x, int length);
extern int cclSysVarSetData(int sysVarID, const unsigned char* x, int length);
extern int cclSysVarGetData(int sysVarID, unsigned char* x, int length);
extern int cclSysVarSetPhysical(int sysVarID, double x);
extern int cclSysVarGetPhysical(int sysVarID, double* x);

// ============================================================================
// Signal
// ============================================================================

extern int cclSignalGetID(char* signalname);
extern int cclSignalSetHandler(int signalID, void (*function)(int signalID) );

extern int cclSignalGetRxPhysDouble(int signalID, double* value);
extern int cclSignalGetRxRawInteger(int signalID, long long* value);
extern int cclSignalGetRxRawDouble(int signalID, double* value);

extern int cclSignalSetTxPhysDouble(int signalID, double value);
extern int cclSignalSetTxRawInteger(int signalID, long long value);
extern int cclSignalSetTxRawDouble(int signalID, double value);

extern int cclSignalGetTxPhysDouble(int signalID, double* value);
extern int cclSignalGetTxRawInteger(int signalID, long long* value);
extern int cclSignalGetTxRawDouble(int signalID, double* value);

// ============================================================================
// CAN 
// ============================================================================

#define CCL_CANFLAGS_RTR           1
#define CCL_CANFLAGS_WAKEUP        2
#define CCL_CANFLAGS_TE            4

extern int cclCanOutputMessage(int channel,
                               unsigned int identifier,
                               unsigned int flags,
                               int dlc,
                               const unsigned char data[]);


#define CCL_CAN_ALLMESSAGES    0xFFFFFFFF

struct cclCanMessage
{
  long long     time;
  int           channel;
  unsigned int  id;
  unsigned int  flags;
  unsigned char dir;
  unsigned char dlc;
  unsigned char data[8];
};

extern int cclCanSetMessageHandler(int channel,
                                   unsigned int identifier,
                                   void (*function) (struct cclCanMessage* message) );

extern unsigned int cclCanMakeExtendedIdentifier(unsigned int identifier);
extern unsigned int cclCanMakeStandardIdentifier(unsigned int identifier);
extern unsigned int cclCanValueOfIdentifier(unsigned int identifier);
extern int cclCanIsExtendedIdentifier(unsigned int identifier);
extern int cclCanIsStandardIdentifier(unsigned int identifier);

// ============================================================================
// LIN
// ============================================================================

struct cclLinFrame
{
  long long     timestampEOF;
  long long     timestampSOF;
  long long     timestampEOH;
  long long     timeSyncBreak;
  long long     timeSyncDel;
  int           channel;
  unsigned int  id;
  unsigned char dir;
  unsigned char dlc;
  unsigned char data[8];
};

#define CCL_LIN_ALLMESSAGES    0xFFFFFFFF

extern int cclLinSetFrameHandler(int channel,
                                 unsigned int identifier,
                                 void (*function) (struct cclLinFrame* frame) );


extern int  cclLinUpdateResponseData(int channel,
                                     unsigned int id,
                                     unsigned char dlc,
                                     unsigned char data[8]);

extern int  cclLinSendHeader(int channel, unsigned int id);
extern int  cclLinStartScheduler(int channel);
extern int  cclLinStopScheduler(int channel);
extern int  cclLinChangeSchedtable(int channel, unsigned int tableIndex);


#if defined __cplusplus
}
#endif

#endif // CCL_H
