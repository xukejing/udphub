
// udphubDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "udphub.h"
#include "udphubDlg.h"
#include "afxdialogex.h"

#include <process.h>          // _beginthreadex
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CudphubDlg 对话框



CudphubDlg::CudphubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UDPHUB_DIALOG, pParent)
	, m_bRunning(0)
	, m_hThread(nullptr)
	, m_nLocalPort(20777)
	, m_nTargetPort1(20778)
	, m_nTargetPort2(20779)
	, m_nTargetPort3(20779)
	, m_nTargetPort4(20779)
	, m_nTargetPort5(20779)
	, m_dwTargetIp1(0)
	, m_dwTargetIp2(0)
	, m_dwTargetIp3(0)
	, m_dwTargetIp4(0)
	, m_dwTargetIp5(0)
	, m_rxCount(0)
	, m_fwdCount(0)
	, m_errCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CudphubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip1);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ip2);
	DDX_Control(pDX, IDC_IPADDRESS3, m_ip3);
	DDX_Control(pDX, IDC_IPADDRESS4, m_ip4);
	DDX_Control(pDX, IDC_IPADDRESS5, m_ip5);
	DDX_Control(pDX, IDC_PORT1_EDIT, m_port1_EDIT);
	DDX_Text(pDX, IDC_PORT1_EDIT, m_nTargetPort1);
	DDX_Control(pDX, IDC_PORT2_EDIT, m_port2_EDIT);
	DDX_Text(pDX, IDC_PORT2_EDIT, m_nTargetPort2);
	DDX_Control(pDX, IDC_PORT3_EDIT, m_port3_EDIT);
	DDX_Text(pDX, IDC_PORT3_EDIT, m_nTargetPort3);
	DDX_Control(pDX, IDC_PORT4_EDIT, m_port4_EDIT);
	DDX_Text(pDX, IDC_PORT4_EDIT, m_nTargetPort4);
	DDX_Control(pDX, IDC_PORT5_EDIT, m_port5_EDIT);
	DDX_Text(pDX, IDC_PORT5_EDIT, m_nTargetPort5);

	DDX_Control(pDX, IDC_LOCALPORT_EDIT, m_localport_EDIT);
	DDX_Text(pDX, IDC_LOCALPORT_EDIT, m_nLocalPort);

}

BEGIN_MESSAGE_MAP(CudphubDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CudphubDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CudphubDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDCLOSE, &CudphubDlg::OnClickedIdclose)
	ON_BN_CLICKED(ID_BUTTON_ABOUT, &CudphubDlg::OnBnClickedButtonAbout)
END_MESSAGE_MAP()


// CudphubDlg 消息处理程序

BOOL CudphubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ip1.SetAddress(192, 168, 31, 52);
	m_ip2.SetAddress(192, 168, 31, 53);
	m_ip3.SetAddress(192, 168, 31, 54);
	m_ip4.SetAddress(192, 168, 31, 55);
	m_ip5.SetAddress(192, 168, 31, 56);
	// OnInitDialog 末尾：初始化 Winsock
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		AfxMessageBox(_T("WSAStartup 失败"));
		return TRUE;
	}

	// 建议：IDCLOSE 初始禁用，避免未启动就点停止
	GetDlgItem(IDCLOSE)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CudphubDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CudphubDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CudphubDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CudphubDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	// 已在运行则忽略
	if (InterlockedCompareExchange(&m_bRunning, 1, 1) == 1)
		return;

	auto checkPort = [](UINT p) { return p >= 0 && p <= 65535; };
	if (!checkPort(m_nLocalPort) ||
		!checkPort(m_nTargetPort1) || !checkPort(m_nTargetPort2) ||
		!checkPort(m_nTargetPort3) || !checkPort(m_nTargetPort4) ||
		!checkPort(m_nTargetPort5)) {
		AfxMessageBox(_T("端口必须在 1~65535 之间"));
		return;
	}

	// 取 m_ip1 的 IP。CIPAddressCtrl::GetAddress 返回的 DWORD
	// 第一个字段在最低字节，内存布局即网络字节序，可直接赋给 s_addr
	DWORD dwAddr1 = 0;
	m_ip1.GetAddress(dwAddr1);
	if (dwAddr1 == 0) {
		AfxMessageBox(_T("请先填写有效的目标 IP 地址"));
		return;
	}
	m_dwTargetIp1 = htonl(dwAddr1);

	DWORD dwAddr2 = 0;
	m_ip2.GetAddress(dwAddr2);
	if (dwAddr2 == 0) {
		AfxMessageBox(_T("请先填写有效的目标 IP 地址"));
		return;
	}
	m_dwTargetIp2 = htonl(dwAddr2);

	DWORD dwAddr3 = 0;
	m_ip3.GetAddress(dwAddr3);
	if (dwAddr3 == 0) {
		AfxMessageBox(_T("请先填写有效的目标 IP 地址"));
		return;
	}
	m_dwTargetIp3 = htonl(dwAddr3);

	DWORD dwAddr4 = 0;
	m_ip4.GetAddress(dwAddr4);
	if (dwAddr4 == 0) {
		AfxMessageBox(_T("请先填写有效的目标 IP 地址"));
		return;
	}
	m_dwTargetIp4 = htonl(dwAddr4);

	DWORD dwAddr5 = 0;
	m_ip5.GetAddress(dwAddr5);
	if (dwAddr5 == 0) {
		AfxMessageBox(_T("请先填写有效的目标 IP 地址"));
		return;
	}
	m_dwTargetIp5 = htonl(dwAddr5);
	


	m_rxCount = m_fwdCount = m_errCount = 0;
	InterlockedExchange(&m_bRunning, 1);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, RecvThreadProc, this, 0, nullptr);
	if (m_hThread == nullptr) {
		InterlockedExchange(&m_bRunning, 0);
		AfxMessageBox(_T("创建接收线程失败"));
		return;
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS2)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS3)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS4)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS5)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT1_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT2_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT3_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT4_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT5_EDIT)->EnableWindow(FALSE); 
	GetDlgItem(IDC_LOCALPORT_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDCLOSE)->EnableWindow(TRUE);
	
}



