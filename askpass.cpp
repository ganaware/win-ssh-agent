// -*- tab-width: 8; -*-
#include <stdio.h>
#include <string>
#include <locale.h>
#include "misc.h"
#include "askpassrc.h"

//
class DialogPassphrase : private noncopyable
{
  HWND m_hwnd;					// HWND of dialog box
  std::wstring m_prompt;			// prompt
  std::wstring m_passPhrase;			// result
  
private:
  static INT_PTR CALLBACK dlgProc(HWND i_hwnd, UINT i_message,
				  WPARAM i_wParam, LPARAM i_lParam)
  {
    DialogPassphrase *This =
      reinterpret_cast<DialogPassphrase *>(
	GetWindowLongPtrW(i_hwnd, GWLP_USERDATA));
    if (!This)
      switch (i_message)
      {
	case WM_INITDIALOG:
	  This = reinterpret_cast<DialogPassphrase *>(i_lParam);
	  SetWindowLongPtrW(i_hwnd, GWLP_USERDATA, i_lParam);
	  This->initialize(i_hwnd);
	  return This->wmInitDialog(reinterpret_cast<HWND>(i_wParam));
      }
    else
      switch (i_message)
      {
	case WM_CLOSE:
	  return This->wmClose();
	case WM_COMMAND:
	  return This->wmCommand(HIWORD(i_wParam), LOWORD(i_wParam),
				 reinterpret_cast<HWND>(i_lParam));
      }
    return FALSE;
  }

  //
  void initialize(HWND i_hwnd)
  {
    m_hwnd = i_hwnd;
  }
  
  // WM_INITDIALOG
  BOOL wmInitDialog(HWND /* i_focus */)
  {
    SetWindowTextW(GetDlgItem(m_hwnd, IDC_PROMPT), m_prompt.c_str());
    setSmallIcon(m_hwnd, IDI_ASK_PASS);
    setBigIcon(m_hwnd, IDI_ASK_PASS);
    return TRUE;
  }

  // WM_CLOSE
  BOOL wmClose()
  {
    EndDialog(m_hwnd, 0);
    return TRUE;
  }

  // WM_COMMAND
  BOOL wmCommand(int /* i_notifyCode */, int i_id, HWND /* i_hwndControl */)
  {
    switch (i_id)
    {
      case IDOK:
      {
	wchar_t buf[1024];
	GetWindowTextW(GetDlgItem(m_hwnd, IDC_PASSPHRASE), buf, NUMBER_OF(buf));
	m_passPhrase = buf;
	EndDialog(m_hwnd, 1);
	return TRUE;
      }
      case IDCANCEL:
      {
	EndDialog(m_hwnd, 0);
	return TRUE;
      }
    }
    return FALSE;
  }

public:
  //
  DialogPassphrase(const std::wstring &i_prompt)
    : m_hwnd(NULL),
      m_prompt(i_prompt)
  {
  }

  //
  INT_PTR dialogBox(HWND i_hwndParent)
  {
    return DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_ASK_PASS),
			  i_hwndParent, dlgProc,
			  reinterpret_cast<LPARAM>(this));
  }
  
  //
  const std::wstring &getPassPhrase() const
  {
    return m_passPhrase;
  }
};

// main
int main(int i_argc, char **i_argv)
{
  setlocale(LC_ALL, "");
  
  g_hInst = GetModuleHandle(NULL);
  
  DialogPassphrase dp(2 <= i_argc ? to_wstring(i_argv[1]).c_str() : L"");
  if (dp.dialogBox(NULL))
    printf("%ls\n", dp.getPassPhrase().c_str());
  else
    printf("\n");
  return 0;
}
