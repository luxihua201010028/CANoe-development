
// FDX_DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FDX_Demo.h"
#include "FDX_DemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "CANoeFDX.h"
#include "FDXDatagram.h"
#include "FDXSocket.h"
#include "FDXDispatcher.h"
#include <iostream>
using namespace std;

//数据交换状态: false - stopped; true - running
bool exchange_flag; 

//数据交换模式：0 -data read ; 1 -data write; 2 - frame access read; 3 - frame access write
int exchange_mode;	

//Measurement的运行状态，初始为-1无效值 其他定义如下
//kMeasurementState_NotRunning = 1;
//kMeasurementState_PreStart = 2;
//kMeasurementState_Running = 3;
// kMeasurementState_Stop = 4;
int measure_status = -1;	


//FDX数据接收的状态
//0- no error; 1 - format error; 2- sequence number error ; 3- measurement is stopped
int dispacth_status = 0;  

int engSpeed = 0; //engine speed变量
int vehSpeed = 0; //vehicle speed变量
int hazards_status = 0; //hazards状态

// 定义一个MyDispatcher类，用于接收和处理FDX协议的数据报，及错误处理
class MyDispatcher : public FDXDispatcher
{
public: 
	int status; //0- no error; 1 - format error; 2- sequence number error ; 3- measurement is stopped
	virtual void OnFormatError();
	virtual void OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr);
	virtual void OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command);
	virtual void OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command);
	virtual void OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command);
};

// 定义数据写结构体，需要严格按照FDX Description中的定义
struct FDXDataRead
{
	static const CANoeFDX::uint16 cGroupID = 1;
	static const CANoeFDX::uint16 cSize = 8;

	CANoeFDX::uint16  SigEngSpeed;					// offset 0
	CANoeFDX::uint16  SigVehicleSpeed;				// offset 2
	CANoeFDX::uint8  SigKeyState;					// offset 4
	CANoeFDX::uint8  VarHazards;					// offset 5
	CANoeFDX::uint8  VarGear;						// offset 6
	CANoeFDX::uint8  VarUnlockCar;					// offset 7
};

// 检查结构体FDXDataRead的成员在内存布局中是否规范
static_assert(FDXDataRead::cSize == sizeof(FDXDataRead), "Invalid size of struct FDXDataRead");
static_assert(0 == offsetof(FDXDataRead, SigEngSpeed), "Invalid offset of FDXDataRead::SigEngSpeed");
static_assert(2 == offsetof(FDXDataRead, SigVehicleSpeed), "Invalid offset of FDXDataRead::SigVehicleSpeed");
static_assert(4 == offsetof(FDXDataRead, SigKeyState), "Invalid offset of FDXDataRead::SigKeyState");
static_assert(5 == offsetof(FDXDataRead, VarHazards), "Invalid offset of FDXDataRead::VarHazards");
static_assert(6 == offsetof(FDXDataRead, VarGear), "Invalid offset of FDXDataRead::VarGear");
static_assert(7 == offsetof(FDXDataRead, VarUnlockCar), "Invalid offset of FDXDataRead::VarOpenCar");

// 定义数据写结构体，需要严格按照FDX Description中的定义
struct FDXDataWrite
{
	static const CANoeFDX::uint16 cGroupID = 2;
	static const CANoeFDX::uint16 cSize = 8;

	CANoeFDX::uint16  SigVehicleSpeed;        // offset 0
	CANoeFDX::uint16  SigEngSpeed;            // offset 2	
	CANoeFDX::uint8  SigKeyState;			 // offset 4
	CANoeFDX::uint8  VarHazards;			 // offset 5
	CANoeFDX::uint8  VarGear;				 // offset 6
	CANoeFDX::uint8  VarUnlockCar;              // offset 7
};