void CudphubDlg::OnClickedIdclose()
{
	// TODO: 在此添加控件通知处理程序代码
	StopForwarding();
	UpdateData(FALSE);
	GetDlgItem(IDCLOSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS2)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS3)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS4)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS5)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT1_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT2_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT3_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT4_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT5_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOCALPORT_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CudphubDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	StopForwarding();
	WSACleanup();
	CDialogEx::OnCancel();
}

void CudphubDlg::StopForwarding()
{
	if (InterlockedCompareExchange(&m_bRunning, 0, 1) != 1 && m_hThread == nullptr)
		return;

	// 线程在 select 上最多等 200ms 就会检查到 m_bRunning==0 并退出
	if (m_hThread) {
		WaitForSingleObject(m_hThread, 3000);
		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}
}

unsigned __stdcall CudphubDlg::RecvThreadProc(void* pParam)
{
	CudphubDlg* pDlg = static_cast<CudphubDlg*>(pParam);

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
		return 0;

	// 绑定本机端口
	sockaddr_in local{};
	local.sin_family = AF_INET;
	local.sin_port = htons((u_short)pDlg->m_nLocalPort);
	local.sin_addr.s_addr = INADDR_ANY;
	if (bind(s, (sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
		closesocket(s);
		return 0;
	}

	// 启动时把 5 个目标拷贝到本地数组，避免线程内反复读取成员
	DWORD ips[5] = { pDlg->m_dwTargetIp1, pDlg->m_dwTargetIp2,
					   pDlg->m_dwTargetIp3, pDlg->m_dwTargetIp4,
					   pDlg->m_dwTargetIp5 };
	UINT  ports[5] = { pDlg->m_nTargetPort1, pDlg->m_nTargetPort2,
					   pDlg->m_nTargetPort3, pDlg->m_nTargetPort4,
					   pDlg->m_nTargetPort5 };

	sockaddr_in targets[5] = {};
	for (int i = 0; i < 5; ++i) {
		targets[i].sin_family = AF_INET;
		targets[i].sin_port = htons((u_short)ports[i]);
		targets[i].sin_addr.s_addr = ips[i];
	}

	char buf[2048];
	while (InterlockedCompareExchange(&pDlg->m_bRunning, 1, 1) == 1) {
		fd_set rfds; FD_ZERO(&rfds); FD_SET(s, &rfds);
		timeval tv{ 0, 200000 };
		int r = select(0, &rfds, nullptr, nullptr, &tv);
		if (r <= 0) continue;

		sockaddr_in from{};
		int fl = sizeof(from);
		int n = recvfrom(s, buf, sizeof(buf), 0, (sockaddr*)&from, &fl);
		if (n <= 0) {
			CString dbg; dbg.Format(_T("[recv] fail n=%d err=%d\n"), n, WSAGetLastError());
			OutputDebugString(dbg);
			continue;
		}
		InterlockedIncrement64((volatile LONG64*)&pDlg->m_rxCount);

		// 依次转发给 5 个目标
		for (int i = 0; i < 5; ++i) {
			if (ips[i] == 0 || ports[i] == 0 || ports[i] > 65535)
				continue;   // 未配置的目标跳过（防御）

			int sent = sendto(s, buf, n, 0,
				(sockaddr*)&targets[i], sizeof(targets[i]));
			if (sent == SOCKET_ERROR) {
				InterlockedIncrement64((volatile LONG64*)&pDlg->m_errCount);
				CString dbg;
				dbg.Format(_T("[fwd#%d] FAIL n=%d err=%d\n"),
					i + 1, n, WSAGetLastError());
				OutputDebugString(dbg);
			}
			else {
				InterlockedIncrement64((volatile LONG64*)&pDlg->m_fwdCount);
			}
		}
	}

	closesocket(s);
	return 0;
}
void CudphubDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}
