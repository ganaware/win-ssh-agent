#ifndef _MISC_H
#  define _MISC_H

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
extern bool writeRegistry(HKEY i_root, const std::string &i_path,
			  const std::string &i_name,
			  const std::string &i_value);

// remove registry
extern bool removeRegistry(HKEY i_root, const std::string &i_path,
			   const std::string &i_name);

// get the path of win-ssh-askpass.exe
extern std::string getSelfPath();

#endif // !_MISC_H
