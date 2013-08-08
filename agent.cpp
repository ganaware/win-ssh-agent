// -*- tab-width: 8; -*-
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/cygwin.h>
#include <sys/wait.h>
#include <unistd.h>
#include <list>
#include <stdarg.h>
#include <errno.h>
#include <locale.h>

#include "agentrc.h"
#include "misc.h"

// global variables to save options

typedef std::list<const char *> IdentityFiles;

static IdentityFiles g_option_identityFiles; // --identity, --default-identity
static const char *g_option_defaultIdentityFile = ""; // --default-identity
static const char *g_option_bindAddress = NULL;	// -a
static const char *g_option_lifetime = NULL;	// -t
static bool g_option_no_ssh_agent = false;	// --no-ssh-agent
static bool g_option_DISPLAY = true;		// --DISPLAY, --no-DISPLAY
static char **g_option_execArgs = NULL;		// --exec
static bool g_option_verbose = false;		// --verbose

// verbose output
void verbose(const char *i_format, ...)
{
  if (!g_option_verbose)
    return;
  va_list args;
  va_start(args, i_format);
  vprintf(i_format, args);
  va_end(args);
  fflush(stdout);
}

void messagebox(const std::wstring &i_message) {
  verbose("%ls\n", i_message.c_str());
  MessageBoxW(NULL, i_message.c_str(), L"win-ssh-agent", MB_OK | MB_ICONSTOP);
}

// run ssh-agent
int run_ssh_agent() {
  std::string cmdline;
  cmdline += "ssh-agent -s";
  if (g_option_bindAddress) {
    cmdline += " -a ";
    cmdline += g_option_bindAddress;
  }
  if (g_option_lifetime) {
    cmdline += " -t ";
    cmdline += g_option_lifetime;
  }
  verbose("$ %s\n", cmdline.c_str());
  errno = 0;
  FILE *fp = popen(cmdline.c_str(), "rt");
  if (!fp) {
    std::wstring message(L"Failed to exec ssh-agent.exe : \n");
    message += to_wstring(sys_errlist[errno]);
    messagebox(message);
    return -1;
  }
  
  /*
   *	$ssh-agent -s
   *	SSH_AUTH_SOCK=/tmp/ssh-pzciDtAh4684/agent.4684; export SSH_AUTH_SOCK;
   *	SSH_AGENT_PID=5084; export SSH_AGENT_PID;
   *	echo Agent pid 5084;
   */
  
  const int BUFSIZE = 1024;
  char buf[BUFSIZE];
  while (fgets(buf, BUFSIZE, fp)) {
    verbose("%s", buf);
    
    // environment variable
    const char *var_begin = buf;
    const char *var_end = strchr(var_begin, '=');
    if (!var_end) {
      continue;
    }
    std::string var(var_begin, var_end);

    // environment value
    const char *value_begin = var_end + 1;
    const char *value_end = strchr(value_begin, ';');
    if (!value_end) {
      continue;
    }
    std::string value(value_begin, value_end);

    if (var == "SSH_AUTH_SOCK" || var == "SSH_AGENT_PID") {
      setenv(var.c_str(), value.c_str(), 1);
    }
  }
  
  pclose(fp);

  return 0;
}

void kill_ssh_agent() {
  if (getenv("SSH_AGENT_PID")) {
    const char *cmdline = "ssh-agent -s -k";
    verbose("$ %s\n", cmdline);
    if (FILE *fp = popen(cmdline, "rt")) {
      const int BUFSIZE = 1024;
      char buf[BUFSIZE];
      while (fgets(buf, BUFSIZE, fp)) {
	verbose("%s", buf);
      }
      pclose(fp);
      unsetenv("SSH_AUTH_SOCK");
      unsetenv("SSH_AGENT_PID");
    }
  }
}

// get the path of win-ssh-askpass.exe
std::string getAskpassPath()
{
  std::string selfPath(getSelfPath());
  std::string::size_type i = selfPath.find_last_of('/');
  if (i != std::string::npos)
    selfPath = selfPath.substr(0, i);
  return selfPath + "/win-ssh-askpass.exe";
}

