#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <windows.h>
#include <sys/cygwin.h>
#include <sys/wait.h>
#include <unistd.h>
#include "resource.h"
#include <list>


// instance
HINSTANCE g_hInst;

// get number of array elements
#define NUMBER_OF(i_array) (sizeof(i_array) / sizeof((i_array)[0]))


// load small icon resource
HICON loadSmallIcon(UINT i_id)
{
  return reinterpret_cast<HICON>(
    LoadImage(g_hInst, MAKEINTRESOURCE(i_id), IMAGE_ICON, 16, 16, 0));
}

// write string to registry
bool writeRegistry(HKEY i_root, const std::string &i_path,
		   const std::string &i_name, const std::string &i_value)
{
  HKEY hkey;
  DWORD disposition;
  if (ERROR_SUCCESS !=
      RegCreateKeyEx(i_root, i_path.c_str(), 0, "",
		     REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, NULL, &hkey, &disposition))
    return false;
  RegSetValueEx(hkey, i_name.c_str(), 0, REG_SZ,
		(BYTE *)i_value.c_str(),
		(i_value.size() + 1) * sizeof(std::string::value_type));
  RegCloseKey(hkey);
  return true;
}

// remove registry
bool removeRegistry(HKEY i_root, const std::string &i_path,
		    const std::string &i_name)
{
  if (i_name.empty())
    return RegDeleteKey(i_root, i_path.c_str()) == ERROR_SUCCESS;
  HKEY hkey;
  if (ERROR_SUCCESS !=
      RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_SET_VALUE, &hkey))
    return false;
  LONG r = RegDeleteValue(hkey, i_name.c_str());
  RegCloseKey(hkey);
  return r == ERROR_SUCCESS;
}

// get the path of win-ssh-askpass.exe
std::string getSelfPath()
{
  char win32_pathname[1024];
  GetModuleFileName(NULL, win32_pathname, NUMBER_OF(win32_pathname));
  char posix_pathname[1024];
  cygwin_conv_to_posix_path(win32_pathname, posix_pathname);
  return posix_pathname;
}

// run ssh-add
int run_ssh_add(const char *i_identityFile)
{
  pid_t childPID = fork();
  setenv("SSH_ASKPASS", getSelfPath().c_str(), 1);
  if (childPID == 0)				// child process
  {
    close(0);
    setenv("DISPLAY", "localhost:0", 0);
    execlp("ssh-add", "ssh-add", i_identityFile, NULL);
  }
  while (true)
  {
    int status;
    int pid = wait3(&status, 0, NULL);
    if (pid == childPID || pid <= 0)
      if (WIFEXITED(status))
	return WEXITSTATUS(status);
      else 
	return -1;
  }
}

// run any program
void run_program(char **i_argv)
{
  char win32_pathname[1024];
  cygwin_conv_to_win32_path(i_argv[0], win32_pathname);
  
  if (i_argv[1])
  {
    std::string param;
    for (int i = 1; i_argv[i]; ++ i)
    {
      std::string arg(" \"");
      for (const char *p = i_argv[i]; *p; ++ p)
	if (*p == '"')
	  arg += "\"\"";
	else
	  arg += *p;
      arg += "\"";
      param += arg;
    }
    ShellExecute(NULL, "open", win32_pathname, param.c_str(),
		 NULL, SW_SHOWDEFAULT);
  }
  else
    ShellExecute(NULL, "open", win32_pathname, NULL, NULL, SW_SHOWDEFAULT);
}

// global variables to save options
typedef std::list<const char *> IdentityFiles;
static IdentityFiles g_option_identityFiles; // --identity, --default-identity
static  const char *g_option_defaultIdentityFile = NULL; // --default-identity
static bool g_option_DISPLAY = true;		// --DISPLAY, --no-DISPLAY
static char **g_option_execArgs = NULL;		// --exec