// 检查结构体FDXDataWrite的成员在内存布局中是否规范
static_assert(FDXDataWrite::cSize == sizeof(FDXDataWrite), "Invalid size of struct FDXDataWrite");
static_assert(0 == offsetof(FDXDataWrite, SigVehicleSpeed), "Invalid offset of FDXDataWrite::SigVehicleSpeed");
static_assert(2 == offsetof(FDXDataWrite, SigEngSpeed), "Invalid offset of FDXDataWrite::SigEngSpeed");
static_assert(4 == offsetof(FDXDataWrite, SigKeyState), "Invalid offset of FDXDataWrite::SigKeyState");
static_assert(5 == offsetof(FDXDataWrite, VarHazards), "Invalid offset of FDXDataWrite::VarHazards");
static_assert(6 == offsetof(FDXDataWrite, VarGear), "Invalid offset of FDXDataWrite::VarGear");
static_assert(7 == offsetof(FDXDataWrite, VarUnlockCar), "Invalid offset of FDXDataWrite::VarOpenCar");

// 定义报文读写结构体，需要严格按照FDX Description中的定义
struct FDXFrameAccess
{
	static const CANoeFDX::uint16 cGroupID = 3;
	static const CANoeFDX::uint16 cSize = 24;

	CANoeFDX::uint32 EngineData_byteArraySize;
	struct FrameEngineData
	{
		static const CANoeFDX::uint16 cSize = 8;

		CANoeFDX::uint16  EngTemp : 7;
		CANoeFDX::uint16 _no_data_1 : 1;
		CANoeFDX::uint16 PetrolLevel : 8;
		CANoeFDX::uint16 EngSpeed : 16;
		CANoeFDX::uint16 _no_data_2 : 16;
		CANoeFDX::uint16 _no_data_3 : 16;
	} enginedata;

	CANoeFDX::uint32 VehihcleData_byteArraySize;
	struct FrameVehihcleData
	{
		static const CANoeFDX::uint16 cSize = 6;

		CANoeFDX::uint16  VehicleSpeed : 10;
		CANoeFDX::uint16  _no_data_1 : 6;
		CANoeFDX::uint16  Diagnostics : 8;
		CANoeFDX::uint16 _no_data_2 : 8;
		CANoeFDX::uint16  _no_data_3 : 16;
	} vehihcledata;
};

// 检查结构体FDXFrameAccess的成员在内存布局中是否规范
static_assert(FDXFrameAccess::cSize == sizeof(FDXFrameAccess), "Invalid size of struct FDXFrameAccess");
static_assert(FDXFrameAccess::FrameEngineData::cSize == sizeof(FDXFrameAccess::FrameEngineData), "Invalid size of struct EngineStateFrame");
static_assert(FDXFrameAccess::FrameVehihcleData::cSize == sizeof(FDXFrameAccess::FrameVehihcleData), "Invalid size of struct VehihcleDataFrame");
static_assert(0 == offsetof(FDXFrameAccess, EngineData_byteArraySize), "Invalid offset of FDXFrameAccess::EngineState_byteArraySize");
static_assert(4 == offsetof(FDXFrameAccess, enginedata), "Invalid offset of FDXFrameAccess::engineState");
static_assert(12 == offsetof(FDXFrameAccess, VehihcleData_byteArraySize), "Invalid offset of FDXFrameAccess::LightState_byteArraySize");
static_assert(16 == offsetof(FDXFrameAccess, vehihcledata), "Invalid offset of FDXFrameAccess::lightState");




CANoeFDX::uint8 gCANoeMeasurementState = CANoeFDX::kMeasurementState_NotRunning;

FDXSocket      gFDXSocket;
FDXDatagram    gFDXDatagram;
MyDispatcher   gFDXDispatcher;


void MyDispatcher::OnFormatError()
{
	status = 1; // Format Error
}

void MyDispatcher::OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr)
{
	status = 2; // Sequence Number Error
}

