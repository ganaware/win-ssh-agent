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
bool writeRegistry(HKEY i_root, const std::wstring &i_path,
		   const std::wstring &i_name, const std::wstring &i_value)
{
  HKEY hkey;
  DWORD disposition;
  if (ERROR_SUCCESS !=
      RegCreateKeyExW(i_root, i_path.c_str(), 0, NULL,
		     REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, NULL, &hkey, &disposition))
    return false;
  RegSetValueExW(hkey, i_name.c_str(), 0, REG_SZ,
		reinterpret_cast<BYTE *>(const_cast<wchar_t *>(i_value.c_str())),
		(i_value.size() + 1) * sizeof(std::wstring::value_type));
  RegCloseKey(hkey);
  return true;
}

// remove registry
bool removeRegistry(HKEY i_root, const std::wstring &i_path,
		    const std::wstring &i_name)
{
  if (i_name.empty())
    return RegDeleteKeyW(i_root, i_path.c_str()) == ERROR_SUCCESS;
  HKEY hkey;
  if (ERROR_SUCCESS !=
      RegOpenKeyExW(i_root, i_path.c_str(), 0, KEY_SET_VALUE, &hkey))
    return false;
  LONG r = RegDeleteValueW(hkey, i_name.c_str());
  RegCloseKey(hkey);
  return r == ERROR_SUCCESS;
}

// get the path of win-ssh-askpass.exe
std::string getSelfPath()
{
  static const size_t N = 1024;
  wchar_t win32_pathname[N];
  if (GetModuleFileNameW(NULL, win32_pathname, N) == 0) {
	abort();
  }
  win32_pathname[N - 1] = L'\0';
  // NULL terminate for XP, 2000
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx
  return conv_path_win_to_posix(win32_pathname);
}

// convert path
bool conv_path_win_to_posix(std::string *o_dst, const std::wstring &i_src)
{
  ssize_t bytes = cygwin_conv_path(CCP_WIN_W_TO_POSIX, i_src.c_str(), NULL, 0);
  if (bytes <= 0) {
	return false;
  }
  o_dst->resize(bytes - 1);
  cygwin_conv_path(CCP_WIN_W_TO_POSIX, i_src.c_str(),
				   const_cast<char *>(o_dst->c_str()), bytes);
  return true;
}

std::string conv_path_win_to_posix(const std::wstring &i_src)
{
  std::string buf;
  if (!conv_path_win_to_posix(&buf, i_src)) {
	abort();
  }
  return buf;
}

// convert path
bool conv_path_posix_to_win(std::wstring *o_dst, const std::string &i_src)
{
  ssize_t bytes = cygwin_conv_path(CCP_POSIX_TO_WIN_W, i_src.c_str(), NULL, 0);
  if (bytes <= 0) {
	return false;
  }
  o_dst->resize(bytes / sizeof(std::wstring::value_type) - 1);
  cygwin_conv_path(CCP_POSIX_TO_WIN_W, i_src.c_str(),
				   const_cast<wchar_t *>(o_dst->c_str()), bytes);
  return true;
}

std::wstring conv_path_posix_to_win(const std::string &i_src)
{
  std::wstring buf;
  if (!conv_path_posix_to_win(&buf, i_src)) {
	abort();
  }
  return buf;
}

// convert string to wstring
bool s_to_ws(std::wstring *o_dst, const std::string &i_src)
{
  if (i_src.size() == 0) {
	o_dst->clear();
	return true;
  }
  size_t size = mbstowcs(NULL, i_src.c_str(), 0);
  if (size == static_cast<size_t>(-1)) {
	return false;
  }
  o_dst->resize(size);
  mbstowcs(const_cast<wchar_t *>(o_dst->c_str()), i_src.c_str(),
		   o_dst->size() + 1);
  return true;
}

std::wstring to_wstring(const std::string &i_src)
{
  std::wstring buf;
  if (!s_to_ws(&buf, i_src)) {
	abort();
  }
  return buf;
}