//
bool checkOptions(int i_argc, char **i_argv)
{
  typedef std::list<const char *> UnknownOptions;
  UnknownOptions unknownOptions;

  // check options
  for (int i = 1; i < i_argc; ++ i)
  {
    std::string a = i_argv[i];
    if (a == "--no-ssh-agent")
      ;					// ignore
    else if (a == "--DISPLAY")
      g_option_DISPLAY = true;
    else if (a == "--no-DISPLAY")
      g_option_DISPLAY = false;
    else if (i + 1 < i_argc)
    {
      if (a == "-i" || a == "--identity")
	g_option_identityFiles.push_back(i_argv[++ i]);
      else if (a == "-I" || a == "--default-identity")
	g_option_identityFiles.push_back(
	  g_option_defaultIdentityFile = i_argv[++ i]);
      else if (a == "-e" || a == "--exec")
      {
	g_option_execArgs = &i_argv[++ i];
	break;
      }
      else
	unknownOptions.push_back(i_argv[i]);
    }
    else
      unknownOptions.push_back(i_argv[i]);
  }

  if (0 < unknownOptions.size())
  {
    std::string message("Unknown options are specified:\r\n");
    for (UnknownOptions::iterator
	   i = unknownOptions.begin(); i != unknownOptions.end(); ++ i)
    {
      message += "  ";
      message += *i;
      message += "\r\n";
    }
    MessageBox(NULL, message.c_str(),
	       "win-ssh-askpass", MB_OK | MB_ICONSTOP);
    return false;
  }
  return true;
}

//
class TaskTray
{
  HWND m_hwndTaskTray;				// tasktray window
  NOTIFYICONDATA m_ni;				// taskbar icon data
  HMENU m_hMenuTaskTray;			// tasktray menu
  UINT m_WM_TaskbarRestart;			/* window message sent when
						   taskber restarts */
  enum
  { 
    WM_APP_taskTrayNotify = WM_APP + 101,
    ID_TaskTrayIcon = 1,
  };

private:
  // register class for tasktray
  ATOM Register_tasktray()
  {
    WNDCLASS wc;
    wc.style         = 0;
    wc.lpfnWndProc   = wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(TaskTray *);
    wc.hInstance     = g_hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "TaskTray";
    return RegisterClass(&wc);
  }
  
  // window procedure for tasktray
  static LRESULT CALLBACK
  wndProc(HWND i_hwnd, UINT i_message, WPARAM i_wParam, LPARAM i_lParam)
  {
    TaskTray *This = reinterpret_cast<TaskTray *>(GetWindowLong(i_hwnd, 0));

    if (!This)
      switch (i_message)
      {
	case WM_CREATE:
	  This = reinterpret_cast<TaskTray *>(
	    reinterpret_cast<CREATESTRUCT *>(i_lParam)->lpCreateParams);
	  SetWindowLong(i_hwnd, 0, (long)This);
	  return 0;
      }
    else
      switch (i_message)
      {
        case WM_QUERYENDSESSION:
	  PostQuitMessage(0);
	  return TRUE;
	  
	case WM_APP_taskTrayNotify:
	{
	  if (i_wParam == ID_TaskTrayIcon)
	    switch (i_lParam)
	    {
	      case WM_RBUTTONUP:
	      {
		POINT p;
		GetCursorPos(&p);
		SetForegroundWindow(i_hwnd);
		HMENU hMenuSub = GetSubMenu(This->m_hMenuTaskTray, 0);
		TrackPopupMenu(hMenuSub, TPM_LEFTALIGN,
			       p.x, p.y, 0, i_hwnd, NULL);
		// TrackPopupMenu may fail (ERROR_POPUP_ALREADY_ACTIVE)
		break;
	      }
	      
	      case WM_LBUTTONDBLCLK:
		SendMessage(i_hwnd, WM_COMMAND,
			    MAKELONG(ID_MENUITEM_Add, 0), 0);
		break;
	    }
	  return 0;
	}
      
	case WM_COMMAND:
	{
	  int notify_code = HIWORD(i_wParam);
	  int id = LOWORD(i_wParam);
	  if (notify_code == 0) // menu
	    switch (id)
	    {
	      case ID_MENUITEM_Add:
		run_ssh_add(NULL);
		break;
	      case ID_MENUITEM_Exit:
		PostQuitMessage(0);
		break;
	      default:
		if (ID_MENUITEM_Add + 1 <= id &&
		    id < (int)(ID_MENUITEM_Add + 1
			       + g_option_identityFiles.size()))
		{
		  IdentityFiles::iterator itr = g_option_identityFiles.begin();
		  for (int i = id - ID_MENUITEM_Add - 1; 0 < i; -- i)
		    ++ itr;
		  run_ssh_add(*itr);
		}
		break;
	    }
	  return 0;
	}

	default:
	  if (i_message == This->m_WM_TaskbarRestart)
	  {
	    This->showTasktrayIcon(true);
	    return 0;
	  }
      }
    return DefWindowProc(i_hwnd, i_message, i_wParam, i_lParam);
  }