void MyDispatcher::OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command)
{
	if (command->measurementState == CANoeFDX::kMeasurementState_NotRunning)
	{
		status = 3; //CANoe measurement is not running
	}
	else
	{
		status = 0;
	}
	gCANoeMeasurementState = command->measurementState;
}

void MyDispatcher::OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command)
{   

	if (command->dataErrorCode != CANoeFDX::kDataErrorCode_MeasurementNotRunning)
	{
		status = 4; //Data error
	}
}

//该函数将处理来自CANoe中的数据报，并提取出信号、变量
void MyDispatcher::OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command)
{
	//数据读取模式
	if (command->groupID == FDXDataRead::cGroupID)
	{
		FDXDataRead * readData = reinterpret_cast<FDXDataRead*>(command->dataBytes);

		engSpeed= readData->SigEngSpeed;
		vehSpeed= readData->SigVehicleSpeed;
		hazards_status = readData->VarHazards;
	
	}
	//报文读取模式
	else if (command->groupID == FDXFrameAccess::cGroupID)
	{
		FDXFrameAccess* readData = reinterpret_cast<FDXFrameAccess*>(command->dataBytes);
		engSpeed = readData->enginedata.EngSpeed;
		vehSpeed = readData->vehihcledata.VehicleSpeed;
	}
	
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFDX_DemoDlg dialog



CFDX_DemoDlg::CFDX_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFDX_DemoDlg::IDD, pParent)
	, m_status(_T(""))
	, m_vehspeed(0)
	, m_engspeed(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFDX_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_ENG_SPEED, m_Eng_Speed);
	DDX_Control(pDX, IDC_SLIDER_VEH_SPEED, m_Veh_Speed);
	DDX_Control(pDX, IDC_EXCHANGE, m_btnExchange);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_status);
	DDX_Control(pDX, IDSTART, m_btnStart);
	DDX_Control(pDX, IDC_CHECK_HAZARDS, m_chkHazards);
	DDX_Text(pDX, IDC_EDIT_VEH_SPEED, m_vehspeed);
	DDX_Text(pDX, IDC_EDIT_ENG_SPEED, m_engspeed);
}

