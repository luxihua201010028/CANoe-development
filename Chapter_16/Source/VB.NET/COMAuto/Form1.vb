Imports CANoe
Imports System.IO

Public Class COMAuto_VB
#Region "声明"
    ' 定义CANoe对象
    Private WithEvents mCANoeApp As Application
    Private WithEvents mCANoeMeasurement As Measurement

    ' 定义CANoe系统变量   
    Private WithEvents mCANoeSysGear As Variable
    Private WithEvents mCANoeSysKeyState As Variable
    Private WithEvents mCANoeSysUnlockCar As Variable
    Private WithEvents mCANoeSysLockCar As Variable
    Private WithEvents mCANoeSysCarDriver As Variable

    ' 定义CANoe总线信号变量    
    Private mCANoeEngineSpeed As Signal
    Private mCANoeVehicleSpeed As Signal

    ' 定义Configuration的绝对路径和相对路径
    Dim mAbsoluteConfigPath As String
    Private Const mConfigPath As String = "..\..\..\..\CAN_Simulation\CAN_Simulation.cfg"


    ' 定义委托函数
    Private Delegate Sub DelSafeInvocation()
    Private Delegate Sub DelSafeInvocationWithParam(sysvarName As String, value As Object)

    ' 定义CANoe运行实例
    Private mCANoeInstanceRunning As Boolean

    ' GUI控件变化防抖
    Private localchangeflag As Integer
    '1 - trackbarGear
    '2 - trackbarKeyState
    '3 - trackbarVehicleSpeed.Value
    '4 - trackbarEngSpeed.Value
    '0 - 无变化

    ' 定义一个CAPL函数别名
    Private mCANoeMultiply As CAPLFunction
    Private lockdelay As Integer
    '0 - lock or unlock button is pressed
    '1 - delay 1 interval
    '2 - delay 2 intervals/ will change the button status to normal
    '>2 - no delay is ongoing

#End Region

#Region "初始化"
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        ' 获取Configuration的绝对路径
        mAbsoluteConfigPath = Path.GetFullPath(mConfigPath)
        txtPath.Text = mAbsoluteConfigPath

        '初始化控件
        btnExit.Enabled = False
        btnMeasurement.Enabled = False
        groupboxSysVar.Enabled = False
        groupboxSignal.Enabled = False
        groupboxCAPL.Enabled = False
        ' 检查Configuration的路径是否存在
        If File.Exists(mAbsoluteConfigPath) Then
            lblStatus.ForeColor = Color.Blue
            lblStatus.Text = "Configuration路径有效"
            btnOpen.Enabled = True
        Else
            lblStatus.ForeColor = Color.Red
            lblStatus.Text = "Configuration路径无效！"
            btnOpen.Enabled = False
            btnExit.Enabled = False
        End If

        localchangeflag = 0

    End Sub
