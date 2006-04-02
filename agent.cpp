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

#include "agentrc.h"
#include "misc.h"

#undef DEBUG_STDOUT

// global variables to save options

typedef std::list<const char *> IdentityFiles;

static IdentityFiles g_option_identityFiles; // --identity, --default-identity
static const char *g_option_defaultIdentityFile = ""; // --default-identity
static const char *g_option_bindAddress = NULL;	// -a
static const char *g_option_lifetime = NULL;	// -t
static bool g_option_DISPLAY = true;		// --DISPLAY, --no-DISPLAY
static char **g_option_execArgs = NULL;		// --exec
static bool g_option_hideConsole = false;	// --hide-console
#ifdef DEBUG_STDOUT
static bool g_option_verbose = true;		// always verbose
#else
static bool g_option_verbose = false;		// --verbose
#endif

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
  pid_t childPID = fork();
  setenv("SSH_ASKPASS", getAskpassPath().c_str(), 1);
  verbose("export SSH_ASKPASS=%s\n", getAskpassPath().c_str());

  if (childPID == 0)				// child process
  {
    close(0);
    setenv("DISPLAY", ":0", 0);
    verbose("export DISPLAY=:0\n");
    verbose("exec ssh-add %s\n", i_identityFile ? i_identityFile : "");
    if (i_identityFile && !i_identityFile[0])
      i_identityFile = NULL;
    execlp("ssh-add", "ssh-add", i_identityFile, NULL);
    
    std::string message("Failed to exec ssh-add.exe : \r\n");
    message += sys_errlist[errno];
    MessageBox(NULL, message.c_str(), "win-ssh-agent", MB_OK | MB_ICONSTOP);
    return 1;
  }
  while (true)
  {
    int status;
    int pid = wait3(&status, 0, NULL);
    if (pid == childPID || pid <= 0)
      if (WIFEXITED(status))
      {
	verbose("ssh-add exit(%d)\n", WEXITSTATUS(status));
#if 1
	if (WEXITSTATUS(status) == 2)
	{
	  MessageBox(NULL, "ssh-add exit(2) : \nCould not open a connection to your authentication agent.",
		     "win-ssh-agent", MB_OK | MB_ICONSTOP);
	}
#endif
	return WEXITSTATUS(status);
      }
      else 
	return -1;
  }
}

// http://sourceware.org/ml/cygwin/2006-02/msg00289.html
/* Copy cygwin environment variables to the Windows environment if they're not
 * already there. */
static void setup_win_environ(void)
{
  char **envp = environ;
  char *var, *val;
  char curval[2];
  char *winpathlist;
  char winpath[MAX_PATH+1];

  while (envp && *envp) {
    var = strdup(*envp++);
    val = strchr(var, '=');
    *val++ = '\0';
        
    if (GetEnvironmentVariable(var, curval, 2) == 0
	&& GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
      /* Convert POSIX to Win32 where necessary */
      if (!strcmp(var, "PATH") ||
	  !strcmp(var, "LD_LIBRARY_PATH")) {
	winpathlist = (char *)
	  malloc(cygwin_posix_to_win32_path_list_buf_size(val) + 1);
	if (winpathlist) {
	  cygwin_posix_to_win32_path_list(val, winpathlist);
	  SetEnvironmentVariable(var, winpathlist);
	  free(winpathlist);
	}
      } else if (!strcmp(var, "HOME") ||
		 !strcmp(var, "TMPDIR") ||
		 !strcmp(var, "TMP") ||
		 !strcmp(var, "TEMP")) {
	cygwin_conv_to_win32_path(val, winpath);
	SetEnvironmentVariable(var, winpath);
      } else {
	SetEnvironmentVariable(var, val);
      }
    }

    free(var);
  }
}

// run any program
void run_program(char **i_argv)
{
  char win32_pathname[1024];
  cygwin_conv_to_win32_path(i_argv[0], win32_pathname);
  setup_win_environ();
  
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
    verbose("ShellExecute %s %s\n", win32_pathname, param.c_str());
    ShellExecute(NULL, "open", win32_pathname, param.c_str(),
		 NULL, SW_SHOWDEFAULT);
  }
  else
  {
    verbose("ShellExecute %s\n", win32_pathname);
    ShellExecute(NULL, "open", win32_pathname, NULL, NULL, SW_SHOWDEFAULT);
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
      ;					// ignore
    else if (a == "--DISPLAY")
      g_option_DISPLAY = true;
    else if (a == "--no-DISPLAY")
      g_option_DISPLAY = false;
    else if (a == "--hide-console")
      g_option_hideConsole = true;
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
      {
	g_option_bindAddress = i_argv[++ i];
	i_argv[i] = i_argv[i - 1] = NULL;
      }
      else if (a == "-t")
      {
	g_option_lifetime = i_argv[++ i];
	i_argv[i] = i_argv[i - 1] = NULL;
      }
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
	       "win-ssh-agent", MB_OK | MB_ICONSTOP);
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

