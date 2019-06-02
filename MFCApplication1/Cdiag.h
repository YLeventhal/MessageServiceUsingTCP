#pragma once


// Cdiag dialog

class Cdiag : public CDialog
{
	DECLARE_DYNAMIC(Cdiag)

public:
	Cdiag(CWnd* pParent = nullptr);   // standard constructor
	virtual ~Cdiag();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
