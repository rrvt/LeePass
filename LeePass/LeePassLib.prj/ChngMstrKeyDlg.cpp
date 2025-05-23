// Change Master Key


#include "pch.h"
#include "ChngMstrKeyDlg.h"



IMPLEMENT_DYNAMIC(ChngMstrKeyDlg, CDialogEx)


BEGIN_MESSAGE_MAP(ChngMstrKeyDlg, CDialogEx)
  ON_BN_CLICKED(IDC_ToggleVisible, &ChngMstrKeyDlg::onToggleVisible)
END_MESSAGE_MAP()


ChngMstrKeyDlg::ChngMstrKeyDlg(CWnd* pParent) :
                                             CDialogEx(IDD_ChangePswd, pParent), visible(false) { }

ChngMstrKeyDlg::~ChngMstrKeyDlg() { }


BOOL ChngMstrKeyDlg::OnInitDialog() {

  CDialogEx::OnInitDialog();

  toggleVisibility(IDC_CurrentPswd);
  toggleVisibility(IDC_NewPswd);
  toggleVisibility(IDC_ConfirmPswd);

  return TRUE;
  }


void ChngMstrKeyDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_CurrentPswd, curPswd);
  DDX_Text(pDX, IDC_NewPswd,     newPswd);
  DDX_Text(pDX, IDC_ConfirmPswd, confirmPswd);
  }



void ChngMstrKeyDlg::onToggleVisible() {
  visible ^= true;
  toggleVisibility(IDC_CurrentPswd);
  toggleVisibility(IDC_NewPswd);
  toggleVisibility(IDC_ConfirmPswd);
  }



///////////-----------------

// ChngMstrKeyDlg message handlers

/*
void CYourDialog::OnBnClickedTogglePassword()
{
    static bool isPasswordVisible = false;
    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);

    if (pEdit)
    {
        if (isPasswordVisible)
        {
            pEdit->SetPasswordChar('*'); // Mask the password
        }
        else
        {
            pEdit->SetPasswordChar(0);  // Show the password
        }
        isPasswordVisible = !isPasswordVisible;
        pEdit->Invalidate(); //Refresh the edit control to reflect the change
    }
}
*/
//static TCchar BulletCh = 0x25CF;
//CEdit* ctl = (CEdit*) GetDlgItem(idc);    if (!ctl) return;
#if 1
#else
  if () ctl->SetPasswordChar(0);
  else         ctl->SetPasswordChar(BulletCh);
  ctl->Invalidate();
#endif


//void ChngMstrKeyDlg::toggleVisibility(int idc) {setVisible((CEdit*) GetDlgItem(idc), visible);}

