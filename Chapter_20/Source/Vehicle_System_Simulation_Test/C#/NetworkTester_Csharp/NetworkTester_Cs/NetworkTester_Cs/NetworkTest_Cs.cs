using System;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.CANoe.Sockets;
using Vector.Diagnostics;
using Vector.Scripting.UI;
using Vector.CANoe.TFS;
using Vector.CANoe.VTS;
using NetworkDB; //为了可以访问DBC中的报文和信号等

public class NetworkTester_Cs : TestModule
{
    //报文发送周期上下限及测试区间时间定义
    const int kMIN_CYCLE_TIME = 40;
    const int kMAX_CYCLE_TIME = 60;
    const int Light_MIN_CYCLE_TIME = 490;
    const int Light_MAX_CYCLE_TIME = 510;
    const int Lock_MIN_CYCLE_TIME = 90;
    const int Lock_MAX_CYCLE_TIME = 110;
    const int kTIMEOUT = 4000;
    public override void Main()
    {
        // test sequence定义
        //周期报文测试
        TestGroupBegin("Check msg cycle time", "Check cycle time of message");
        ChkMsgCycleTimeEngineData();
        ChkMsgCycleTimeVehicleData();
        ChkMsgCycleTimeGear_Info();
        ChkMsgCycleTimeIgnition_Info();
        ChkMsgCycleTimeLight_Info();
        TestGroupEnd();

        //报文长度测试
        TestGroupBegin("Check msg DLC", "Check the DLC of message");
        ChkMsgDLCLock_Info();
        TestGroupEnd();

        //未定义报文测试
        TestGroupBegin("Check undefined msg", "Check the undefined messages");
        CaplTestCases.CAPLTestLib.CheckUndefinedMessage(); //直接调用CAPL测试用例
        TestGroupEnd();

        //功能测试演示：引擎速度测试
        TestGroupBegin("Function test", "Check the engine speed after setup");
        ChkEngSpeed();
        TestGroupEnd();
    }

    //初始化测试条件：切忌不能跳过，否则总线可能处于offline!!!
    public void Init_Test_Condition()
    {
        Vehicle_Key.Unlock_Car.Value = 1;
        Vehicle_Key.Car_Driver.Value = 0;
        Vehicle_Key.Key_State.Value = 2;
        Execution.Wait(1000); //wait for 1s
    }