BEGIN_MESSAGE_MAP(CFDX_DemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CFDX_DemoDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ENG_SPEED, &CFDX_DemoDlg::OnCustomdrawSliderEngSpeed)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_VEH_SPEED, &CFDX_DemoDlg::OnCustomdrawSliderVehSpeed)
	ON_BN_CLICKED(IDC_EXCHANGE, &CFDX_DemoDlg::OnBnClickedExchange)
	ON_COMMAND(IDC_RADIO_ACCESSWRITE, &CFDX_DemoDlg::OnRadioAccesswrite)
	ON_COMMAND(IDC_RADIO_ACCESSREAD, &CFDX_DemoDlg::OnRadioAccessread)
	ON_COMMAND(IDC_RADIO_DATAREAD, &CFDX_DemoDlg::OnRadioDataread)
	ON_COMMAND(IDC_RADIO_DATAWRITE, &CFDX_DemoDlg::OnRadioDatawrite)
	ON_BN_CLICKED(IDSTART, &CFDX_DemoDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_CHECK_HAZARDS, &CFDX_DemoDlg::OnBnClickedCheckHazards)
	ON_BN_CLICKED(IDC_START, &CFDX_DemoDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// 初始化FDX_Demo对话框

BOOL CFDX_DemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CButton *Btn = (CButton *)GetDlgItem(IDC_RADIO_DATAREAD);
	Btn->SetCheck(true); //数据读取为初始模式
	m_btnStart.EnableWindow(FALSE);
	m_Eng_Speed.EnableWindow(FALSE); //数据读取模式时，滑动条只显示不可操作
	m_Veh_Speed.EnableWindow(FALSE); //数据读取模式时，滑动条只显示不可操作

	//初始化Engine Speed滑动条的最小值、最大值和初始位置
	m_Eng_Speed.SetRangeMin(0);
	m_Eng_Speed.SetRangeMax(8000);
	m_Eng_Speed.SetPos(0);
	m_engspeed = 0;

	//初始化Vehicle Speed滑动条的最小值、最大值和初始位置
	m_Veh_Speed.SetRangeMin(0);
	m_Veh_Speed.SetRangeMax(600);
	m_Veh_Speed.SetPos(0);
	m_vehspeed = 0;

	exchange_mode = 0; //数据读取为初始模式
	exchange_flag = false; //数据交换未开启

	//在状态栏输出时间和状态信息
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");	
	m_status = str + _T("Data Exchange is stopped!\r\n");
	m_status = str + _T("Data Read Mode is selected!\r\n") + m_status;
	UpdateData(false);
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFDX_DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFDX_DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFDX_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 在定时器事件中处理数据处理和显示
void CFDX_DemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int curr_measure_status; //临时变量用于表示测量运行状态
	int curr_dispatch_status = 1; //临时变量用于表示数据接收状态

	curr_measure_status = gCANoeMeasurementState; //获取测量状态

	
	//数据读操作模式
	if (exchange_mode == 0)
	{
		// build datagram using individual signal and variable access
		
			gFDXDatagram.InitWithHeader();
			gFDXDatagram.AddDataRequest(FDXDataRead::cGroupID);

			if (hazards_status)
				m_chkHazards.SetCheck(1);
			else
				m_chkHazards.SetCheck(0);

			m_Eng_Speed.SetPos(engSpeed);
			m_Veh_Speed.SetPos(vehSpeed);
			m_engspeed = m_Eng_Speed.GetPos();
			m_vehspeed = m_Veh_Speed.GetPos()/2;
	}
	//数据写操作模式
	if (exchange_mode==1)
	{
		// build datagram using individual signal and variable access
		
		gFDXDatagram.InitWithHeader();
		gFDXDatagram.AddDataRequest(FDXDataRead::cGroupID);
		void* dataBytes;
		dataBytes = gFDXDatagram.AddDataExchange(FDXDataWrite::cGroupID, FDXDataWrite::cSize);
		FDXDataWrite* writeData = reinterpret_cast<FDXDataWrite*>(dataBytes);
			

		writeData->VarGear = 3; 
		writeData->SigKeyState = 2;
		writeData->VarUnlockCar = 1;

		writeData->VarHazards = hazards_status;
		writeData->SigVehicleSpeed = m_Veh_Speed.GetPos();
		writeData->SigEngSpeed = m_Eng_Speed.GetPos(); // currentEngSpeed; 

		m_engspeed = m_Eng_Speed.GetPos();
		m_vehspeed = m_Veh_Speed.GetPos()/2;
	}

	//报文读操作模式
	if (exchange_mode==2)
	{
		// build datagram using full frame access (byte array)
		gFDXDatagram.InitWithHeader();
		gFDXDatagram.AddDataRequest(FDXDataRead::cGroupID);
		gFDXDatagram.AddDataRequest(FDXFrameAccess::cGroupID);

		m_Eng_Speed.SetPos(engSpeed);
		m_Veh_Speed.SetPos(vehSpeed);

		m_engspeed = m_Eng_Speed.GetPos();
		m_vehspeed = m_Veh_Speed.GetPos()/2;
	}

	//报文写操作模式
	if (exchange_mode == 3)
	{
		// build datagram using full frame access (byte array)
		gFDXDatagram.InitWithHeader();
		gFDXDatagram.AddDataRequest(FDXDataRead::cGroupID);
		gFDXDatagram.AddDataRequest(FDXFrameAccess::cGroupID);
		
		void* dataBytes;
		dataBytes = gFDXDatagram.AddDataExchange(FDXFrameAccess::cGroupID, FDXFrameAccess::cSize);
		FDXFrameAccess* writeData = reinterpret_cast<FDXFrameAccess*>(dataBytes);


		writeData->EngineData_byteArraySize = FDXFrameAccess::FrameEngineData::cSize;
		writeData->enginedata.EngSpeed = m_Eng_Speed.GetPos();
		writeData->enginedata.EngTemp = 16;
		writeData->enginedata.PetrolLevel = 255;

		writeData->VehihcleData_byteArraySize = FDXFrameAccess::FrameVehihcleData::cSize;
		writeData->vehihcledata.VehicleSpeed =m_Veh_Speed.GetPos();
		writeData->vehihcledata.Diagnostics = 0;

		m_engspeed = m_Eng_Speed.GetPos();
		m_vehspeed = m_Veh_Speed.GetPos()/2;
	}

	UpdateData(FALSE);
	//发送数据报到CANoe
	gFDXSocket.Send(gFDXDatagram);

	// 接收和处理来自CANoe的数据报
	int rc = gFDXSocket.Receive(gFDXDatagram);
	if (rc == FDXSocket::OK)
	{
		gFDXDispatcher.DispatchDatagram(gFDXDatagram);
		curr_dispatch_status = gFDXDispatcher.status; //数据报状态
	}

	// 判断measurement状态是否有变化
	// kMeasurementState_NotRunning = 1;
	// kMeasurementState_PreStart = 2;
	// kMeasurementState_Running = 3;
	// kMeasurementState_Stop = 4;
	if (measure_status != curr_measure_status)
	{
		CTime tm;
		CString str;
		tm = CTime::GetCurrentTime();
		str = tm.Format("%X: ");
		switch (curr_measure_status)
		{
		case 1:
			m_status = str + _T("Measurement is not running!\r\n") + m_status ;
			m_btnStart.SetWindowTextW(_T("Start Measurement"));
			break;
		case 2:
			m_status = str + _T("Measurement is at PreStart!\r\n") + m_status;
			break;
		case 3:
			m_status = str + _T("Measurement is running!\r\n") + m_status;
			m_btnStart.SetWindowTextW(_T("Stop Measurement"));
			break;
		case 4:
			m_status = str + _T("Measurement is stopped!\r\n") + m_status;
			m_btnStart.SetWindowTextW(_T("Start Measurement"));
			break;
		}
		measure_status = curr_measure_status;
		UpdateData(false);
	}

	if (dispacth_status != curr_dispatch_status)
	{
		CTime tm;
		CString str;
		tm = CTime::GetCurrentTime();
		str = tm.Format("%X: ");
		switch (curr_dispatch_status)
		{
		case 1:
			m_status = str + _T("Format Error!\r\n") + m_status;
			break;
		case 2:
			m_status = str + _T("Sequence Number Error!\r\n") + m_status;
			break;
		case 3:
			//m_status = str + _T("Measurement is stopped!\r\n") + m_status;
			//m_btnStart.SetWindowTextW(_T("Start Measurement"));
			break;
		case 4:
			m_status = str + _T("Data error!\r\n") + m_status;
			break;
		default:
			//m_status = str + _T("Measurement is running!\r\n") + m_status;
			//m_btnStart.SetWindowTextW(_T("Stop Measurement"));
			break;
		}
		dispacth_status = curr_dispatch_status;
		UpdateData(false);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CFDX_DemoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	gFDXDatagram.AddStop();
	gFDXSocket.Send(gFDXDatagram);
	gFDXSocket.Close();
	CDialogEx::OnCancel();
}


void CFDX_DemoDlg::OnCustomdrawSliderEngSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CFDX_DemoDlg::OnCustomdrawSliderVehSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CFDX_DemoDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	if (measure_status != 3)
	{
		m_btnStart.SetWindowTextW(_T("Start Measurement"));
		gFDXDatagram.AddStart();
		gFDXSocket.Send(gFDXDatagram);
		if (exchange_mode % 2 != 0)
		{
			m_Eng_Speed.EnableWindow(TRUE);
			m_Veh_Speed.EnableWindow(TRUE);
		}
	}
	else
	{
		m_btnStart.SetWindowTextW(_T("Stop Measurement"));
		gFDXDatagram.AddStop();
		gFDXSocket.Send(gFDXDatagram);
		m_Eng_Speed.EnableWindow(FALSE);
		m_Veh_Speed.EnableWindow(FALSE);
	}
}

