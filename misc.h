// -*- tab-width: 8; -*-
#ifndef MISC_H_
#  define MISC_H_

#  include <windows.h>
#  include <string>

// get number of array elements
#  define NUMBER_OF(i_array) (sizeof(i_array) / sizeof((i_array)[0]))

// instance
extern HINSTANCE g_hInst;

// load small icon resource
extern HICON loadSmallIcon(UINT i_id);

// load big icon resource
extern HICON loadBigIcon(UINT i_id);

/** set small icon to the specified window.
    @return handle of previous icon or NULL */
extern HICON setSmallIcon(HWND i_hwnd, UINT i_id);

/** set big icon to the specified window.
    @return handle of previous icon or NULL */
extern HICON setBigIcon(HWND i_hwnd, UINT i_id);

// write string to registry
extern bool writeRegistry(HKEY i_root, const std::wstring &i_path,
			  const std::wstring &i_name,
			  const std::wstring &i_value);

// remove registry
extern bool removeRegistry(HKEY i_root, const std::wstring &i_path,
			   const std::wstring &i_name);

// get the path of win-ssh-askpass.exe
extern std::string getSelfPath();

// convert path
bool conv_path_win_to_posix(std::string *o_dst, const std::wstring &i_src);
std::string conv_path_win_to_posix(const std::wstring &i_src);
bool conv_path_posix_to_win(std::wstring *o_dst, const std::string &i_src);
std::wstring conv_path_posix_to_win(const std::string &i_src);

// convert string to wstring
bool s_to_ws(std::wstring *o_dst, const std::string &i_src);
std::wstring to_wstring(const std::string &i_src);

#endif // !MISC_H_
