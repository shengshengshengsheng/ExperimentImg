
// ExperimentImgDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include <cstring>
#include "ImageProcess.h"
#include <ctime>
using namespace std;
// CExperimentImgDlg �Ի���
class CExperimentImgDlg : public CDialogEx
{
	// ����
public:
	CExperimentImgDlg(CWnd* pParent = NULL);	// ��׼���캯��
	void LogInfo(const string& str);
	void paintPic(CImage* img, CStatic& pic);
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPERIMENTIMG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	CImage* getImage() { return m_pImgSrc; }
	// ʵ��
protected:
	HICON m_hIcon;
	CImage * m_pImgSrc;
	CImage * m_pImgDst;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	CEdit mEditInfo;
	CStatic picLeft;
	CStatic picRight;
	CStatic messageText;
	CStatic labelText;
	CSliderCtrl slider;
	CStatic threadNum;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	CButton checkBox;
	CComboBox comboBox;
};