#End Region
#Region "CANoe初始化和退出"

    ' ConfigurationOpened()函数：Configuration成功打开以后，将被调用
    Private Sub ConfigurationOpened()
        Try
            '将CANoe中的命名空间变量赋给变量参数
            Dim CANoeNamespaceVehControl As CANoe.Namespace = mCANoeApp.System.Namespaces("Vehicle_Control")
            Dim CANoeNamespaceVehKey As CANoe.Namespace = mCANoeApp.System.Namespaces("Vehicle_Key")
            mCANoeSysGear = CANoeNamespaceVehControl.Variables("Gear")
            mCANoeSysKeyState = CANoeNamespaceVehKey.Variables("Key_State")
            mCANoeSysCarDriver = CANoeNamespaceVehKey.Variables("Car_Driver")
            mCANoeSysUnlockCar = CANoeNamespaceVehKey.Variables("Unlock_Car")
            mCANoeSysLockCar = CANoeNamespaceVehKey.Variables("Lock_Car")

            ' 将CANoe中的总线信号赋给变量参数
            Dim CANoeBus As Bus = mCANoeApp.Bus("CAN")
            mCANoeEngineSpeed = CANoeBus.GetSignal(1, "EngineData", "EngSpeed")
            mCANoeVehicleSpeed = CANoeBus.GetSignal(1, "VehicleData", "VehicleSpeed")

        Catch ex As Exception
            MessageBox.Show("Possible cause: Wrong namespace names, bus name, system variable names or signal names in source code or configuration.",
                            "Error while assigning system variables and signals", MessageBoxButtons.OK, MessageBoxIcon.Hand)

            Return
        End Try

        btnOpen.Enabled = False
        btnMeasurement.Enabled = True
        btnExit.Enabled = True
        If (Not mCANoeApp Is Nothing) Then
            ' 连接CANoe退出事件的处理函数CANoeQuit
            AddHandler mCANoeApp.OnQuit, AddressOf CANoeQuit
        End If

        If (Not mCANoeMeasurement Is Nothing) Then
            ' 连接测量初始化事件的处理函数MeasurementInitiated
            AddHandler mCANoeMeasurement.OnInit, AddressOf MeasurementInitiated
            ' 连接测量停止事件的处理函数MeasurementExited
            AddHandler mCANoeMeasurement.OnExit, AddressOf MeasurementExited
        End If

        ' 更新状态信息
        lblStatus.Text = "Configuration打开成功"
        mCANoeInstanceRunning = True
    End Sub

    ' MeasurementInitiated(): measurement初始化时调用
    Private Sub MeasurementInitiated()

        ' 编译Configuration文件的CAPL代码
        Dim CANoeCAPL As CANoe.CAPL = mCANoeApp.CAPL
        If (Not CANoeCAPL Is Nothing) Then
            CANoeCAPL.Compile(Nothing)
        End If

        Try
            ' 获取CAPL函数接口
            mCANoeMultiply = CANoeCAPL.GetFunction("Multiply")

        Catch ex As Exception
            MessageBox.Show("Possible cause: Wrong CAPL function name in source code or configuration.",
                            "Error while assigning CAPL functions", MessageBoxButtons.OK, MessageBoxIcon.Hand)

            Return
        End Try

        ' 为了线程安全，使用委托调用
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf MeasurementInitiatedInternal)
        Invoke(safeinvocation)

        ' 连接系统变量变化的处理函数CANoeGearChanged和CANoeKeyStateChanged
        If (Not mCANoeSysGear Is Nothing) Then
            AddHandler mCANoeSysGear.OnChange, AddressOf CANoeGearChanged
        End If
        If (Not mCANoeSysKeyState Is Nothing) Then
            AddHandler mCANoeSysKeyState.OnChange, AddressOf CANoeKeyStateChanged
        End If

    End Sub


    ' MeasurementInitiatedInternal():用于measurement开始前初始化界面控件的状态
    Private Sub MeasurementInitiatedInternal()

        groupboxCAPL.Enabled = True
        groupboxSignal.Enabled = True
        groupboxSysVar.Enabled = True
        btnMeasurement.Text = "停止测试"
        lblStatus.Text = "测量进行中..."
        btnExit.Enabled = False

        trackbarGear.Value = mCANoeSysGear.Value
        trackbarKeyState.Value = mCANoeSysKeyState.Value
        trackbarVehicleSpeed.Value = mCANoeVehicleSpeed.Value
        trackbarEngSpeed.Value = mCANoeEngineSpeed.Value
        lblEngSpeed.Text = mCANoeVehicleSpeed.Value.ToString
        lblVehicleSpeed.Text = mCANoeEngineSpeed.Value.ToString
        progbarEngSpeed.Value = mCANoeEngineSpeed.Value
        progbarVehicleSpeed.Value = mCANoeVehicleSpeed.Value

        txtOp1.Text = "50"
        txtOp2.Text = "25"
        txtResult.Text = ""

        Timer1.Start()
    End Sub

    ' MeasurementExited()：测量停止后被调用
    Private Sub MeasurementExited()

        ' 为了线程安全，使用委托调用
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf MeasurementExitedInternal)
        Invoke(safeinvocation)

        ' 断开系统变量变化事件处理函数
        If (Not mCANoeSysGear Is Nothing) Then
            RemoveHandler mCANoeSysGear.OnChange, AddressOf CANoeGearChanged
        End If
        If (Not mCANoeSysKeyState Is Nothing) Then
            RemoveHandler mCANoeSysKeyState.OnChange, AddressOf CANoeKeyStateChanged
        End If

    End Sub

    ' MeasurementExitedInternal()：测量停止后被调用，更新控件显示
    Private Sub MeasurementExitedInternal()

        groupboxCAPL.Enabled = False
        groupboxSignal.Enabled = False
        groupboxSysVar.Enabled = False
        btnExit.Enabled = True
        btnMeasurement.Text = "开始测试"
        lblStatus.Text = "空闲状态"


        trackbarGear.Value = 0
        trackbarKeyState.Value = 0
        trackbarVehicleSpeed.Value = 0
        trackbarEngSpeed.Value = 0
        lblEngSpeed.Text = 0
        lblVehicleSpeed.Text = 0
        progbarEngSpeed.Value = 0
        progbarVehicleSpeed.Value = 0

        txtOp1.Text = "50"
        txtOp2.Text = "25"
        txtResult.Text = ""

        ' 停止Timer1
        Timer1.Stop()

    End Sub