  // change the task tray icon
  void showTasktrayIcon(bool i_doesAdd = false)
  {
    Shell_NotifyIcon(i_doesAdd ? NIM_ADD : NIM_MODIFY, &m_ni);
  }

public:
  //
  TaskTray()
    : m_hwndTaskTray(NULL),
      m_WM_TaskbarRestart(RegisterWindowMessage("TaskbarCreated"))
  {
    Register_tasktray();
    m_hwndTaskTray =
      CreateWindow("TaskTray", "win-ssh-agent",
		   WS_OVERLAPPEDWINDOW,
		   CW_USEDEFAULT, CW_USEDEFAULT, 
		   CW_USEDEFAULT, CW_USEDEFAULT, 
		   NULL, NULL, g_hInst, this);
    
    // show tasktray icon
    std::memset(&m_ni, 0, sizeof(m_ni));
    m_ni.cbSize = sizeof(m_ni);
    m_ni.uID    = ID_TaskTrayIcon;
    m_ni.hWnd   = m_hwndTaskTray;
    m_ni.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_ni.hIcon  = loadSmallIcon(IDI_TASKTRAY);
    m_ni.uCallbackMessage = WM_APP_taskTrayNotify;
    strncpy(m_ni.szTip, "win-ssh-agent", NUMBER_OF(m_ni.szTip));
    showTasktrayIcon(true);
    
    // create menu
    m_hMenuTaskTray = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_TASKTRAY));
    HMENU hMenuSub = GetSubMenu(m_hMenuTaskTray, 0);
    
    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_ID | MIIM_TYPE;
    mii.fType = MFT_STRING;
    
    if (g_option_identityFiles.size())
    {
      mii.wID = ID_MENUITEM_Add;
      mii.dwTypeData = "ssh-add";
      mii.cch = 7;
      InsertMenuItem(hMenuSub, 0, TRUE, &mii);
      int n = 1;
      for (IdentityFiles::iterator
	     i = g_option_identityFiles.begin();
	   i != g_option_identityFiles.end(); ++ i, ++ n)
      {
	mii.wID = ID_MENUITEM_Add + n;
	std::string text("ssd-add ");
	text += *i;
	mii.dwTypeData = (char *)text.c_str();
	mii.cch = text.size();
	InsertMenuItem(hMenuSub, n, TRUE, &mii);
      }
    }
    else
    {
      mii.wID = ID_MENUITEM_Add;
      mii.dwTypeData = "&Add ...";
      mii.cch = 8;
      InsertMenuItem(hMenuSub, 0, TRUE, &mii);
    }
    SetMenuDefaultItem(hMenuSub, 0, TRUE);
  }

  //
  ~TaskTray()
  {
    // destroy menu
    DestroyMenu(m_hMenuTaskTray);
    
    // delete tasktray icon
    Shell_NotifyIcon(NIM_DELETE, &m_ni);
    DestroyIcon(m_ni.hIcon);
  }

  // message loop
  WPARAM messageLoop()
  {
    MSG msg;
    while (0 < GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return msg.wParam;
  }  
};

