// C_Library.cpp : Example of a CANoe C Library
//
// 这个例程演示如何使用CCL接口函数实现CAPL语言的一些功能
// 例如：报文发送、接收处理、定时器

#include "../CCL/CCL.h"

extern void OnDllUnLoad();
extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnTimer(long long time, int timerID);
extern void OnCanMessage0x101(struct cclCanMessage* message);
extern void OnCanMessage0x102x(struct cclCanMessage* message);
extern void OnSysVar_Cal(long long time, int sysVarID);

int gTimerID;  //定义定时器ID
int counter;   //定义计数变量，用于CAN报文发送字节的递增

//此函数是CCL库的入口函数，必须包含。
void cclOnDllLoad()
{
	//添加测量preStart回调函数
	cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
	//添加测量Start回调函数
	cclSetMeasurementStartHandler(&OnMeasurementStart);
}

//CANoe测量PreStart时将调用该回调函数
void OnMeasurementPreStart()
{
	int rc;
	int sysVarID_Cal;
	counter = 0;
	gTimerID = cclTimerCreate(&OnTimer); //创建一个定时器

	//创建两个回调函数，分别针对0x101和0x102x, 与CANoe中on message事件功能一样
	rc = cclCanSetMessageHandler(1, cclCanMakeStandardIdentifier(0x101), &OnCanMessage0x101);
	rc = cclCanSetMessageHandler(1, cclCanMakeExtendedIdentifier(0x102), &OnCanMessage0x102x);

	//创建一个回调函数，针对系统变量Calculate的变化, 与CANoe中on sysvar事件功能一样
	sysVarID_Cal= cclSysVarGetID("CCL_Demo::Calculate");
	rc = cclSysVarSetHandler(sysVarID_Cal, &OnSysVar_Cal);
}

//CANoe测量Start时将调用该回调函数
void OnMeasurementStart()
{
	int rc;
	//设置定时器之间间隔为500ms
	rc = cclTimerSet(gTimerID, cclTimeMilliseconds(500));
}


void OnTimer(long long time, int timerID)
{
	int channel = 1;
	//定义一个扩展帧报文
	unsigned int id = cclCanMakeExtendedIdentifier(0x100); 
	unsigned int flags = 0;
	int dlc = 8;
	unsigned char data[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
	int rc;

	counter++;
	if (counter > 0xFF) counter = 0;
	data[7] = counter;	//更新data[7]
	//发送报文到总线上
	rc = cclCanOutputMessage(channel, id, flags, dlc, data);
	//设置定时器之间间隔为500ms，不能忘记！
	rc = cclTimerSet(gTimerID, cclTimeMilliseconds(500));
}

//针对CAN ID 0x101的回调函数
void OnCanMessage0x101(struct cclCanMessage* message)
{
	int isStandardIdentifier;
	unsigned int idValue;
	//输出信息到Write窗口
	cclWrite("C-API Example: OnCanMessage0x101");

	//判别是标准帧还是扩展帧报文
	isStandardIdentifier = cclCanIsStandardIdentifier(message->id);
	if (isStandardIdentifier)
		cclWrite("This is a CAN Standard Identifier.");
	else
		cclWrite("This is a CAN Exetended Identifier.");

	//读取CAN ID，并输出到Write窗口
	idValue = cclCanValueOfIdentifier(message->id);
	cclPrintf("The Value of the CAN ID is %d", idValue);
}

//针对CAN ID 0x102x的回调函数
void OnCanMessage0x102x(struct cclCanMessage* message)
{
	int isExtendedIdentifier;
	unsigned int idValue;
	//输出信息到Write窗口
	cclWrite("C-API Example: OnCanMessage0x102x");
	//判别是标准帧还是扩展帧报文
	isExtendedIdentifier = cclCanIsExtendedIdentifier(message->id);
	if (isExtendedIdentifier)
		cclWrite("This is a CAN Exetended Identifier.");
	else
		cclWrite("This is a CAN Standard Identifier.");
	//读取CAN ID，并输出到Write窗口
	idValue = cclCanValueOfIdentifier(message->id);
	cclPrintf("The Value of the CAN ID is %d", idValue);
}

//针对系统变量Calculate(即Calculate按钮状态）的回调函数
void OnSysVar_Cal(long long time, int sysVarID)
{ 
	int sysVarID_Op1;
	int sysVarID_Op2;
	int sysVarID_Res;
	long op1;
	long op2;
	long res;
	
	long cal_button;
	//读取系统变量Calculate的数值(即Calculate按钮状态）
	cclSysVarGetInteger(sysVarID,&cal_button); 
	if (cal_button)  //数值为1时（即按钮按下时）
	{
		//读取系统变量Operator1的数值，并赋值给op1,同时在Write窗口输出相关信息
		cclWrite("C Library Example: Read the value of integer system: Operator1");
		sysVarID_Op1 = cclSysVarGetID("CCL_Demo::Operator1");
		cclSysVarGetInteger(sysVarID_Op1, &op1);
		cclPrintf("The Current Value of the Operator1 is %d", op1);

		//读取系统变量Operator2的数值，并赋值给op2,同时在Write窗口输出相关信息
		cclWrite("C Library Example: Read the value of integer system: Operator2");
		sysVarID_Op2 = cclSysVarGetID("CCL_Demo::Operator2");
		cclSysVarGetInteger(sysVarID_Op2, &op2);
		cclPrintf("The Current Value of the Operator2 is %d", op2);

		//将op1-op2的结果赋值给系统变量Result,同时在Write窗口输出相关信息
		res = op1 - op2;
		cclWrite("C Library Example: Write the value of integer system: Result");
		sysVarID_Res = cclSysVarGetID("CCL_Demo::Result");
		cclSysVarSetInteger(sysVarID_Res, res);
		cclPrintf("The Current Value of the Result is %d", res);
	}
}