#End Region

#Region "CANoe关闭处理"

    ' CANoeQuit(): CANoe退出时被调用
    Private Sub CANoeQuit()

        ' 为了线程安全，使用委托调用
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf CANoeQuitInternal)
        Invoke(safeinvocation)

        UnregisterCANoeEventHandlers()

        ' 更新CANoe运行实例状态
        mCANoeInstanceRunning = False
    End Sub

    ' CANoeQuitInternal()：CANoe退出时被调用，更新控件显示
    Private Sub CANoeQuitInternal()
        ' 更新按钮显示文字
        btnMeasurement.Text = "开始测量"
        lblStatus.Text = "测量停止"

        ' 禁止测量相关控件
        MeasurementExited()
    End Sub

    ' 断开所有CANoe事件处理函数
    Private Sub UnregisterCANoeEventHandlers()

        If (Not mCANoeApp Is Nothing) Then
            RemoveHandler mCANoeApp.OnQuit, AddressOf CANoeQuit
        End If

        If (Not mCANoeMeasurement Is Nothing) Then
            RemoveHandler mCANoeMeasurement.OnInit, AddressOf MeasurementInitiated
            RemoveHandler mCANoeMeasurement.OnExit, AddressOf MeasurementExited
        End If
    End Sub
#End Region
#Region "CANoe系统变量变化处理"
    ' CANoeGearChanged（）：系统变量变化时被调用
    Private Sub CANoeGearChanged(ByVal value As Object)
        ' ' 为了线程安全，使用委托调用
        Dim safeinvocationwithparam As DelSafeInvocationWithParam = New DelSafeInvocationWithParam(AddressOf SysVarsChangedInternal)
        Invoke(safeinvocationwithparam, "Gear", value)
    End Sub
    Private Sub CANoeKeyStateChanged(ByVal value As Object)
        ' 为了线程安全，使用委托调用
        Dim safeinvocationwithparam As DelSafeInvocationWithParam = New DelSafeInvocationWithParam(AddressOf SysVarsChangedInternal)
        Invoke(safeinvocationwithparam, "Key_Status", value)
    End Sub

    Private Sub SysVarsChangedInternal(sysvarName As String, value As Object)

        If (((Not mCANoeMeasurement Is Nothing) AndAlso mCANoeMeasurement.Running)) Then

            ' 更新控件显示数值
            If (sysvarName = "Gear") Then
                trackbarGear.Value = mCANoeSysGear.Value
            End If
            If (sysvarName = "Key_Status") Then
                trackbarKeyState.Value = mCANoeSysKeyState.Value
            End If

        End If
    End Sub