// run ssh-add
int run_ssh_add(const char *i_identityFile)
{
  verbose("\n(run ssh-add)\n");
  pid_t childPID = fork();
  if (childPID == 0)				// child process
  {
    close(0);
    setenv("SSH_ASKPASS", getAskpassPath().c_str(), 1);
    verbose("$ export SSH_ASKPASS=%s\n", getenv("SSH_ASKPASS"));
    setenv("DISPLAY", ":0", 0);
    verbose("$ export DISPLAY=%s\n", getenv("DISPLAY"));
    verbose("$ ssh-add %s\n", i_identityFile ? i_identityFile : "");
    if (i_identityFile && !i_identityFile[0])
      i_identityFile = NULL;
    execlp("ssh-add", "ssh-add", i_identityFile, NULL);
    
    std::wstring message(L"Failed to exec ssh-add.exe : \r\n");
    message += to_wstring(sys_errlist[errno]);
    messagebox(message);
    return 1;
  }
  while (true)
  {
    int status;
    int pid = wait3(&status, 0, NULL);
    if (pid == childPID || pid <= 0)
    {
      if (WIFEXITED(status))
      {
	verbose("ssh-add exit(%d)\n", WEXITSTATUS(status));
#if 1
	if (WEXITSTATUS(status) == 2)
	{
	  messagebox(L"ssh-add exit(2) : \nCould not open a connection to your authentication agent.");
	}
#endif
	return WEXITSTATUS(status);
      }
      else 
      {
	return -1;
      }
    }
  }
}

// run any program
void run_program(char **i_argv)
{
  std::wstring win32_pathname = conv_path_posix_to_win(i_argv[0]);
  cygwin_internal(CW_SYNC_WINENV);

  HINSTANCE result;
  std::wstring param;
  if (i_argv[1])
  {
    for (int i = 1; i_argv[i]; ++ i)
    {
      std::wstring arg = to_wstring(i_argv[i]);
      std::wstring tmp = L" \"";
      for (const wchar_t *p = arg.c_str(); *p; ++ p)
	if (*p == L'"')
	  tmp += L"\"\"";
	else
	  tmp += *p;
      tmp += L"\"";
      param += tmp;
    }
    verbose("$ cygstart %ls %ls\n", win32_pathname.c_str(), param.c_str());
    result = 
      ShellExecuteW(NULL, L"open", win32_pathname.c_str(), param.c_str(),
		    NULL, SW_SHOWDEFAULT);
  }
  else
  {
    verbose("$ cygstart %ls\n", win32_pathname.c_str());
    result = 
      ShellExecuteW(NULL, L"open", win32_pathname.c_str(), NULL,
		    NULL, SW_SHOWDEFAULT);
  }
  intptr_t r = reinterpret_cast<intptr_t>(result);
  if (r < 32) {
    std::wstring message;
    message += L"Failed to run: ";
    message += win32_pathname;
    if (param.size()) {
      message += L" ";
      message += param;
    }
    message += L"\nShellExecuteW returned ";
    int BUFSIZE = 20;
    wchar_t buf[BUFSIZE];
    swprintf(buf, BUFSIZE, L"%d", r);
    message += buf;
    switch (reinterpret_cast<intptr_t>(result)) {
#define SHELL_EXECUTE_ERROR(name) case name: message += L" (" L## #name L")"; break
    SHELL_EXECUTE_ERROR(ERROR_FILE_NOT_FOUND);
    SHELL_EXECUTE_ERROR(ERROR_PATH_NOT_FOUND);
    SHELL_EXECUTE_ERROR(ERROR_BAD_FORMAT);
    SHELL_EXECUTE_ERROR(SE_ERR_ACCESSDENIED);
    SHELL_EXECUTE_ERROR(SE_ERR_ASSOCINCOMPLETE);
    SHELL_EXECUTE_ERROR(SE_ERR_DDEBUSY);
    SHELL_EXECUTE_ERROR(SE_ERR_DDEFAIL);
    SHELL_EXECUTE_ERROR(SE_ERR_DDETIMEOUT);
    SHELL_EXECUTE_ERROR(SE_ERR_DLLNOTFOUND);
    //SHELL_EXECUTE_ERROR(SE_ERR_FNF); == ERROR_FILE_NOT_FOUND
    SHELL_EXECUTE_ERROR(SE_ERR_NOASSOC);
    SHELL_EXECUTE_ERROR(SE_ERR_OOM);
    //SHELL_EXECUTE_ERROR(SE_ERR_PNF); == ERROR_PATH_NOT_FOUND
    SHELL_EXECUTE_ERROR(SE_ERR_SHARE);
#undef SHELL_EXECUTE_ERROR
    }
    messagebox(message);
  }
}

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
      g_option_no_ssh_agent = true;
    else if (a == "--DISPLAY")
      g_option_DISPLAY = true;
    else if (a == "--no-DISPLAY")
      g_option_DISPLAY = false;
    else if (a == "--hide-console")
      ;					// ignore
    else if (a == "--verbose")
      g_option_verbose = true;
    else if (i + 1 < i_argc)
    {
      if (a == "-i" || a == "--identity")
	g_option_identityFiles.push_back(i_argv[++ i]);
      else if (a == "-I" || a == "--default-identity")
      {
	++ i;
	if (std::string(i_argv[i]) == "-")
	  g_option_defaultIdentityFile = "";
	else
	{
	  g_option_identityFiles.push_back(i_argv[i]);
	  g_option_defaultIdentityFile = i_argv[i];
	}
      }
      else if (a == "--no-default-identity")
	g_option_defaultIdentityFile = NULL;
      else if (a == "-a")
	g_option_bindAddress = i_argv[++ i];
      else if (a == "-t")
	g_option_lifetime = i_argv[++ i];
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
    std::wstring message(L"Unknown options are specified:\r\n");
    for (UnknownOptions::iterator
	   i = unknownOptions.begin(); i != unknownOptions.end(); ++ i)
    {
      message += L"  ";
      message += to_wstring(*i);
      message += L"\r\n";
    }
    messagebox(message);
    return false;
  }
  return true;
}

