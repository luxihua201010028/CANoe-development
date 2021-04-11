<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class COMAuto_VB
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.txtPath = New System.Windows.Forms.TextBox()
        Me.groupboxControl = New System.Windows.Forms.GroupBox()
        Me.lblStatus = New System.Windows.Forms.Label()
        Me.btnExit = New System.Windows.Forms.Button()
        Me.btnMeasurement = New System.Windows.Forms.Button()
        Me.btnOpen = New System.Windows.Forms.Button()
        Me.groupboxSysVar = New System.Windows.Forms.GroupBox()
        Me.btnDriver2 = New System.Windows.Forms.Button()
        Me.btnDriver1 = New System.Windows.Forms.Button()
        Me.btnUnlock = New System.Windows.Forms.Button()
        Me.btnLock = New System.Windows.Forms.Button()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.trackbarKeyState = New System.Windows.Forms.TrackBar()
        Me.trackbarGear = New System.Windows.Forms.TrackBar()
        Me.groupboxSignal = New System.Windows.Forms.GroupBox()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.lblEngSpeed = New System.Windows.Forms.Label()
        Me.lblVehicleSpeed = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.progbarEngSpeed = New System.Windows.Forms.ProgressBar()
        Me.progbarVehicleSpeed = New System.Windows.Forms.ProgressBar()
        Me.trackbarEngSpeed = New System.Windows.Forms.TrackBar()
        Me.trackbarVehicleSpeed = New System.Windows.Forms.TrackBar()
        Me.groupboxCAPL = New System.Windows.Forms.GroupBox()
        Me.Label19 = New System.Windows.Forms.Label()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.btnCal = New System.Windows.Forms.Button()
        Me.txtResult = New System.Windows.Forms.TextBox()
        Me.txtOp2 = New System.Windows.Forms.TextBox()
        Me.txtOp1 = New System.Windows.Forms.TextBox()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.groupboxControl.SuspendLayout()
        Me.groupboxSysVar.SuspendLayout()
        CType(Me.trackbarKeyState, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarGear, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupboxSignal.SuspendLayout()
        CType(Me.trackbarEngSpeed, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarVehicleSpeed, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupboxCAPL.SuspendLayout()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(6, 23)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(99, 13)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "CANoe *.cfg 路径："
        '
        'txtPath
        '
        Me.txtPath.Location = New System.Drawing.Point(6, 39)
        Me.txtPath.Name = "txtPath"
        Me.txtPath.Size = New System.Drawing.Size(527, 20)
        Me.txtPath.TabIndex = 1
        '
        'groupboxControl
        '
        Me.groupboxControl.Controls.Add(Me.lblStatus)
        Me.groupboxControl.Controls.Add(Me.btnExit)
        Me.groupboxControl.Controls.Add(Me.btnMeasurement)
        Me.groupboxControl.Controls.Add(Me.btnOpen)
        Me.groupboxControl.Controls.Add(Me.txtPath)
        Me.groupboxControl.Controls.Add(Me.Label1)
        Me.groupboxControl.Location = New System.Drawing.Point(2, 7)
        Me.groupboxControl.Name = "groupboxControl"
        Me.groupboxControl.Size = New System.Drawing.Size(547, 116)
        Me.groupboxControl.TabIndex = 2
        Me.groupboxControl.TabStop = False
        Me.groupboxControl.Text = "应用控制"
        '
        'lblStatus
        '
        Me.lblStatus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblStatus.Location = New System.Drawing.Point(11, 92)
        Me.lblStatus.Name = "lblStatus"
        Me.lblStatus.Size = New System.Drawing.Size(514, 21)
        Me.lblStatus.TabIndex = 4
        Me.lblStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnExit
        '
        Me.btnExit.Location = New System.Drawing.Point(369, 65)
        Me.btnExit.Name = "btnExit"
        Me.btnExit.Size = New System.Drawing.Size(156, 24)
        Me.btnExit.TabIndex = 3
        Me.btnExit.Text = "退出CANoe"
        Me.btnExit.UseVisualStyleBackColor = True
        '
        'btnMeasurement
        '
        Me.btnMeasurement.Location = New System.Drawing.Point(190, 65)
        Me.btnMeasurement.Name = "btnMeasurement"
        Me.btnMeasurement.Size = New System.Drawing.Size(156, 24)
        Me.btnMeasurement.TabIndex = 3
        Me.btnMeasurement.Text = "开始测量"
        Me.btnMeasurement.UseVisualStyleBackColor = True
        '
        'btnOpen
        '
        Me.btnOpen.Location = New System.Drawing.Point(11, 65)
        Me.btnOpen.Name = "btnOpen"
        Me.btnOpen.Size = New System.Drawing.Size(156, 24)
        Me.btnOpen.TabIndex = 2
        Me.btnOpen.Text = "打开CANoe Configuration"
        Me.btnOpen.UseVisualStyleBackColor = True
        '
        'groupboxSysVar
        '
        Me.groupboxSysVar.Controls.Add(Me.btnDriver2)
        Me.groupboxSysVar.Controls.Add(Me.btnDriver1)
        Me.groupboxSysVar.Controls.Add(Me.btnUnlock)
        Me.groupboxSysVar.Controls.Add(Me.btnLock)
        Me.groupboxSysVar.Controls.Add(Me.Label8)
        Me.groupboxSysVar.Controls.Add(Me.Label3)
        Me.groupboxSysVar.Controls.Add(Me.Label5)
        Me.groupboxSysVar.Controls.Add(Me.Label10)
        Me.groupboxSysVar.Controls.Add(Me.Label4)
        Me.groupboxSysVar.Controls.Add(Me.Label6)
        Me.groupboxSysVar.Controls.Add(Me.Label9)
        Me.groupboxSysVar.Controls.Add(Me.Label7)
        Me.groupboxSysVar.Controls.Add(Me.Label11)
        Me.groupboxSysVar.Controls.Add(Me.Label12)
        Me.groupboxSysVar.Controls.Add(Me.trackbarKeyState)
        Me.groupboxSysVar.Controls.Add(Me.trackbarGear)
        Me.groupboxSysVar.Location = New System.Drawing.Point(2, 132)
        Me.groupboxSysVar.Name = "groupboxSysVar"
        Me.groupboxSysVar.Size = New System.Drawing.Size(547, 139)
        Me.groupboxSysVar.TabIndex = 3
        Me.groupboxSysVar.TabStop = False
        Me.groupboxSysVar.Text = "系统变量"
        '
        'btnDriver2
        '
        Me.btnDriver2.Location = New System.Drawing.Point(107, 83)
        Me.btnDriver2.Name = "btnDriver2"
        Me.btnDriver2.Size = New System.Drawing.Size(60, 43)
        Me.btnDriver2.TabIndex = 4
        Me.btnDriver2.Text = "Driver2"
        Me.btnDriver2.UseVisualStyleBackColor = True
        '
        'btnDriver1
        '
        Me.btnDriver1.Location = New System.Drawing.Point(28, 83)
        Me.btnDriver1.Name = "btnDriver1"
        Me.btnDriver1.Size = New System.Drawing.Size(60, 43)
        Me.btnDriver1.TabIndex = 4
        Me.btnDriver1.Text = "Driver1"
        Me.btnDriver1.UseVisualStyleBackColor = True
        '
        'btnUnlock
        '
        Me.btnUnlock.Location = New System.Drawing.Point(107, 24)
        Me.btnUnlock.Name = "btnUnlock"
        Me.btnUnlock.Size = New System.Drawing.Size(60, 43)
        Me.btnUnlock.TabIndex = 4
        Me.btnUnlock.Text = "Unlock"
        Me.btnUnlock.UseVisualStyleBackColor = True
        '
        'btnLock
        '
        Me.btnLock.Location = New System.Drawing.Point(28, 24)
        Me.btnLock.Name = "btnLock"
        Me.btnLock.Size = New System.Drawing.Size(60, 43)
        Me.btnLock.TabIndex = 4
        Me.btnLock.Text = "Lock"
        Me.btnLock.UseVisualStyleBackColor = True
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label8.Location = New System.Drawing.Point(405, 14)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(45, 18)
        Me.Label8.TabIndex = 3
        Me.Label8.Text = "Gear:"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(242, 14)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(58, 18)
        Me.Label3.TabIndex = 3
        Me.Label3.Text = "Ignition:"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label5.Location = New System.Drawing.Point(345, 51)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(34, 15)
        Me.Label5.TabIndex = 2
        Me.Label5.Text = "RUN"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label10.Location = New System.Drawing.Point(490, 51)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(18, 16)
        Me.Label10.TabIndex = 2
        Me.Label10.Text = "N"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.Location = New System.Drawing.Point(345, 19)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(48, 15)
        Me.Label4.TabIndex = 2
        Me.Label4.Text = "CRANK"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label6.Location = New System.Drawing.Point(345, 83)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(29, 15)
        Me.Label6.TabIndex = 2
        Me.Label6.Text = "K15"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label9.Location = New System.Drawing.Point(490, 19)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(18, 16)
        Me.Label9.TabIndex = 2
        Me.Label9.Text = "D"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label7.Location = New System.Drawing.Point(345, 115)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(30, 15)
        Me.Label7.TabIndex = 2
        Me.Label7.Text = "OFF"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label11.Location = New System.Drawing.Point(490, 83)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(18, 16)
        Me.Label11.TabIndex = 2
        Me.Label11.Text = "R"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label12.Location = New System.Drawing.Point(491, 115)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(17, 16)
        Me.Label12.TabIndex = 2
        Me.Label12.Text = "P"
        '
        'trackbarKeyState
        '
        Me.trackbarKeyState.LargeChange = 1
        Me.trackbarKeyState.Location = New System.Drawing.Point(310, 14)
        Me.trackbarKeyState.Maximum = 3
        Me.trackbarKeyState.Name = "trackbarKeyState"
        Me.trackbarKeyState.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trackbarKeyState.Size = New System.Drawing.Size(45, 124)
        Me.trackbarKeyState.TabIndex = 1
        '
        'trackbarGear
        '
        Me.trackbarGear.LargeChange = 1
        Me.trackbarGear.Location = New System.Drawing.Point(456, 14)
        Me.trackbarGear.Maximum = 3
        Me.trackbarGear.Name = "trackbarGear"
        Me.trackbarGear.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trackbarGear.Size = New System.Drawing.Size(45, 124)
        Me.trackbarGear.TabIndex = 1
        '
        'groupboxSignal
        '
        Me.groupboxSignal.Controls.Add(Me.Label16)
        Me.groupboxSignal.Controls.Add(Me.Label15)
        Me.groupboxSignal.Controls.Add(Me.lblEngSpeed)
        Me.groupboxSignal.Controls.Add(Me.lblVehicleSpeed)
        Me.groupboxSignal.Controls.Add(Me.Label14)
        Me.groupboxSignal.Controls.Add(Me.Label13)
        Me.groupboxSignal.Controls.Add(Me.progbarEngSpeed)
        Me.groupboxSignal.Controls.Add(Me.progbarVehicleSpeed)
        Me.groupboxSignal.Controls.Add(Me.trackbarEngSpeed)
        Me.groupboxSignal.Controls.Add(Me.trackbarVehicleSpeed)
        Me.groupboxSignal.Location = New System.Drawing.Point(8, 277)
        Me.groupboxSignal.Name = "groupboxSignal"
        Me.groupboxSignal.Size = New System.Drawing.Size(541, 113)
        Me.groupboxSignal.TabIndex = 3
        Me.groupboxSignal.TabStop = False
        Me.groupboxSignal.Text = "CAN总线信号"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(500, 71)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(24, 13)
        Me.Label16.TabIndex = 13
        Me.Label16.Text = "rpm"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(500, 29)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(32, 13)
        Me.Label15.TabIndex = 13
        Me.Label15.Text = "km/h"
        '
        'lblEngSpeed
        '
        Me.lblEngSpeed.AutoSize = True
        Me.lblEngSpeed.Location = New System.Drawing.Point(465, 71)
        Me.lblEngSpeed.Name = "lblEngSpeed"
        Me.lblEngSpeed.Size = New System.Drawing.Size(13, 13)
        Me.lblEngSpeed.TabIndex = 12
        Me.lblEngSpeed.Text = "0"
        '
        'lblVehicleSpeed
        '
        Me.lblVehicleSpeed.AutoSize = True
        Me.lblVehicleSpeed.Location = New System.Drawing.Point(465, 28)
        Me.lblVehicleSpeed.Name = "lblVehicleSpeed"
        Me.lblVehicleSpeed.Size = New System.Drawing.Size(13, 13)
        Me.lblVehicleSpeed.TabIndex = 12
        Me.lblVehicleSpeed.Text = "0"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label14.Location = New System.Drawing.Point(19, 66)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(88, 15)
        Me.Label14.TabIndex = 11
        Me.Label14.Text = "Engine Speed:"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label13.Location = New System.Drawing.Point(19, 23)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(89, 15)
        Me.Label13.TabIndex = 10
        Me.Label13.Text = "Vehicle Speed:"
        '
        'progbarEngSpeed
        '
        Me.progbarEngSpeed.Location = New System.Drawing.Point(304, 62)
        Me.progbarEngSpeed.Maximum = 8000
        Me.progbarEngSpeed.Name = "progbarEngSpeed"
        Me.progbarEngSpeed.Size = New System.Drawing.Size(155, 22)
        Me.progbarEngSpeed.Step = 40
        Me.progbarEngSpeed.TabIndex = 8
        '
        'progbarVehicleSpeed
        '
        Me.progbarVehicleSpeed.Location = New System.Drawing.Point(304, 19)
        Me.progbarVehicleSpeed.Maximum = 300
        Me.progbarVehicleSpeed.Name = "progbarVehicleSpeed"
        Me.progbarVehicleSpeed.Size = New System.Drawing.Size(155, 22)
        Me.progbarVehicleSpeed.Step = 15
        Me.progbarVehicleSpeed.TabIndex = 8
        '
        'trackbarEngSpeed
        '
        Me.trackbarEngSpeed.LargeChange = 100
        Me.trackbarEngSpeed.Location = New System.Drawing.Point(133, 62)
        Me.trackbarEngSpeed.Maximum = 8000
        Me.trackbarEngSpeed.Name = "trackbarEngSpeed"
        Me.trackbarEngSpeed.Size = New System.Drawing.Size(165, 45)
        Me.trackbarEngSpeed.SmallChange = 40
        Me.trackbarEngSpeed.TabIndex = 9
        Me.trackbarEngSpeed.TickFrequency = 15
        Me.trackbarEngSpeed.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarVehicleSpeed
        '
        Me.trackbarVehicleSpeed.LargeChange = 15
        Me.trackbarVehicleSpeed.Location = New System.Drawing.Point(133, 19)
        Me.trackbarVehicleSpeed.Maximum = 300
        Me.trackbarVehicleSpeed.Name = "trackbarVehicleSpeed"
        Me.trackbarVehicleSpeed.Size = New System.Drawing.Size(165, 45)
        Me.trackbarVehicleSpeed.SmallChange = 5
        Me.trackbarVehicleSpeed.TabIndex = 9
        Me.trackbarVehicleSpeed.TickFrequency = 15
        Me.trackbarVehicleSpeed.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'groupboxCAPL
        '
        Me.groupboxCAPL.Controls.Add(Me.Label19)
        Me.groupboxCAPL.Controls.Add(Me.Label20)
        Me.groupboxCAPL.Controls.Add(Me.btnCal)
        Me.groupboxCAPL.Controls.Add(Me.txtResult)
        Me.groupboxCAPL.Controls.Add(Me.txtOp2)
        Me.groupboxCAPL.Controls.Add(Me.txtOp1)
        Me.groupboxCAPL.Location = New System.Drawing.Point(8, 396)
        Me.groupboxCAPL.Name = "groupboxCAPL"
        Me.groupboxCAPL.Size = New System.Drawing.Size(541, 81)
        Me.groupboxCAPL.TabIndex = 3
        Me.groupboxCAPL.TabStop = False
        Me.groupboxCAPL.Text = "CAPL函数调用"
        '
        'Label19
        '
        Me.Label19.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label19.Location = New System.Drawing.Point(130, 43)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(16, 18)
        Me.Label19.TabIndex = 4
        Me.Label19.Text = "*"
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label20.Location = New System.Drawing.Point(272, 41)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(18, 20)
        Me.Label20.TabIndex = 4
        Me.Label20.Text = "="
        '
        'btnCal
        '
        Me.btnCal.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnCal.Location = New System.Drawing.Point(430, 38)
        Me.btnCal.Name = "btnCal"
        Me.btnCal.Size = New System.Drawing.Size(89, 23)
        Me.btnCal.TabIndex = 3
        Me.btnCal.Text = "计算"
        Me.btnCal.UseVisualStyleBackColor = True
        '
        'txtResult
        '
        Me.txtResult.Location = New System.Drawing.Point(312, 40)
        Me.txtResult.Name = "txtResult"
        Me.txtResult.Size = New System.Drawing.Size(94, 20)
        Me.txtResult.TabIndex = 0
        '
        'txtOp2
        '
        Me.txtOp2.Location = New System.Drawing.Point(167, 40)
        Me.txtOp2.Name = "txtOp2"
        Me.txtOp2.Size = New System.Drawing.Size(94, 20)
        Me.txtOp2.TabIndex = 0
        '
        'txtOp1
        '
        Me.txtOp1.Location = New System.Drawing.Point(22, 40)
        Me.txtOp1.Name = "txtOp1"
        Me.txtOp1.Size = New System.Drawing.Size(94, 20)
        Me.txtOp1.TabIndex = 0
        '
        'Timer1
        '
        Me.Timer1.Interval = 200
        '
        'COMAuto_VB
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(553, 514)
        Me.Controls.Add(Me.groupboxCAPL)
        Me.Controls.Add(Me.groupboxSignal)
        Me.Controls.Add(Me.groupboxSysVar)
        Me.Controls.Add(Me.groupboxControl)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "COMAuto_VB"
        Me.Text = "CANoe Server Demo"
        Me.groupboxControl.ResumeLayout(False)
        Me.groupboxControl.PerformLayout()
        Me.groupboxSysVar.ResumeLayout(False)
        Me.groupboxSysVar.PerformLayout()
        CType(Me.trackbarKeyState, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarGear, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupboxSignal.ResumeLayout(False)
        Me.groupboxSignal.PerformLayout()
        CType(Me.trackbarEngSpeed, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarVehicleSpeed, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupboxCAPL.ResumeLayout(False)
        Me.groupboxCAPL.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtPath As System.Windows.Forms.TextBox
    Friend WithEvents groupboxControl As System.Windows.Forms.GroupBox
    Friend WithEvents btnMeasurement As System.Windows.Forms.Button
    Friend WithEvents btnOpen As System.Windows.Forms.Button
    Friend WithEvents groupboxSysVar As System.Windows.Forms.GroupBox
    Friend WithEvents groupboxSignal As System.Windows.Forms.GroupBox
    Friend WithEvents groupboxCAPL As System.Windows.Forms.GroupBox
    Friend WithEvents btnExit As System.Windows.Forms.Button
    Friend WithEvents trackbarGear As System.Windows.Forms.TrackBar
    Friend WithEvents trackbarKeyState As System.Windows.Forms.TrackBar
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Private WithEvents progbarEngSpeed As System.Windows.Forms.ProgressBar
    Private WithEvents progbarVehicleSpeed As System.Windows.Forms.ProgressBar
    Private WithEvents trackbarEngSpeed As System.Windows.Forms.TrackBar
    Private WithEvents trackbarVehicleSpeed As System.Windows.Forms.TrackBar
    Friend WithEvents txtResult As System.Windows.Forms.TextBox
    Friend WithEvents txtOp2 As System.Windows.Forms.TextBox
    Friend WithEvents txtOp1 As System.Windows.Forms.TextBox
    Friend WithEvents btnDriver2 As System.Windows.Forms.Button
    Friend WithEvents btnDriver1 As System.Windows.Forms.Button
    Friend WithEvents btnUnlock As System.Windows.Forms.Button
    Friend WithEvents btnLock As System.Windows.Forms.Button
    Friend WithEvents lblEngSpeed As System.Windows.Forms.Label
    Friend WithEvents lblVehicleSpeed As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents lblStatus As System.Windows.Forms.Label
    Friend WithEvents Label19 As System.Windows.Forms.Label
    Friend WithEvents Label20 As System.Windows.Forms.Label
    Private WithEvents btnCal As System.Windows.Forms.Button

End Class