#End Region
#Region "VB.net事件处理"
    Private Sub btnOpen_Click(sender As Object, e As EventArgs) Handles btnOpen.Click

        ' 初始化CANoe应用程序
        mCANoeApp = New CANoe.Application

        ' 初始化measurement对象
        mCANoeMeasurement = mCANoeApp.Measurement

        ' 停止measurement如果已经运行中
        If mCANoeMeasurement.Running Then
            mCANoeMeasurement.Stop()
        End If

        ' 如果CANoe应用程序没打开，打开应用并确保configuration被成功加载
        If (Not mCANoeApp Is Nothing) Then

            ' 打开演示的configuration
            mCANoeApp.Open(mAbsoluteConfigPath, True, True)

            ' 确保configuration被成功加载
            Dim ocresult As CANoe.OpenConfigurationResult = mCANoeApp.Configuration.OpenConfigurationResult
            If (ocresult.result = 0) Then
                ConfigurationOpened()
            End If
        End If
    End Sub

    Private Sub btnMeasurement_Click(sender As Object, e As EventArgs) Handles btnMeasurement.Click
        If mCANoeMeasurement.Running Then
            ' 停止运行中的测量
            mCANoeMeasurement.Stop()
        Else
            ' 开始测量
            mCANoeMeasurement.Start()
        End If

    End Sub

    Private Sub btnExit_Click(sender As Object, e As EventArgs) Handles btnExit.Click
        UnregisterCANoeEventHandlers()

        ' 如果测量在运行中，先停止测量
        If (mCANoeInstanceRunning AndAlso (Not mCANoeMeasurement Is Nothing) AndAlso mCANoeMeasurement.Running) Then
            mCANoeMeasurement.Stop()
        End If
        mCANoeApp.Quit()
        btnMeasurement.Enabled = False
        btnMeasurement.Text = "开始测量"
        lblStatus.Text = "CANoe应用程序已退出"
        btnOpen.Enabled = True
        btnExit.Enabled = False
        Timer1.Enabled = False
    End Sub


    Private Sub trackbarKeyState_Scroll(sender As Object, e As EventArgs) Handles trackbarKeyState.Scroll
        mCANoeSysKeyState.Value = trackbarKeyState.Value
        localchangeflag = 1
    End Sub

    Private Sub trackbarGear_Scroll(sender As Object, e As EventArgs) Handles trackbarGear.Scroll
        mCANoeSysGear.Value = trackbarGear.Value
        localchangeflag = 2
    End Sub

    Private Sub trackbarVehicleSpeed_Scroll(sender As Object, e As EventArgs) Handles trackbarVehicleSpeed.Scroll
        mCANoeVehicleSpeed.Value = trackbarVehicleSpeed.Value
        localchangeflag = 3
    End Sub

    Private Sub trackbarEngSpeed_Scroll(sender As Object, e As EventArgs) Handles trackbarEngSpeed.Scroll
        mCANoeEngineSpeed.Value = trackbarEngSpeed.Value
        localchangeflag = 4
    End Sub

    '实时更新控件状态
    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick

        progbarEngSpeed.Value = mCANoeEngineSpeed.Value
        lblEngSpeed.Text = mCANoeEngineSpeed.Value
        progbarVehicleSpeed.Value = mCANoeVehicleSpeed.Value
        lblVehicleSpeed.Text = mCANoeVehicleSpeed.Value

        '更新trackbar,去除防抖
        If (localchangeflag <> 1) Then
            trackbarKeyState.Value = mCANoeSysKeyState.Value
        End If
        If (localchangeflag <> 2) Then
            trackbarGear.Value = mCANoeSysGear.Value
        End If
        If (localchangeflag <> 3) Then
            trackbarEngSpeed.Value = mCANoeEngineSpeed.Value
        End If
        If (localchangeflag <> 4) Then
            trackbarVehicleSpeed.Value = mCANoeVehicleSpeed.Value
        End If
        localchangeflag = 0

        '更新lock/unlock; driver1/driver2按钮状态
        If (mCANoeSysCarDriver.Value = 1) Then
            btnDriver1.ForeColor = Color.Red
            btnDriver2.ForeColor = Color.Black
        Else
            btnDriver1.ForeColor = Color.Black
            btnDriver2.ForeColor = Color.Red
        End If

        If (lockdelay < 3) Then
            lockdelay = lockdelay + 1
            If (lockdelay = 2) Then
                btnLock.ForeColor = Color.Black
                btnUnlock.ForeColor = Color.Black
                mCANoeSysUnlockCar.Value = 0
                mCANoeSysLockCar.Value = 0
            End If
        End If


    End Sub


    Private Sub btnUnlock_Click(sender As Object, e As EventArgs) Handles btnUnlock.Click
        btnUnlock.ForeColor = Color.Red
        mCANoeSysUnlockCar.Value = 1
        mCANoeSysLockCar.Value = 0
        lockdelay = 0
    End Sub

    Private Sub btnLock_Click(sender As Object, e As EventArgs) Handles btnLock.Click
        btnLock.ForeColor = Color.Red
        mCANoeSysUnlockCar.Value = 0
        mCANoeSysLockCar.Value = 1
        lockdelay = 0
    End Sub

    Private Sub btnDriver1_Click(sender As Object, e As EventArgs) Handles btnDriver1.Click
        mCANoeSysCarDriver.Value = 1
    End Sub

    Private Sub btnDriver2_Click(sender As Object, e As EventArgs) Handles btnDriver2.Click
        mCANoeSysCarDriver.Value = 2
    End Sub
    Private Sub btnCal_Click(sender As Object, e As EventArgs) Handles btnCal.Click
        ' 调用 "Multiply" CAPL函数并传递参数，返回结果
        If (Not mCANoeMultiply Is Nothing) Then

            Dim op1 As Integer
            Dim op2 As Integer
            Dim result As Integer

            Integer.TryParse(txtOp1.Text, op1)
            Integer.TryParse(txtOp2.Text, op2)

            ' 返回乘法结果
            result = mCANoeMultiply.Call(op1, op2)
            txtResult.Text = result.ToString
        End If
    End Sub

    Private Sub COMAuto_VB_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        UnregisterCANoeEventHandlers()

        If btnExit.Enabled = True Then
            ' 如果测量在运行中，先停止测量
            If (mCANoeInstanceRunning AndAlso (Not mCANoeMeasurement Is Nothing) AndAlso mCANoeMeasurement.Running) Then
                mCANoeMeasurement.Stop()
            End If
        End If

    End Sub
#End Region


End Class