//
class TaskTray : private noncopyable
{
  HWND m_hwndTaskTray;				// tasktray window
  NOTIFYICONDATAW m_ni;			// taskbar icon data
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
    WNDCLASSW wc;
    wc.style         = 0;
    wc.lpfnWndProc   = wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(TaskTray *);
    wc.hInstance     = g_hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"TaskTray";
    return RegisterClassW(&wc);
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
	    reinterpret_cast<CREATESTRUCTW *>(i_lParam)->lpCreateParams);
	  SetWindowLongW(i_hwnd, 0, (long)This);
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
		SendMessageW(i_hwnd, WM_COMMAND,
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
    return DefWindowProcW(i_hwnd, i_message, i_wParam, i_lParam);
  }

  // change the task tray icon
  void showTasktrayIcon(bool i_doesAdd = false)
  {
    Shell_NotifyIconW(i_doesAdd ? NIM_ADD : NIM_MODIFY, &m_ni);
  }

public:
  //
  TaskTray()
    : m_hwndTaskTray(NULL),
      m_WM_TaskbarRestart(RegisterWindowMessageW(L"TaskbarCreated"))
  {
    Register_tasktray();
    m_hwndTaskTray =
      CreateWindowW(L"TaskTray", L"win-ssh-agent",
		   WS_OVERLAPPEDWINDOW,
		   CW_USEDEFAULT, CW_USEDEFAULT, 
		   CW_USEDEFAULT, CW_USEDEFAULT, 
		   NULL, NULL, g_hInst, this);
    
    // show tasktray icon
    memset(&m_ni, 0, sizeof(m_ni));
    m_ni.cbSize = sizeof(m_ni);
    m_ni.uID    = ID_TaskTrayIcon;
    m_ni.hWnd   = m_hwndTaskTray;
    m_ni.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_ni.hIcon  = loadSmallIcon(IDI_TASKTRAY);
    m_ni.uCallbackMessage = WM_APP_taskTrayNotify;
    wcsncpy(m_ni.szTip, L"win-ssh-agent", NUMBER_OF(m_ni.szTip));
    showTasktrayIcon(true);
    
    // create menu
    m_hMenuTaskTray = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_TASKTRAY));
    HMENU hMenuSub = GetSubMenu(m_hMenuTaskTray, 0);
    
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_ID | MIIM_TYPE;
    mii.fType = MFT_STRING;
    
    if (g_option_identityFiles.size())
    {
      mii.wID = ID_MENUITEM_Add;
      mii.dwTypeData = const_cast<wchar_t *>(L"ssh-add");
      mii.cch = 7;
      InsertMenuItemW(hMenuSub, 0, TRUE, &mii);
      int n = 1;
      for (IdentityFiles::iterator
	     i = g_option_identityFiles.begin();
	   i != g_option_identityFiles.end(); ++ i, ++ n)
      {
	mii.wID = ID_MENUITEM_Add + n;
	std::wstring text(L"ssd-add ");
	text += to_wstring(*i);
	mii.dwTypeData = const_cast<wchar_t *>(text.c_str());
	mii.cch = text.size();
	InsertMenuItemW(hMenuSub, n, TRUE, &mii);
      }
    }
    else
    {
      mii.wID = ID_MENUITEM_Add;
      mii.dwTypeData = const_cast<wchar_t *>(L"&Add ...");
      mii.cch = 8;
      InsertMenuItemW(hMenuSub, 0, TRUE, &mii);
    }
    SetMenuDefaultItem(hMenuSub, 0, TRUE);
  }

  //
  ~TaskTray()
  {
    // destroy menu
    DestroyMenu(m_hMenuTaskTray);
    
    // delete tasktray icon
    Shell_NotifyIconW(NIM_DELETE, &m_ni);
    DestroyIcon(m_ni.hIcon);
  }

  // message loop
  WPARAM messageLoop()
  {
    MSG msg;
    while (0 < GetMessageW(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
    return msg.wParam;
  }  
};

//
class CMutex : private noncopyable {
  HANDLE m_handle;
public:
  CMutex(const wchar_t *i_filename)
    : m_handle(CreateMutexW(NULL, TRUE, i_filename)) {
  }
  
  ~CMutex() {
    close();
  }

  void close() {
    if (m_handle) {
      CloseHandle(m_handle);
    }
    m_handle = NULL;
  }

  HANDLE getHandle() const { return m_handle; }
};

//
class CRegistoryEnvironment : private noncopyable {
  struct Var {
    const char *A;
    const wchar_t *W;
  };
  
  typedef std::list<Var> Vars;
  Vars m_vars;
  Vars m_broadcastVars;
  
public:
  ~CRegistoryEnvironment() {
    broadcastUnsetenv();
  }
  
  void add(const char *i_varA, const wchar_t *i_varW) {
    Var var = { i_varA, i_varW };
    m_vars.push_back(var);
  }

  void broadcastSetenv() {
    setenv();
    broadcast();
  }

  void broadcastUnsetenv() {
    if (m_broadcastVars.size()) {
      unsetenv();
      broadcast();
    }
  }

private:
  void setenv() {
    unsetenv();
    for (Vars::iterator itr = m_vars.begin(); itr != m_vars.end(); ++ itr) {
      if (const char *value = getenv(itr->A)) {
	verbose("$ export %s=%s\n", itr->A, value);
	writeRegistry(HKEY_CURRENT_USER, L"Environment", itr->W,
		      to_wstring(value));
	m_broadcastVars.push_back(*itr);
      }
    }
  }
  
  void unsetenv() {
    for (Vars::iterator itr = m_broadcastVars.begin();
	 itr != m_broadcastVars.end(); ++ itr) {
      verbose("$ unset %s\n", itr->A);
      removeRegistry(HKEY_CURRENT_USER, L"Environment", itr->W);
    }
    m_broadcastVars.clear();
  }
  
  void broadcast() {
    DWORD_PTR returnValue;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
			reinterpret_cast<LPARAM>(L"Environment"),
			SMTO_ABORTIFHUNG,
			5000, &returnValue);
    verbose("(broadcast)\n");
  }
};