// main
int main(int i_argc, char **i_argv)
{
  g_hInst = GetModuleHandle(NULL);
  
#ifdef DEBUG_STDOUT
  printf("> ");
  for (int i = 0; i < i_argc; ++ i)
    printf("%s ", i_argv[i]);
  printf("\n");
  fflush(stdout);
#endif
  
  // 2. Executed by me via ssh-agent
  if (2 <= i_argc && i_argv[1] == std::string("--no-ssh-agent"))
  {
    if (!(getenv("SSH_AGENT_PID") && getenv("SSH_AUTH_SOCK")))
      return 1;

    if (!checkOptions(i_argc, i_argv))
      return 1;

    // is another running ?
    static const char *GUID = "{6A1AFE71-F534-47a2-AB94-2ED3758C18AE}";
    HANDLE mutex = CreateMutex(NULL, TRUE, GUID);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
      MessageBox(NULL, "Another win-ssh-agent.exe is runnnig!",
		 "win-ssh-agent", MB_OK | MB_ICONSTOP);
      return 1;
    }

    // hide console
    HWND hwndConsole = NULL;
    if (g_option_hideConsole)
    {
      char buf[1024];
      GetConsoleTitle(buf, sizeof(buf));
      SetConsoleTitle(GUID);
      for (int i = 0; !hwndConsole && i < 256; ++ i)
      {
	Sleep(100);
        hwndConsole = FindWindowEx(NULL, NULL, NULL, GUID);
      }
      SetConsoleTitle(buf);
      if (hwndConsole)
	ShowWindow(hwndConsole, SW_HIDE);
    }
    
    bool hasDISPLAY = !!getenv("DISPLAY");
    if (hasDISPLAY)
      verbose("DISPLAY=%s\n", getenv("DISPLAY"));
    
    std::string askpassPath(getAskpassPath());
    setenv("SSH_ASKPASS", askpassPath.c_str(), 1);
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AGENT_PID",
		  getenv("SSH_AGENT_PID"));
    verbose("export SSH_AGENT_PID=%s\n", getenv("SSH_AGENT_PID"));
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AUTH_SOCK",
		  getenv("SSH_AUTH_SOCK"));
    verbose("export SSH_AUTH_SOCK=%s\n", getenv("SSH_AUTH_SOCK"));
    writeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_ASKPASS",
		  askpassPath);
    verbose("export SSH_ASKPASS=%s\n", askpassPath.c_str());
    if (!hasDISPLAY && g_option_DISPLAY)
    {
      writeRegistry(HKEY_CURRENT_USER, "Environment", "DISPLAY", ":0");
      verbose("export DISPLAY=:0\n");
    }
      
    DWORD returnValue;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		       (LPARAM)"Environment", SMTO_ABORTIFHUNG,
		       5000, &returnValue);

    if (g_option_defaultIdentityFile)
    {
      int status = run_ssh_add(g_option_defaultIdentityFile);
      if (g_option_execArgs && (status == 0))
      {
	run_program(g_option_execArgs);
      }
    }

    verbose("sleep\n");
    TaskTray().messageLoop();

    if (!hasDISPLAY && g_option_DISPLAY)
    {
      removeRegistry(HKEY_CURRENT_USER, "Environment", "DISPLAY");
      verbose("export DISPLAY=\n");
    }
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_ASKPASS");
    verbose("export SSH_ASKPASS=\n");
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AUTH_SOCK");
    verbose("export SSH_AUTH_SOCK=\n");
    removeRegistry(HKEY_CURRENT_USER, "Environment", "SSH_AGENT_PID");
    verbose("export SSH_AGENT_PID=\n");
    
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		       (LPARAM)"Environment", SMTO_ABORTIFHUNG,
		       5000, &returnValue);

    CloseHandle(mutex);

    if (hwndConsole)
      ShowWindow(hwndConsole, SW_SHOW);
    
    return 0;
  }

  // 1. Executed by user (from Explorer etc.)
  else
  {
    if (!checkOptions(i_argc, i_argv))
      return 1;
    
    const std::string selfPath(getSelfPath());
    char **argv = new char *[i_argc + 3];
    char **argp = argv;
    *argp ++ = "ssh-agent";
    if (g_option_bindAddress)
    {
      *argp ++ = "-a";
      *argp ++ = const_cast<char *>(g_option_bindAddress);
    }
    if (g_option_lifetime)
    {
      *argp ++ = "-t";
      *argp ++ = const_cast<char *>(g_option_lifetime);
    }
    *argp ++ = const_cast<char *>(selfPath.c_str());
    *argp ++ = "--no-ssh-agent";
    for (int i = 1; i <= i_argc; ++ i)
      if (i_argv[i])	// some options are removed in the checkOptions()
	*argp ++ = i_argv[i];
    *argp ++ = NULL;
#ifdef DEBUG_STDOUT
    printf("exec ");
    for (int i = 0; argv[i]; ++ i)
      printf("%s ", argv[i]);
    printf("\n");
    fflush(stdout);
#endif
    execvp("ssh-agent", argv);
    
    std::string message("Failed to exec ssh-agent.exe : \n");
    message += sys_errlist[errno];
    MessageBox(NULL, message.c_str(), "win-ssh-agent", MB_OK | MB_ICONSTOP);
    delete argv;
    return 1;
  }
}