//打开或关闭数据交换
void CFDX_DemoDlg::OnBnClickedExchange()
{
	// TODO: Add your control notification handler code here
	const char* addr = "127.0.0.1";//本机回送地址（Loopback Address）
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");

	if (!exchange_flag)
	{
		gFDXSocket.SetCANoeAddr(addr, 2809);//设置Socket的IP和端口
		gFDXSocket.Open(); //打开Socket
		SetTimer(1, 200, NULL); //设置并开启计时器1
		m_status = str + _T("Data Exchange Running!\r\n") + m_status;
		m_btnExchange.SetWindowTextW(_T("Stop Exchange"));
		m_btnStart.EnableWindow(TRUE);
	}
	else
	{
		gFDXSocket.Close(); //关闭Socket
		KillTimer(1); //关闭定时器1
		m_status = str + _T("Data Exchange is stopped!\r\n") + m_status;
		m_btnExchange.SetWindowTextW(_T("Start Exchange"));
		m_btnStart.EnableWindow(FALSE);
	}
	exchange_flag = !exchange_flag;
	UpdateData(false);
}

//开启数据读模式
void CFDX_DemoDlg::OnRadioDataread()
{
	// TODO: Add your command handler code here
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");
	m_status = str + _T("Data Read Mode is selected!\r\n") + m_status;
	exchange_mode = 0;
	m_Eng_Speed.EnableWindow(FALSE);
	m_Veh_Speed.EnableWindow(FALSE);
	m_chkHazards.ShowWindow(TRUE);
	UpdateData(false);
}