// main
int main(int i_argc, char **i_argv)
{
  setlocale(LC_ALL, "");
  
  g_hInst = GetModuleHandle(NULL);

  bool option_ok = checkOptions(i_argc, i_argv);
  verbose("$ ");
  for (int i = 0; i < i_argc; ++ i) {
    verbose("%s ", i_argv[i]);
  }
  verbose("\n");
  if (!option_ok) {
    return 1;
  }

  // is another running ?
  static const wchar_t *GUID = L"{6A1AFE71-F534-47a2-AB94-2ED3758C18AE}";
  CMutex mutex(GUID);
  if (mutex.getHandle() == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
    messagebox(L"Another win-ssh-agent.exe is runnnig!");
    return 1;
  }

  // run ssh-agent
  if (!g_option_no_ssh_agent) {
    if (run_ssh_agent() != 0) {
      return 1;
    }
  }

  // environment
  CRegistoryEnvironment registoryEnvironment;
#define addAW(a) add(a, L##a)
  
  if (getenv("DISPLAY")) {
    verbose("$ echo $DISPLAY\n");
    verbose("%s\n", getenv("DISPLAY"));
  } else if (g_option_DISPLAY) {
    setenv("DISPLAY", ":0", 1);
    registoryEnvironment.addAW("DISPLAY");
  }
  
  registoryEnvironment.addAW("SSH_AUTH_SOCK");
  registoryEnvironment.addAW("SSH_AGENT_PID");
  
  setenv("SSH_ASKPASS", getAskpassPath().c_str(), 1);
  registoryEnvironment.addAW("SSH_ASKPASS");

  registoryEnvironment.broadcastSetenv();

  // run
  if (g_option_defaultIdentityFile) {
    int status = run_ssh_add(g_option_defaultIdentityFile);
    if (g_option_execArgs && (status == 0)) {
      run_program(g_option_execArgs);
    }
  }
  verbose("\n(message loop)\n");
  TaskTray().messageLoop();

  // cleanup
  verbose("\n(cleanup)\n");
  if (!g_option_no_ssh_agent) {
    kill_ssh_agent();
  }
  
  return 0;
}
