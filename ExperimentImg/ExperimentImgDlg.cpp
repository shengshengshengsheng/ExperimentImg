
// ExperimentImgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ExperimentImg.h"
#include "ExperimentImgDlg.h"
#include "afxdialogex.h"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CExperimentImgDlg �Ի���



CExperimentImgDlg::CExperimentImgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EXPERIMENTIMG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//���ضԻ����ʱ���ʼ��
	m_pImgSrc = NULL;
}

void CExperimentImgDlg::LogInfo(const string & str)
{
	this->messageText.SetWindowTextW(CString(str.c_str()));
}


void CExperimentImgDlg::paintPic(CImage * img, CStatic & pic)
{
	int height;
	int width;
	CRect rect;
	CRect rect1;
	height = img->GetHeight();
	width = img->GetWidth();
	pic.GetClientRect(&rect);
	CDC *pDC = pic.GetDC();
	SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);
	if (width <= rect.Width() && height <= rect.Width())
	{
		rect1 = CRect(rect.TopLeft(), CSize(width, height));
		img->StretchBlt(pDC->m_hDC, rect1, SRCCOPY);
	}
	else
	{
		float xScale = (float)rect.Width() / (float)width;
		float yScale = (float)rect.Height() / (float)height;
		float ScaleIndex = (xScale <= yScale ? xScale : yScale);
		rect1 = CRect(rect.TopLeft(), CSize((int)width*ScaleIndex, (int)height*ScaleIndex));
		img->StretchBlt(pDC->m_hDC, rect1, SRCCOPY);
	}
	ReleaseDC(pDC);
}


void CExperimentImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
	DDX_Control(pDX, IDC_PICTURE, picLeft);
	DDX_Control(pDX, IDC_PICTURE2, picRight);
	DDX_Control(pDX, IDC_STATIC3, messageText);
	DDX_Control(pDX, IDC_STATIC2, labelText);
	DDX_Control(pDX, IDC_SLIDER1, slider);
	DDX_Control(pDX, IDC_STATIC5, threadNum);
	DDX_Control(pDX, IDC_CHECK1, checkBox);
	DDX_Control(pDX, IDC_COMBO1, comboBox);
}

BEGIN_MESSAGE_MAP(CExperimentImgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CExperimentImgDlg::OnBnClickedButtonOpen)
//	ON_EN_CHANGE(IDC_EDIT1, &CExperimentImgDlg::OnEnChangeEdit1)
//	ON_EN_CHANGE(IDC_EDIT_INFO, &CExperimentImgDlg::OnEnChangeEditInfo)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CExperimentImgDlg::OnNMCustomdrawSlider1)
ON_BN_CLICKED(IDC_BUTTON2, &CExperimentImgDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CExperimentImgDlg ��Ϣ�������

BOOL CExperimentImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	mEditInfo.SetWindowTextW(CString("File Path"));
	this->labelText.SetWindowTextW(CString("�߳���"));
	this->slider.SetRange(1, 8);
	this->slider.SetPos(1);
	this->threadNum.SetWindowTextW(CString(to_string(this->slider.GetPos()).c_str()));
	CComboBox * cmb_function = ((CComboBox*)GetDlgItem(IDC_COMBO1));
	cmb_function->AddString(_T("��������"));
	cmb_function->AddString(_T("��ֵ�˲�"));
	cmb_function->AddString(_T("˫���ײ�ֵ�����ţ�"));
	cmb_function->AddString(_T("˫���ײ�ֵ����ת��"));
	cmb_function->AddString(_T("�Զ�ɫ��"));
	cmb_function->AddString(_T("�Զ���ƽ��"));
	cmb_function->AddString(_T("����Ӧ˫���˲�"));
	cmb_function->AddString(_T("�ں�����ͼ"));
	cmb_function->SetCurSel(0);
	srand(time(nullptr));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CExperimentImgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CExperimentImgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		if (m_pImgSrc != NULL)
		{
			this->paintPic(m_pImgSrc, picLeft);
		}
		if (m_pImgDst != NULL)
		{
			this->paintPic(m_pImgDst, picRight);
		}
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CExperimentImgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExperimentImgDlg::OnBnClickedButtonOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilter[] = _T("JPEG(*jpg)|*.jpg|*.bmp|TIFF(*.tif)|*.tif|All Files ��*.*��|*.*||");
	CString filePath("");
	
	CFileDialog fileOpenDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDialog.DoModal() == IDOK)
	{
		VERIFY(filePath = fileOpenDialog.GetPathName());
		CString strFilePath(filePath);
		mEditInfo.SetWindowTextW(strFilePath);	//���ı�������ʾͼ��·��

		if (m_pImgSrc != NULL)
		{
			m_pImgSrc->Destroy();
			delete m_pImgSrc;
		}
		m_pImgSrc = new CImage();
		m_pImgSrc->Load(strFilePath);

		this->Invalidate();

	}
}



void CExperimentImgDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	this->threadNum.SetWindowTextW(CString(to_string(this->slider.GetPos()).c_str()));
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CExperimentImgDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int threadNum = this->slider.GetPos();
	bool isCurrent = this->checkBox.GetCheck();
	CString typeName;
	auto index = this->comboBox.GetCurSel();
	this->comboBox.GetLBText(index, typeName);
	if (this->m_pImgSrc) {
		this->LogInfo("������");
		auto initTime = clock();
		if (typeName == "�ں�����ͼ") {
			this->m_pImgDst=ImageProcess::merge(m_pImgSrc, m_pImgDst, double(threadNum)/8);
		}else {
			ImageProcess imgPro(this->m_pImgSrc, typeName, threadNum, isCurrent);
			this->m_pImgDst = imgPro.process_photo();
		}
		this->paintPic(m_pImgDst, picRight);
		this->LogInfo(string("������ɡ���ʱ")+to_string(clock()-initTime)+string("����"));
	}
	else {
		this->LogInfo("���ȴ�ͼƬ");
	}
}