//开启数据写模式
void CFDX_DemoDlg::OnRadioDatawrite()
{
	// TODO: Add your command handler code here
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");
	m_status = str + _T("Data Write Mode is selected!\r\n") + m_status;
	exchange_mode = 1;
	if (measure_status == 3)
	{
		m_Eng_Speed.EnableWindow(TRUE);
		m_Veh_Speed.EnableWindow(TRUE);
	}
	m_chkHazards.ShowWindow(TRUE);
	UpdateData(false);
}

//开启报文读模式
void CFDX_DemoDlg::OnRadioAccessread()
{
	// TODO: Add your command handler code here
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");
	m_status = str + _T("Frame Access Read Mode is selected!\r\n") + m_status;

	exchange_mode = 2;
	m_Eng_Speed.EnableWindow(FALSE);
	m_Veh_Speed.EnableWindow(FALSE);
	m_chkHazards.ShowWindow(FALSE);
	UpdateData(false);
}
//开启报文写模式
void CFDX_DemoDlg::OnRadioAccesswrite()
{
	// TODO: Add your command handler code here
	CTime tm;
	CString str;
	tm = CTime::GetCurrentTime();
	str = tm.Format("%X: ");
	m_status = str + _T("Frame Access Write Mode is selected!\r\n") + m_status;

	exchange_mode = 3;
	if (measure_status == 3)
	{
		m_Eng_Speed.EnableWindow(TRUE);
		m_Veh_Speed.EnableWindow(TRUE);
	}
	m_chkHazards.ShowWindow(FALSE);
	UpdateData(false);
}

//修改数据写分组中Hazards系统变量
void CFDX_DemoDlg::OnBnClickedCheckHazards()
{
	// TODO: Add your control notification handler code here
	if ((exchange_mode == 1) && (measure_status == 3))
	{
		UpdateData(FALSE);
		if (m_chkHazards.GetCheck())
			hazards_status = 1;
		else
			hazards_status = 0;
	}
	else
	{
		UpdateData(TRUE);
	}
}