//
class DialogPassphrase
{
  HWND m_hwnd;					// HWND of dialog box
  std::string m_prompt;				// prompt
  std::string m_passPhrase;			// result
  
private:
  static BOOL CALLBACK dlgProc(HWND i_hwnd, UINT i_message,
			       WPARAM i_wParam, LPARAM i_lParam)
  {
    DialogPassphrase *This =
      reinterpret_cast<DialogPassphrase *>(
	GetWindowLong(i_hwnd, GWL_USERDATA));
    if (!This)
      switch (i_message)
      {
	case WM_INITDIALOG:
	  This = reinterpret_cast<DialogPassphrase *>(i_lParam);
	  SetWindowLong(i_hwnd, GWL_USERDATA, i_lParam);
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
    SetWindowText(GetDlgItem(m_hwnd, IDC_PROMPT), m_prompt.c_str());
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
	TCHAR buf[1024];
	GetWindowText(GetDlgItem(m_hwnd, IDC_PASSPHRASE), buf, NUMBER_OF(buf));
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
  DialogPassphrase(const std::string &i_prompt)
    : m_hwnd(NULL),
      m_prompt(i_prompt)
  {
  }

  //
  INT_PTR dialogBox(HWND i_hwndParent)
  {
    return DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_ASK_PASS),
			  i_hwndParent, dlgProc,
			  reinterpret_cast<LPARAM>(this));
  }
  
  //
  const std::string &getPassPhrase() const
  {
    return m_passPhrase;
  }
};

// main
int main(int i_argc, char **i_argv)
{
  g_hInst = GetModuleHandle(NULL);
  
  // 3. Executed via ssh-add
  if (getenv("SSH_ASKPASS") && getenv("DISPLAY")) // invoked as ASKPASS mode
  {
    DialogPassphrase dp(2 <= i_argc ? i_argv[1] : "");
    if (dp.dialogBox(NULL))
      printf("%s\n", dp.getPassPhrase().c_str());
    else
      printf("\n");
    return 0;
  }
  
  // 2. Executed by me via ssh-agent
  if (2 <= i_argc && i_argv[1] == std::string("--no-ssh-agent"))
  {
    if (!(getenv("SSH_AGENT_PID") && getenv("SSH_AUTH_SOCK")))
      return 1;

    if (!checkOptions(i_argc, i_argv))
      return 1;

    // is another running ?
    HANDLE mutex =
      CreateMutex(NULL, TRUE, "{6A1AFE71-F534-47a2-AB94-2ED3758C18AE}");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
      MessageBox(NULL, "Another WIN-SSH-ASKPASS is runnnig!",
		 "win-ssh-askpass", MB_OK | MB_ICONSTOP);
      return 1;
    }
    
    bool hasDISPLAY = !!getenv("DISPLAY");
    
    std::string selfPath(getSelfPath());
    setenv("SSH_ASKPASS", selfPath.c_str(), 1);
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AGENT_PID",
		  getenv("SSH_AGENT_PID"));
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AUTH_SOCK",
		  getenv("SSH_AUTH_SOCK"));
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_ASKPASS", selfPath);
    if (!hasDISPLAY && g_option_DISPLAY)
      writeRegistry(HKEY_CURRENT_USER, "Environment", "DISPLAY",
		    "localhost:0");
      
    DWORD returnValue;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		       (LPARAM)"Environment", SMTO_ABORTIFHUNG,
		       5000, &returnValue);

    if (g_option_defaultIdentityFile)
    {
      int status = run_ssh_add(g_option_defaultIdentityFile);
      if (g_option_execArgs && (status == 0))
	run_program(g_option_execArgs);
    }
    
    TaskTray().messageLoop();

    if (!hasDISPLAY && g_option_DISPLAY)
      removeRegistry(HKEY_CURRENT_USER, "Environment", "DISPLAY");
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_ASKPASS");
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AUTH_SOCK");
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AGENT_PID");
    
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		       (LPARAM)"Environment", SMTO_ABORTIFHUNG,
		       5000, &returnValue);

    CloseHandle(mutex);
    
    return 0;
  }

  // 1. Executed by user (from Explorer etc.)
  {
    const std::string selfPath(getSelfPath());
    char **argv = new char *[i_argc + 3];
    argv[0] = "ssh-agent";
    argv[1] = const_cast<char *>(selfPath.c_str());
    argv[2] = "--no-ssh-agent";
    for (int i = 1; i <= i_argc; ++ i)
      argv[i + 2] = i_argv[i];
    execvp("ssh-agent", argv);
    delete argv;
    return 1;
  }
}
