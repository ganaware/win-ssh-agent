#include "misc.h"
#include <sys/cygwin.h>

// instance
HINSTANCE g_hInst = NULL;

// load small icon resource
HICON loadSmallIcon(UINT i_id)
{
  return reinterpret_cast<HICON>(
    LoadImage(g_hInst, MAKEINTRESOURCE(i_id), IMAGE_ICON, 16, 16, 0));
}

// load big icon resource
HICON loadBigIcon(UINT i_id)
{
  return reinterpret_cast<HICON>(
    LoadImage(g_hInst, MAKEINTRESOURCE(i_id), IMAGE_ICON, 32, 32, 0));
}

/** set small icon to the specified window.
    @return handle of previous icon or NULL */
HICON setSmallIcon(HWND i_hwnd, UINT i_id)
{
  HICON hicon = (i_id == static_cast<UINT>(-1)) ? NULL : loadSmallIcon(i_id);
  return reinterpret_cast<HICON>(
    SendMessage(i_hwnd, WM_SETICON, static_cast<WPARAM>(ICON_SMALL),
		reinterpret_cast<LPARAM>(hicon)));
}

/** set big icon to the specified window.
    @return handle of previous icon or NULL */
HICON setBigIcon(HWND i_hwnd, UINT i_id)
{
  HICON hicon = (i_id == static_cast<UINT>(-1)) ? NULL : loadBigIcon(i_id);
  return reinterpret_cast<HICON>(
    SendMessage(i_hwnd, WM_SETICON, static_cast<WPARAM>(ICON_BIG),
		reinterpret_cast<LPARAM>(hicon)));
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
