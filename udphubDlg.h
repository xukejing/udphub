
// udphubDlg.h: 头文件
//

#pragma once
#pragma comment(lib, "ws2_32.lib")

// CudphubDlg 对话框
class CudphubDlg : public CDialogEx
{
// 构造
public:
	CudphubDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPHUB_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK() {}    // 重写为空，屏蔽Enter键的默认关闭行为

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClickedIdclose();
	CIPAddressCtrl m_ip1;
	CIPAddressCtrl m_ip2;
	CIPAddressCtrl m_ip3;
	CIPAddressCtrl m_ip4;
	CIPAddressCtrl m_ip5;

protected:
	volatile LONG m_bRunning;      // 1=运行中
	HANDLE        m_hThread;       // 接收线程句柄
	UINT          m_nLocalPort;    // 本机监听端口，20778
	UINT          m_nTargetPort1;   // 目标端口，20777
	UINT          m_nTargetPort2;   // 目标端口
	UINT          m_nTargetPort3;   // 目标端口
	UINT          m_nTargetPort4;   // 目标端口
	UINT          m_nTargetPort5;   // 目标端口
	DWORD         m_dwTargetIp1;    // m_ip1 的 IP（网络字节序内存布局）
	DWORD         m_dwTargetIp2;    // m_ip1 的 IP（网络字节序内存布局）
	DWORD         m_dwTargetIp3;    // m_ip1 的 IP（网络字节序内存布局）
	DWORD         m_dwTargetIp4;    // m_ip1 的 IP（网络字节序内存布局）
	DWORD         m_dwTargetIp5;    // m_ip1 的 IP（网络字节序内存布局）
	UINT64        m_rxCount;       // 接收计数（原子）
	UINT64        m_fwdCount;      // 转发成功计数
	UINT64        m_errCount;      // 发送失败计数

	static unsigned __stdcall RecvThreadProc(void* pParam);
	void StopForwarding();
public:
	CEdit m_port1_EDIT;
	CEdit m_port2_EDIT;
	CEdit m_port3_EDIT;
	CEdit m_port4_EDIT;
	CEdit m_port5_EDIT;
	CEdit m_localport_EDIT;
	afx_msg void OnBnClickedButtonAbout();
};