    //测试用例1：周期报文测试 - EngineData
    [TestCase("TC-1: Check cycle time of msg EngineData", "Check cycle time of message EngineData")]
    public void ChkMsgCycleTimeEngineData()
    {
        int lCycMinCycleTime;  // Minimum relative distance 
        int lCycMaxCycleTime;  // Maximum relative distance

        lCycMinCycleTime = kMIN_CYCLE_TIME;
        lCycMaxCycleTime = kMAX_CYCLE_TIME;

        Init_Test_Condition(); //运行测试话确保总线不处于offline

        Report.TestStep("Check cycle time of message EngineData");
        Report.TestCaseDescription("Check cycle time of message EngineData");
        ICheck check = new AbsoluteCycleTimeCheck<NetworkDB.Frames.EngineData>(CheckType.Condition, lCycMinCycleTime, lCycMaxCycleTime);
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例2：周期报文测试 - VehicleData
    [TestCase("TC-2: Check cycle time of msg VehicleData", "Check cycle time of message VehicleData")]
    public void ChkMsgCycleTimeVehicleData()
    {

        int lCycMinCycleTime;  // Minimum relative distance 
        int lCycMaxCycleTime;  // Maximum relative distance

        lCycMinCycleTime = kMIN_CYCLE_TIME;
        lCycMaxCycleTime = kMAX_CYCLE_TIME;

        Report.TestStep("Check cycle time of message VehicleData");
        Report.TestCaseDescription("Check cycle time of message EngineData");
        ICheck check = new AbsoluteCycleTimeCheck<NetworkDB.Frames.VehicleData>(CheckType.Condition, lCycMinCycleTime, lCycMaxCycleTime);
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例3：周期报文测试 - Gear_Info
    [TestCase("TC-3: Check cycle time of message Gear_Info", "Check cycle time of message Gear_Info")]
    public void ChkMsgCycleTimeGear_Info()
    {

        int lCycMinCycleTime;  // Minimum relative distance 
        int lCycMaxCycleTime;  // Maximum relative distance

        lCycMinCycleTime = kMIN_CYCLE_TIME;
        lCycMaxCycleTime = kMAX_CYCLE_TIME;

        Report.TestStep("Check cycle time of message Gear_Info");
        Report.TestCaseDescription("Check cycle time of message Gear_Info");
        ICheck check = new AbsoluteCycleTimeCheck<NetworkDB.Frames.Gear_Info>(CheckType.Condition, lCycMinCycleTime, lCycMaxCycleTime);
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例4：周期报文测试 - Ignition_Info
    [TestCase("TC-4: Check cycle time of message Ignition_Info", "Check cycle time of message Ignition_Info")]
    public void ChkMsgCycleTimeIgnition_Info()
    {

        int lCycMinCycleTime;  // Minimum relative distance 
        int lCycMaxCycleTime;  // Maximum relative distance

        lCycMinCycleTime = kMIN_CYCLE_TIME;
        lCycMaxCycleTime = kMAX_CYCLE_TIME;

        Report.TestStep("Check cycle time of message Ignition_Info");
        Report.TestCaseDescription("Check cycle time of message Ignition_Info");
        ICheck check = new AbsoluteCycleTimeCheck<NetworkDB.Frames.Ignition_Info>(CheckType.Condition, lCycMinCycleTime, lCycMaxCycleTime);
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例5：周期报文测试 - Light_Info
    [TestCase("TC-5: Check cycle time of message Light_Info", "Check cycle time of message Light_Info")]
    public void ChkMsgCycleTimeLight_Info()
    {

        int lCycMinCycleTime;  // Minimum relative distance 
        int lCycMaxCycleTime;  // Maximum relative distance

        lCycMinCycleTime = Light_MIN_CYCLE_TIME;
        lCycMaxCycleTime = Light_MAX_CYCLE_TIME;

        Report.TestStep("Check cycle time of message Light_Info");
        Report.TestCaseDescription("Check cycle time of message Light_Info");
        ICheck check = new AbsoluteCycleTimeCheck<NetworkDB.Frames.Light_Info>(CheckType.Condition, lCycMinCycleTime, lCycMaxCycleTime);
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例6：报文长度测试 - Lock_Info
    [TestCase("TC-6: Check DLC of message Lock_Info", "Check DLC of message Lock_Info")]
    public void ChkMsgDLCLock_Info()
    {

        Report.TestStep("Check DLC of message Lock_Info");
        Report.TestCaseDescription("Check DLC of message Lock_Info");
        ICheck check = new DlcCheck<NetworkDB.Frames.Lock_Info>();
        check.Activate();
        Execution.Wait(kTIMEOUT); // observation active
        check.Deactivate();

    }

    //测试用例8：功能测试演示 - 引擎速度测试
    [TestCase("TC-8: Check Engine Speed", "Check the engine speed after setup")]
    public void ChkEngSpeed()
    {
        Report.TestStep("Check Engine Speed");
        Report.TestCaseDescription("Check the engine speed after setup");
        Vehicle_Key.Unlock_Car.Value = 1;
        Vehicle_Key.Car_Driver.Value = 0;
        Vehicle_Key.Key_State.Value = 2;
        Vehicle_Control.Eng_Speed.Value = 2000;
        Execution.Wait(kTIMEOUT); // Wait for 4000 ms

        if (Execution.Wait<NetworkDB.EngSpeed>(2000, 500) == 1)
            Report.TestStepPass("Engine Speed is Correct!");
        else
            Report.TestStepFail("Engine Speed is Not Correct!");
    }
}
