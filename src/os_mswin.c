/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_mswin.c
 *
 * Routines common to both Win16 and Win32.
 */

#ifdef WIN16
# ifdef __BORLANDC__
#  pragma warn -par
#  pragma warn -ucp
#  pragma warn -use
#  pragma warn -aus
# endif
#endif

#include <io.h>
#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#ifdef WIN16
# define SHORT_FNAME		/* always 8.3 file name */
# include <dos.h>
# include <string.h>
#endif
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <process.h>

#undef chdir
#ifdef __GNUC__
# ifndef __MINGW32__
#  include <dirent.h>
# endif
#else
# include <direct.h>
#endif

#if defined(FEAT_TITLE) && !defined(FEAT_GUI_W32)
# include <shellapi.h>
#endif

#ifdef FEAT_PRINTER
# include <dlgs.h>
# ifdef WIN3264
#  include <winspool.h>
# endif
# include <commdlg.h>
#endif

#ifdef __MINGW32__
# ifndef FROM_LEFT_1ST_BUTTON_PRESSED
#  define FROM_LEFT_1ST_BUTTON_PRESSED    0x0001
# endif
# ifndef RIGHTMOST_BUTTON_PRESSED
#  define RIGHTMOST_BUTTON_PRESSED        0x0002
# endif
# ifndef FROM_LEFT_2ND_BUTTON_PRESSED
#  define FROM_LEFT_2ND_BUTTON_PRESSED    0x0004
# endif
# ifndef FROM_LEFT_3RD_BUTTON_PRESSED
#  define FROM_LEFT_3RD_BUTTON_PRESSED    0x0008
# endif
# ifndef FROM_LEFT_4TH_BUTTON_PRESSED
#  define FROM_LEFT_4TH_BUTTON_PRESSED    0x0010
# endif

/*
 * EventFlags
 */
# ifndef MOUSE_MOVED
#  define MOUSE_MOVED   0x0001
# endif
# ifndef DOUBLE_CLICK
#  define DOUBLE_CLICK  0x0002
# endif
#endif

/* Record all output and all keyboard & mouse input */
/* #define MCH_WRITE_DUMP */

#ifdef MCH_WRITE_DUMP
FILE* fdDump = NULL;
#endif

#ifdef WIN3264
extern DWORD g_PlatformId;
#endif
#ifndef FEAT_GUI_MSWIN
extern char g_szOrigTitle[];
#endif

/*
 * When generating prototypes for Win32 on Unix, these lines make the syntax
 * errors disappear.  They do not need to be correct.
 */
#ifdef PROTO
# define HANDLE int
# define SMALL_RECT int
# define COORD int
# define SHORT int
# define WORD int
# define DWORD int
# define BOOL int
# define UINT int
# define CALLBACK
# define LRESULT int
# define LPSTR int
# define LPTSTR int
# define WPARAM int
# define LPARAM int
# define KEY_EVENT_RECORD int
# define MOUSE_EVENT_RECORD int
# define WINAPI
# define CONSOLE_CURSOR_INFO int
# define LPCSTR char_u *
# define WINBASEAPI
# define INPUT_RECORD int
# define SECURITY_INFORMATION int
# define PSECURITY_DESCRIPTOR int
# define VOID void
# define HWND int
# define PSID int
# define PACL int
# define HICON int
# define HFONT int
# define PRINTDLG int
# define TEXTMETRIC int
# define COLORREF int
typedef int HDC;
typedef int LOGFONT;
# define ENUMLOGFONT int
# define NEWTEXTMETRIC int
#endif


#if defined(__GNUC__) && !defined(PROTO)
# ifndef __MINGW32__
int _stricoll(char *a, char *b)
{
    // the ANSI-ish correct way is to use strxfrm():
    char a_buff[512], b_buff[512];  // file names, so this is enough on Win32
    strxfrm(a_buff, a, 512);
    strxfrm(b_buff, b, 512);
    return strcoll(a_buff, b_buff);
}

char * _fullpath(char *buf, char *fname, int len)
{
    LPTSTR toss;

    return (char *)GetFullPathName(fname, len, buf, &toss);
}
# endif

int _chdrive(int drive)
{
    char temp [3] = "-:";
    temp[0] = drive + 'A' - 1;
    return !SetCurrentDirectory(temp);
}
#else
#ifdef __BORLANDC__
/* being a more ANSI compliant compiler, BorlandC doesn't define _stricoll:
 * but it does in BC 5.02! */
#if __BORLANDC__ < 0x502
int _stricoll(char *a, char *b)
{
#if 1
    // this is fast but not correct:
    return stricmp(a,b);
#else
    // the ANSI-ish correct way is to use strxfrm():
    char a_buff[512], b_buff[512];  // file names, so this is enough on Win32
    strxfrm(a_buff, a, 512);
    strxfrm(b_buff, b, 512);
    return strcoll(a_buff, b_buff);
#endif
}
#endif
#endif
#endif


#if defined(FEAT_GUI_MSWIN) || defined(PROTO)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    void
mch_exit(int r)
{
    display_errors();

    ml_close_all(TRUE);		/* remove all memfiles */

# ifdef FEAT_OLE
    UninitOLE();
# endif
#ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
#endif

    if (gui.in_use)
	gui_exit(r);
    exit(r);
}

#endif /* FEAT_GUI_MSWIN */


/*
 * Init the tables for toupper() and tolower().
 */
    void
mch_early_init(void)
{
    int		i;

#ifdef WIN3264
    PlatformId();
#endif

    /* Init the tables for toupper() and tolower() */
    for (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
#ifdef WIN3264
    CharUpperBuff(toupper_tab, 256);
    CharLowerBuff(tolower_tab, 256);
#else
    AnsiUpperBuff(toupper_tab, 256);
    AnsiLowerBuff(tolower_tab, 256);
#endif
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty()
{
#ifdef FEAT_GUI_MSWIN
    return OK;	    /* GUI always has a tty */
#else
    if (isatty(read_cmd_fd))
	return TRUE;
    return FALSE;
#endif
}

#ifdef FEAT_TITLE
/*
 * mch_settitle(): set titlebar of our window
 */
    void
mch_settitle(
    char_u *title,
    char_u *icon)
{
# ifdef FEAT_GUI_MSWIN
    gui_mch_settitle(title, icon);
# else
    if (title != NULL)
	SetConsoleTitle(title);
# endif
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  1: Just restore title
 *  2: Just restore icon (which we don't have)
 *  3: Restore title and icon (which we don't have)
 */
    void
mch_restore_title(
    int which)
{
#ifndef FEAT_GUI_MSWIN
    mch_settitle((which & 1) ? g_szOrigTitle : NULL, NULL);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    int
mch_can_restore_title()
{
    return TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    int
mch_can_restore_icon()
{
    return FALSE;
}
#endif /* FEAT_TITLE */


/*
 * Get absolute file name into buffer 'buf' of length 'len' bytes,
 * turning all '/'s into '\\'s and getting the correct case of each
 * component of the file name.  Append a backslash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    int
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    int		len,
    int		force)
{
    int		nResult = FAIL;

#ifdef __BORLANDC__
    if (*fname == NUL) /* Borland behaves badly here - make it consistent */
	nResult = mch_dirname(buf, len);
    else
#endif
	if (_fullpath(buf, fname, len - 1) == NULL)
    {
	STRNCPY(buf, fname, len);   /* failed, use the relative path name */
	buf[len - 1] = NUL;
#ifndef USE_FNAME_CASE
	slash_adjust(buf);
#endif
    }
    else
	nResult = OK;

#ifdef USE_FNAME_CASE
    fname_case(buf);
#endif

    return nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    int
mch_isFullName(char_u *fname)
{
    char szName[_MAX_PATH + 1];

    /* A name like "d:/foo" and "//server/share" is absolute */
    if ((fname[0] && fname[1] == ':' && (fname[2] == '/' || fname[2] == '\\'))
	    || (fname[0] == fname[1] && (fname[0] == '/' || fname[0] == '\\')))
	return TRUE;

    /* A name that can't be made absolute probably isn't absolute. */
    if (mch_FullName(fname, szName, _MAX_PATH, FALSE) == FAIL)
	return FALSE;

    return pathcmp(fname, szName) == 0;
}

/*
 * Replace all slashes by backslashes.
 * This used to be the other way around, but MS-DOS sometimes has problems
 * with slashes (e.g. in a command name).  We can't have mixed slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use a file name should try to avoid the need to type a
 * backslash twice.
 * When 'shellslash' set do it the other way around.
 */
    void
slash_adjust(p)
    char_u  *p;
{
    if (p != NULL)
	while (*p)
	{
	    if (*p == psepcN)
		*p = psepc;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		p += (*mb_ptr2len_check)(p);
	    else
#endif
		++p;
	}
}


/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 */
    int
vim_stat(const char *name, struct stat *stp)
{
    char	buf[_MAX_PATH + 1];
    char	*p;

    STRNCPY(buf, name, _MAX_PATH);
    buf[_MAX_PATH] = NUL;
    p = buf + strlen(buf);
    if (p > buf)
	--p;
    if (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
	*p = NUL;
    return stat(buf, stp);
}

#if defined(FEAT_GUI_MSWIN) || defined(PROTO)
    void
mch_settmode(int tmode)
{
    /* nothing to do */
}

    int
mch_get_shellsize(void)
{
    /* never used */
    return OK;
}

    void
mch_set_shellsize(void)
{
    /* never used */
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    /* never used */
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    void
mch_suspend()
{
    suspend_shell();
}

#if defined(USE_MCH_ERRMSG) || defined(PROTO)

#ifdef display_errors
# undef display_errors
#endif

/*
 * Display the saved error message(s).
 */
    void
display_errors()
{
    char *p;

    if (error_ga.ga_data != NULL)
    {
	/* avoid putting up a message box with blanks only */
	for (p = (char *)error_ga.ga_data; *p; ++p)
	    if (!isspace(*p))
	    {
		/* Truncate a very long message, it will go off-screen. */
		if (STRLEN(p) > 2000)
		    STRCPY(p + 2000 - 14, "...(truncated)");
#ifdef WIN3264
		MessageBox(NULL, p, "Vim", MB_TASKMODAL|MB_SETFOREGROUND);
#else
		MessageBox(NULL, p, "Vim", MB_TASKMODAL);
#endif
		break;
	    }
	ga_clear(&error_ga);
    }
}
#endif


/*
 * Does `s' contain a wildcard?
 */
    int
mch_has_wildcard(char_u *p)
{
    for ( ; *p; ++p)
    {
	if (vim_strchr((char_u *)
#  ifdef VIM_BACKTICK
				    "?*$[`"
#  else
				    "?*$["
#  endif
                                                , *p) != NULL
		|| (*p == '~' && p[1] != NUL))
	    return TRUE;
    }
    return FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    int
mch_chdir(char *path)
{
    if (path[0] == NUL)		/* just checking... */
	return -1;

    if (isalpha(path[0]) && path[1] == ':')	/* has a drive name */
    {
	if (_chdrive(TO_LOWER(path[0]) - 'a' + 1) != 0)
	    return -1;		/* invalid drive name */
	path += 2;
    }

    if (*path == NUL)		/* drive name only */
	return 0;

    return chdir(path);	       /* let the normal chdir() do the rest */
}


/*
 * Switching off termcap mode is only allowed when Columns is 80, otherwise a
 * crash may result.  It's always allowed on NT or when running the GUI.
 */
    int
can_end_termcap_mode(
    int give_msg)
{
#ifdef FEAT_GUI_MSWIN
    return TRUE;	/* GUI starts a new console anyway */
#else
    if (g_PlatformId == VER_PLATFORM_WIN32_NT || Columns == 80)
	return TRUE;
    if (give_msg)
	msg(_("'columns' is not 80, cannot execute external commands"));
    return FALSE;
#endif
}

#ifdef FEAT_GUI_MSWIN
/*
 * return non-zero if a character is available
 */
    int
mch_char_avail()
{
    /* never used */
    return TRUE;
}
#endif


/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(
    char_u *arg)
{
    EMSG(_("E359: Screen mode setting not supported"));
    return FAIL;
}


#ifdef FEAT_EVAL
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
#ifdef WIN3264
typedef LPTSTR (*MYSTRPROCSTR)(LPTSTR);
typedef LPTSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPTSTR);
typedef int (*MYINTPROCINT)(int);
#else
typedef LPSTR (*MYSTRPROCSTR)(LPSTR);
typedef LPSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPSTR);
typedef int (*MYINTPROCINT)(int);
#endif

    int
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	/* NULL when using a argint */
    int		argint,
    char_u	**string_result,/* NULL when using number_result */
    int		*number_result)
{
    HINSTANCE		hinstLib;
    MYSTRPROCSTR	ProcAdd;
    MYINTPROCSTR	ProcAddI;
    char_u		*retval_str = NULL;
    int			retval_int = 0;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = LoadLibrary(libname);

    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL)
    {
	if (argstring != NULL)
	{
	    /* Call with string argument */
	    ProcAdd = (MYSTRPROCSTR) GetProcAddress(hinstLib, funcname);
	    if ((fRunTimeLinkSuccess = (ProcAdd != NULL)) != 0)
	    {
		if (string_result == NULL)
		    retval_int = ((MYSTRPROCINT)ProcAdd)(argstring);
		else
		    retval_str = (ProcAdd)(argstring);
	    }
	}
	else
	{
	    /* Call with number argument */
	    ProcAddI = (MYINTPROCSTR) GetProcAddress(hinstLib, funcname);
	    if ((fRunTimeLinkSuccess = (ProcAddI != NULL)) != 0)
	    {
		if (string_result == NULL)
		    retval_int = ((MYINTPROCINT)ProcAddI)(argint);
		else
		    retval_str = (ProcAddI)(argint);
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	if (string_result == NULL)
	    *number_result = retval_int;
	else if (retval_str != NULL
		&& retval_str != (char_u *)1
		&& retval_str != (char_u *)-1
		&& !IsBadStringPtr(retval_str, INT_MAX))
	    *string_result = vim_strsave(retval_str);

	// Free the DLL module.
	(void)FreeLibrary(hinstLib);
    }

    if (!fRunTimeLinkSuccess)
    {
	EMSG2(_("E364: Library call failed for \"%s()\""), funcname);
	return FAIL;
    }

    return OK;
}
#endif

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    int
clip_mch_own_selection(VimClipboard *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    return FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    void
clip_mch_lose_selection(VimClipboard *cbd)
{
    /* Nothing needs to be done here */
}

/*
 * Get the current selection and put it in the clipboard register.
 *
 * NOTE: Must use GlobalLock/Unlock here to ensure Win32s compatibility.
 * On NT/W95 the clipboard data is a fixed global memory object and
 * so its handle = its pointer.
 * On Win32s, however, co-operation with the Win16 system means that
 * the clipboard data is moveable and its handle is not a pointer at all,
 * so we can't just cast the return value of GetClipboardData to (char_u*).
 * <VN>
 */
    void
clip_mch_request_selection(VimClipboard *cbd)
{
    int		type = MCHAR;
    HGLOBAL	hMem;
    char_u	*str = NULL;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    if (OpenClipboard(NULL))
    {
	/* Check for vim's own clipboard format first */
	if ((hMem = GetClipboardData(cbd->format)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		switch (*str++)
		{
		    default:
		    case 'L':	type = MLINE;	break;
		    case 'C':	type = MCHAR;	break;
		    case 'B':	type = MBLOCK;	break;
		}
	}
	/* Otherwise, check for the normal text format */
	else if ((hMem = GetClipboardData(CF_TEXT)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		type = (vim_strchr((char*) str, '\r') != NULL) ? MLINE : MCHAR;
	}

	if (hMem != NULL && str != NULL)
	{
	    /* successful lock - must unlock when finished */
	    if (*str != NUL)
	    {
		LPCSTR		psz = (LPCSTR)str;
		char_u		*temp_clipboard;
		char_u		*pszTemp;
		const char	*pszNL;
		int		len;

		temp_clipboard = (char_u *)lalloc((long_u)STRLEN(psz) + 1,
									TRUE);
		if (temp_clipboard != NULL)
		{
		    /* Translate <CR><NL> into <NL>. */
		    pszTemp = temp_clipboard;
		    while (*psz != NUL)
		    {
			pszNL = psz;
			for (;;)
			{
			    pszNL = strchr(pszNL, '\r');
			    if (pszNL == NULL || pszNL[1] == '\n')
				break;
			    ++pszNL;
			}
			len = (int)((pszNL != NULL) ? pszNL - psz : STRLEN(psz));
			STRNCPY(pszTemp, psz, len);
			pszTemp += len;
			if (pszNL != NULL)
			    *pszTemp++ = '\n';
			psz += len + ((pszNL != NULL) ? 2 : 0);
		    }
		    *pszTemp = NUL;
		    clip_yank_selection(type, temp_clipboard,
				(long)(pszTemp - temp_clipboard), cbd);
		    vim_free(temp_clipboard);
		}
	    }
	    /* unlock the global object */
	    (void)GlobalUnlock(hMem);
	}
	CloseClipboard();
    }
}

/*
 * Send the current selection to the clipboard.
 */
    void
clip_mch_set_selection(VimClipboard *cbd)
{
    char_u	*str = NULL;
    long_u	cch;
    int		type;
    HGLOBAL	hMem = NULL;
    HGLOBAL	hMemVim = NULL;
    LPSTR	lpszMem = NULL;
    LPSTR	lpszMemVim = NULL;

    /* If the '*' register isn't already filled in, fill it in now */
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &cch, cbd);

    if (type < 0)
	return;

    if ((hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, cch+1)) != NULL
	&& (lpszMem = (LPSTR)GlobalLock(hMem)) != NULL
	&& (hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, cch+2)) != NULL
	&& (lpszMemVim = (LPSTR)GlobalLock(hMemVim)) != NULL)
    {
	switch (type)
	{
	    default:
	    case MLINE:	    *lpszMemVim++ = 'L';    break;
	    case MCHAR:	    *lpszMemVim++ = 'C';    break;
	    case MBLOCK:    *lpszMemVim++ = 'B';    break;
	}

	STRNCPY(lpszMem, str, cch);
	lpszMem[cch] = NUL;

	STRNCPY(lpszMemVim, str, cch);
	lpszMemVim[cch] = NUL;

	/*
	 * Don't pass GetActiveWindow() as an argument to OpenClipboard()
	 * because then we can't paste back into the same window for some
	 * reason - webb.
	 */
	if (OpenClipboard(NULL))
	{
	    if (EmptyClipboard())
	    {
		SetClipboardData(cbd->format, hMemVim);
		SetClipboardData(CF_TEXT, hMem);
	    }
	    CloseClipboard();
	}
    }
    if (lpszMem != NULL)
	GlobalUnlock(hMem);
    if (lpszMemVim != NULL)
	GlobalUnlock(hMemVim);

    vim_free(str);
}

#endif /* FEAT_CLIPBOARD */


/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    void
DumpPutS(
    const char *psz)
{
# ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fputs(psz, fdDump);
	if (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
# endif
}

#ifdef _DEBUG

void __cdecl
Trace(
    char *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list args;

    va_start(args, pszFormat);
    vsprintf(szBuff, pszFormat, args);
    va_end(args);

    OutputDebugString(szBuff);
}

#endif //_DEBUG

#ifdef FEAT_PRINTER
# ifdef WIN16
#  define TEXT(a) a
# endif
/*=================================================================
 * Win32 printer stuff
 */

static HFONT		prt_font_handles[2][2][2];
static PRINTDLG		prt_dlg;
static const int	boldface[2] = {FW_REGULAR, FW_BOLD};
static TEXTMETRIC	prt_tm;
static int		prt_line_height;
static int		prt_number_width;
static int		prt_left_margin;
static int		prt_right_margin;
static int		prt_top_margin;
static char_u		szAppName[] = TEXT("VIM");
static HWND		hDlgPrint;
static int		*bUserAbort = NULL;

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    static COLORREF
swap_me(COLORREF colorref)
{
    int  temp;
    char *ptr = (char *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    return colorref;
}

    static BOOL CALLBACK
PrintDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_INITDIALOG:
	    SetWindowText(hDlg, szAppName);
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
	    return TRUE;

	case WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = NULL;
	    return TRUE;
    }
    return FALSE;
}

    static BOOL CALLBACK
AbortProc(HDC hdcPrn, int iCode)
{
    MSG msg;

    while (!*bUserAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
	if (!hDlgPrint || !IsDialogMessage(hDlgPrint, &msg))
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
    return !*bUserAbort;
}

#ifndef FEAT_GUI
/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */

    HWND
GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.

    char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated WindowTitle.
    char pszOldWindowTitle[MY_BUFSIZE]; // Contains original WindowTitle.
    HANDLE hwndFound;

    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    wsprintf(pszNewWindowTitle, "%s/%d/%d",
	    pszOldWindowTitle,
	    GetTickCount(),
	    GetCurrentProcessId());

    SetConsoleTitle(pszNewWindowTitle);

    Sleep(40);

    hwndFound = FindWindow(NULL, pszNewWindowTitle);

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}


    static UINT CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT uiMsg,	// message identifier
	WPARAM wParam,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLG	*pPD;

    if (uiMsg == WM_INITDIALOG)
    {
	// Get the owner window and dialog box rectangles.
	if ((hwndOwner = GetParent(hDlg)) == NULL)
	    hwndOwner = GetDesktopWindow();

	GetWindowRect(hwndOwner, &rcOwner);
	GetWindowRect(hDlg, &rcDlg);
	CopyRect(&rc, &rcOwner);

	// Offset the owner and dialog box rectangles so that
	// right and bottom values represent the width and
	// height, and then offset the owner again to discard
	// space taken up by the dialog box.

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	// The new position is the sum of half the remaining
	// space and the owner's original position.

	SetWindowPos(hDlg,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,		// ignores size arguments
		SWP_NOSIZE);

	/*  tackle the printdlg copiesctrl problem */
	pPD = (PRINTDLG *)lParam;
	pPD->nCopies = (WORD)pPD->lCustData;
	SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
	/*  Bring the window to top */
	BringWindowToTop(GetParent(hDlg));
	SetForegroundWindow(hDlg);
    }

    return FALSE;
}
#endif

    void
mch_print_cleanup(void)
{
    int pifItalic;
    int pifBold;
    int pifUnderline;

    for (pifBold = 0; pifBold <= 1; pifBold++)
	for (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    for (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    if (prt_dlg.hDC != NULL)
	DeleteDC(prt_dlg.hDC);
    if (!*bUserAbort)
    {
	EnableWindow(prt_dlg.hwndOwner, TRUE);
	DestroyWindow(hDlgPrint);
    }
}

#ifdef FEAT_GUI
extern HWND s_hwnd;
#endif

    static int
to_device_units(int idx, int dpi, int physsize, int offset, int def_number)
{
    int		ret;
    int		c;
    int		nr;

    if (printer_opts[idx].present)
    {
	c = TO_LOWER(printer_opts[idx].string[0]);
	nr = printer_opts[idx].number;
    }
    else
    {
	c = 'p';
	nr = def_number;
    }
    if (c == 'i')
	ret = (nr * dpi);
    else if (c == 'm')
	ret = (nr * 10 * dpi) / 254;
    else
	ret = (physsize * nr) / 100;

    if (ret < offset)
	return 0;
    else
	return ret - offset;
}

    static int
mch_print_get_cpl(void)
{
    int		hr;
    int		phyw;
    int		dvoff;
    int		rev_offset;
    int		dpi;
#ifdef WIN16
    POINT	pagesize;
#endif

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_line_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
#ifdef WIN16
    Escape(prt_dlg.hDC, GETPHYSPAGESIZE, NULL, NULL, &pagesize);
    phyw    = pagesize.x;
    Escape(prt_dlg.hDC, GETPRINTINGOFFSET, NULL, NULL, &pagesize);
    dvoff   = pagesize.x;
#else
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
#endif
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_left_margin = to_device_units(OPT_PRINT_LEFT, dpi, phyw, dvoff, 10);
    if (prt_use_number())
    {
	prt_number_width = PRINT_NUMBER_WIDTH * prt_tm.tmAveCharWidth;
	prt_left_margin += prt_number_width;
    }
    else
	prt_number_width = 0;

    prt_right_margin = hr - to_device_units(OPT_PRINT_RIGHT, dpi, phyw,
							       rev_offset, 5);

    return (prt_right_margin - prt_left_margin) / prt_tm.tmAveCharWidth;
}

    static int
mch_print_get_lpp(void)
{
    int vr;
    int phyw;
    int dvoff;
    int rev_offset;
    int	bottom_margin;
    int	dpi;
#ifdef WIN16
    POINT pagesize;
#endif

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
#ifdef WIN16
    Escape(prt_dlg.hDC, GETPHYSPAGESIZE, NULL, NULL, &pagesize);
    phyw    = pagesize.y;
    Escape(prt_dlg.hDC, GETPRINTINGOFFSET, NULL, NULL, &pagesize);
    dvoff   = pagesize.y;
#else
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
#endif
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_top_margin = to_device_units(OPT_PRINT_TOP, dpi, phyw, dvoff, 5);

    /* adjust top margin if there is a header */
    prt_top_margin += prt_line_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    return (bottom_margin - prt_top_margin) / prt_line_height;
}

    int
mch_print_init(prt_settings_T *psettings, char_u *jobname, int forceit)
{
    static HGLOBAL	stored_dm = NULL;
    static HGLOBAL	stored_devn = NULL;
    static int		stored_nCopies = 1;
    static int		stored_nFlags = 0;

    LOGFONT		fLogFont;
    int			pifItalic;
    int			pifBold;
    int			pifUnderline;

#ifdef WIN3264
    DEVMODE		*mem;
#endif
    int			i;

    bUserAbort = &(psettings->user_abort);
    memset(&prt_dlg, 0, sizeof(PRINTDLG));
    prt_dlg.lStructSize = sizeof(PRINTDLG);
#ifdef FEAT_GUI
    prt_dlg.hwndOwner = s_hwnd;
#else
    prt_dlg.hwndOwner = GetConsoleHwnd();
#endif
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    prt_dlg.hDevMode = stored_dm;
    prt_dlg.hDevNames = stored_devn;
    prt_dlg.lCustData = stored_nCopies; /* work around bug in print dialog */
#ifndef FEAT_GUI
    /*
     * Use hook to prevent console window being sent to back
     */
    prt_dlg.lpfnPrintHook = PrintHookProc;
    prt_dlg.Flags |= PD_ENABLEPRINTHOOK;
#endif
    prt_dlg.Flags |= stored_nFlags;
    /*
     * If bang present, return default printer setup with no dialog
     * never show dialog if we are running over telnet
     */
    if (forceit
#ifndef FEAT_GUI
	    || !term_console
#endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
#ifdef WIN3264
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	if (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(NULL, p_pdev, NULL, NULL);
	else
#endif
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    if (PrintDlg(&prt_dlg) == 0)
		goto init_fail_dlg;
	}
    }
    else if (PrintDlg(&prt_dlg) == 0)
	goto init_fail_dlg;
    hDlgPrint = CreateDialog(GetModuleHandle(NULL), TEXT("PrintDlgBox"),
                               prt_dlg.hwndOwner, PrintDlgProc);
    if (prt_dlg.hDC == NULL)
    {
	EMSG(_("E237: Printer selection failed"));
	mch_print_cleanup();
	return FALSE;
    }

    SetAbortProc(prt_dlg.hDC, AbortProc);

    /*
     * keep the previous driver context
     */
    stored_dm = prt_dlg.hDevMode;
    stored_devn = prt_dlg.hDevNames;
    stored_nFlags = prt_dlg.Flags;
    stored_nCopies = prt_dlg.nCopies;

    /* Not all printer drivers report the support of color (or grey) in the
     * same way.  Let's set has_color if there appears to be some way to print
     * more than B&W. */
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);
#ifdef WIN3264
    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODE *)GlobalLock(prt_dlg.hDevMode);
    if (mem != NULL)
    {
	if (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	if ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	if ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);
#endif

    /*
     * Initialise the font according to 'printfont'
     */
    memset(&fLogFont, 0, sizeof(fLogFont));
    if (!get_logfont(&fLogFont, p_pfn, prt_dlg.hDC))
	return FALSE;

    for (pifBold = 0; pifBold <= 1; pifBold++)
	for (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    for (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lfWeight =  boldface[pifBold];
		fLogFont.lfCharSet = DEFAULT_CHARSET;
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = CreateFontIndirect(&fLogFont);
	    }

    SetBkMode(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_per_line = mch_print_get_cpl();
    psettings->lines_per_page = mch_print_get_lpp();
    psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
							? prt_dlg.nCopies : 1;
    psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
							? 1 : prt_dlg.nCopies;

    if (psettings->n_collated_copies == 0)
	psettings->n_collated_copies = 1;

    if (psettings->n_uncollated_copies == 0)
	psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

    return TRUE;

init_fail_dlg:
    {
	DWORD err = CommDlgExtendedError();

	if (err)
	{
#ifdef WIN16
	    char buf[20];

	    sprintf(buf, "%ld", err);
	    EMSG2(_("E238: Print error: %s"), buf);
#else
	    char_u *buf;

	    /* I suspect FormatMessage() doesn't work for values returned by
	     * CommDlgExtendedError().  What does? */
	    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS,
			  NULL, err, 0, (LPTSTR)(&buf), 0, NULL);
	    EMSG2(_("E238: Print error: %s"), buf == NULL ? _("Unknown") : buf);
	    LocalFree((LPVOID)(buf));
#endif
	}
	else
	    msg_clr_eos(); /* Maybe canceled */

	mch_print_cleanup();
	return FALSE;
    }
}


    int
mch_print_begin(prt_settings_T *psettings)
{
    int			ret;
    static DOCINFO	di;

    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = psettings->jobname;
    ret = StartDoc(prt_dlg.hDC, &di);

    return (ret > 0);
}

    void
mch_print_end(prt_settings_T *psettings)
{
    EndDoc(prt_dlg.hDC);
    if (!*bUserAbort)
    {
	EnableWindow(prt_dlg.hwndOwner, TRUE);
	DestroyWindow(hDlgPrint);
    }
}

    int
mch_print_end_page(void)
{
    return (EndPage(prt_dlg.hDC) > 0);
}

    int
mch_print_begin_page(void)
{
    return (StartPage(prt_dlg.hDC) > 0);
}

    int
mch_print_blank_page(void)
{
    return (mch_print_begin_page() ? (mch_print_end_page()) : FALSE);
}

static int prt_pos_x = 0;
static int prt_pos_y = 0;

    void
mch_print_start_line(margin, page_line)
    int		margin;
    int		page_line;
{
    if (margin)
	prt_pos_x = -prt_number_width;
    else
	prt_pos_x = 0;
    prt_pos_y = page_line * prt_line_height;
}

    int
mch_print_text_out(char_u *p, int len)
{
    SIZE	sz;

    TextOut(prt_dlg.hDC, prt_pos_x + prt_left_margin,
					  prt_pos_y + prt_top_margin, p, len);
#ifdef WIN16
    GetTextExtentPoint(prt_dlg.hDC, p, len, &sz);
#else
    GetTextExtentPoint32(prt_dlg.hDC, p, len, &sz);
#endif
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
#ifndef FEAT_PROPORTIONAL_FONTS
    return (prt_pos_x + prt_left_margin + prt_tm.tmAveCharWidth
				     + prt_tm.tmOverhang > prt_right_margin);
#else
    /* This is wrong when printing spaces for a TAB. */
    if (p[len] == NUL)
	return FALSE;
# ifdef WIN16
    GetTextExtentPoint(prt_dlg.hDC, p + len, 1, &sz);
# else
    GetTextExtentPoint32(prt_dlg.hDC, p + len, 1, &sz);
# endif
    return (prt_pos_x + prt_left_margin + sz.cx > prt_right_margin);
#endif
}

    void
mch_print_set_font(int iBold, int iItalic, int iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    void
mch_print_set_bg(unsigned long bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC, swap_me(bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    if (bgcol == 0xffffff)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    else
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    void
mch_print_set_fg(unsigned long fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC, swap_me(fgcol)));
}

#endif /*FEAT_PRINTER*/

#if defined(FEAT_SHORTCUT) || defined(PROTO)
# include <shlobj.h>

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    char_u *
mch_resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = NULL;
    IPersistFile	*ppf = NULL;
    OLECHAR		wsz[MAX_PATH];
    WIN32_FIND_DATA	ffd; // we get those free of charge
    TCHAR		buf[MAX_PATH]; // could have simply reused 'wsz'...
    char_u		*rfname = NULL;
    int			len;

    /* Check if the file name ends in ".lnk". Avoid calling
     * CoCreateInstance(), it's quite slow. */
    if (fname == NULL)
	return rfname;
    len = STRLEN(fname);
    if (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	return rfname;

    CoInitialize(NULL);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
	    &IID_IShellLink, (void**)&psl);
    if (hr != S_OK)
	goto shortcut_error;

    // Get a pointer to the IPersistFile interface.
    hr = psl->lpVtbl->QueryInterface(
	    psl, &IID_IPersistFile, (void**)&ppf);
    if (hr != S_OK)
	goto shortcut_error;

    // full path string must be in Unicode.
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wsz, MAX_PATH);

    // "load" the name and resove the link
    hr = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
    if (hr != S_OK)
	goto shortcut_error;
#if 0  // This makes Vim wait a long time if the target doesn't exist.
    hr = psl->lpVtbl->Resolve(psl, NULL, SLR_NO_UI);
    if (hr != S_OK)
	goto shortcut_error;
#endif

    // Get the path to the link target.
    ZeroMemory(buf, MAX_PATH);
    hr = psl->lpVtbl->GetPath(psl, buf, MAX_PATH, &ffd, 0);
    if (hr == S_OK && buf[0] != NUL)
	rfname = vim_strsave(buf);

shortcut_error:
    // Release all interface pointers (both belong to the same object)
    if (ppf != NULL)
	ppf->lpVtbl->Release(ppf);
    if (psl != NULL)
	psl->lpVtbl->Release(psl);

    CoUninitialize();
    return rfname;
}
#endif

#if defined(FEAT_CLIENTSERVER) || defined(PROTO)
/*
 * Client-server code for Vim
 *
 * Originally written by Paul Moore
 */

/* In order to handle inter-process messages, we need to have a window. But
 * the functions in this module can be called before the main GUI window is
 * created (and may also be called in the console version, where there is no
 * GUI window at all).
 *
 * So we create a hidden window, and arrange to destroy it on exit.
 */
HWND message_window = 0;

/* We also use a broadcast message to locate servers
 */
static unsigned int vim_broadcast = 0;

/* Communication is via WM_COPYDATA messages. The message type is send in
 * the dwData parameter. Types are defined here.
 */
#define COPYDATA_KEYS	0
#define COPYDATA_EXPR	1
#define COPYDATA_REPLY	2
#define COPYDATA_GETHWND 3
#define COPYDATA_LIST	4
#define BROADCAST_WAKEUP 5

/* This is a structure containing a server HWND and its name
 */
struct server_id
{
    HWND hwnd;
    char_u *name;
};

/* Clean up on exit. This destroys the hidden message window.
 */
    static void
CleanUpMessaging(void)
{
    if (message_window != 0)
    {
	DestroyWindow(message_window);
	message_window = 0;
    }
}

static int save_reply(char_u *id, char_u *reply);

/* The window procedure for the hidden message window.
 * It handles callback messages and notifications from servers.
 * In order to process these messages, it is necessary to run a
 * message loop. Code which may run before the main message loop
 * is started (in the GUI) is careful to pump messages when it needs
 * to. Features which require message delivery during normal use will
 * not work in the console version - this basically means those
 * features which allow Vim to act as a server, rather than a client.
 */
    static LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == vim_broadcast)
    {
	/* wParam is the sender's HWND,
	 * lParam is the notification type to return.
	 * This is simply passed back in the WM_COPYDATA reply.
	 */
	HWND requester = (HWND)wParam;
	DWORD type = (DWORD)lParam;
	COPYDATASTRUCT reply;

	/* If the type is BROADCAST_WAKEUP, we don't need to reply - it's just
	 * a notification */
	if (type == BROADCAST_WAKEUP)
	    return 1;

	/* Don't respond if we don't have a name to reply with */
	if (serverName == NULL)
	    return 0;

	reply.dwData = type;
	reply.cbData = STRLEN(serverName) + 1;
	reply.lpData = serverName;
	SendMessage(requester, WM_COPYDATA, (WPARAM)message_window,
							    (LPARAM)(&reply));

	/* Don't bother if the sender ignores us */
	return 1;
    }
    else if (msg == WM_COPYDATA)
    {
	/* This is a message from another Vim. The dwData member of the
	 * COPYDATASTRUCT determines the type of message:
	 *    0 = A key sequence. We are a server, and a client wants
	 *        these keys adding to the input queue.
	 *    1 = An expression. We are a server, and a client wants us
	 *        to evaluate this expression.
	 *    2 = A reply. We are a client, and a server has sent this
	 *        message in response to a request. (serverreply_send)
	 *    3 = A response to a VimBroadcast message.
	 *        The broadcast requested a HWND for a server name.
	 *        The window user data contains a pointer to a
	 *        struct server_id with the requested name in it.
	 *        The window procedure should fill in the HWND.
	 *    4 = A response to a VimBroadcast message.
	 *        The broadcast requested a list of server names.
	 *        The window user data contains a pointer to a
	 *        growarray. Each server name is added to the growarray,
	 *        separated by newlines.
	 */
	COPYDATASTRUCT *data = (COPYDATASTRUCT*)lParam;
	HWND sender = (HWND)wParam;
	COPYDATASTRUCT reply;
	char_u *cpo_save;
	char_u *res;
	char_u winstr[30];
	garray_T *ga;
	struct server_id *id;

	switch (data->dwData)
	{
	case COPYDATA_KEYS:
	    /* Set 'cpoptions' the way we want it.
	     *    B set - backslashes are *not* treated specially
	     *    k set - keycodes are *not* reverse-engineered
	     *    < unset - <Key> sequenecs *are* interpreted
	     *  last parameter of replace_termcodes() is TRUE so that
	     *  the <lt> sequence is recognised - needed as backslash
	     *  is not special...
	     */
	    cpo_save = p_cpo;
	    p_cpo = (char_u *)"Bk";
	    res = replace_termcodes((char_u *)(data->lpData), &res,
								 FALSE, TRUE);
	    p_cpo = cpo_save;
#ifdef FEAT_GUI
	    add_to_input_buf(res, STRLEN(res));
#else
	    ins_typebuf(res, REMAP_NONE, 0, TRUE, TRUE);
#endif
	    vim_free((char_u *)res);

	    /* Remember who sent this, for <client> */
	    clientWindow = sender;

	    return 1;
	case COPYDATA_EXPR:
	    res = eval_to_string(data->lpData, NULL);
	    reply.dwData = COPYDATA_REPLY;
	    reply.lpData = res;
	    reply.cbData = STRLEN(res) + 1;

	    /* Remember who sent this, for <client> */
	    clientWindow = sender;

	    return SendMessage(sender, WM_COPYDATA, (WPARAM)message_window,
			      (LPARAM)(&reply));
	case COPYDATA_REPLY:
	    sprintf((char *)winstr, "0x%x", (unsigned int)sender);
	    save_reply(winstr, data->lpData);
#ifdef FEAT_AUTOCMD
	    apply_autocmds(EVENT_REMOTEREPLY, winstr, data->lpData,
								TRUE, curbuf);
#endif
	    return 1;
	case COPYDATA_GETHWND:
	    id = (struct server_id *)GetWindowLong(message_window,
								GWL_USERDATA);
	    if (STRICMP(id->name, data->lpData) == 0)
		id->hwnd = sender;
	    return 1;
	case COPYDATA_LIST:
	    ga = (garray_T *)GetWindowLong(message_window, GWL_USERDATA);
	    ga_concat(ga, data->lpData);
	    ga_concat(ga, "\n");
	    return 1;
	}

	return 0;
    }
    else if (msg == WM_DESTROY)
    {
	/* We're dying - wake up any clients waiting for us */
	SendMessage(HWND_BROADCAST, vim_broadcast, 0, BROADCAST_WAKEUP);
	return 0;
    }
    else
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* Initialise the message handling process. This involves creating a window
 * to handle messages - the window will not be visible.
 */
    int
serverInitMessaging(void)
{
    WNDCLASS wndclass;
    HINSTANCE s_hinst = (HINSTANCE)GetModuleHandle(0);

    /* Clean up on exit */
    atexit(CleanUpMessaging);

    /* Register a broadcast message */
    vim_broadcast = RegisterWindowMessage("VimBroadcast");

    /* Register a window class - we only really care
     * about the window procedure
     */
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = s_hinst;
    wndclass.hIcon = NULL;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "VIM_MESSAGES";
    RegisterClass(&wndclass);

    /* Create the message window. It will be hidden,
     * so the details don't matter.
     */
    message_window = CreateWindow("VIM_MESSAGES", "Vim Message Window",
			 WS_OVERLAPPEDWINDOW,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, NULL, NULL,
			 s_hinst, NULL);

    return 1;
}

    static HWND
findServer(char_u *name)
{
    struct server_id id;

    id.name = name;
    id.hwnd = 0;

    SetWindowLong(message_window, GWL_USERDATA, (unsigned long)&id);
    SendMessage(HWND_BROADCAST, vim_broadcast, (WPARAM)message_window,
							    COPYDATA_GETHWND);
    SetWindowLong(message_window, GWL_USERDATA, 0);

    return id.hwnd;
}

    void
serverSetName(char_u *name)
{
    char_u	*ok_name;
    HWND	hwnd = 0;
    int		i = 0;
    char_u	*p;

    /* Leave enough space for a 9-digit suffix to ensure uniqueness! */
    ok_name = alloc(STRLEN(name) + 10);

    STRCPY(ok_name, name);
    p = ok_name + STRLEN(name);

    for (;;)
    {
	hwnd = findServer(ok_name);
	if (hwnd == 0)
	    break;

	++i;
	if (i >= 1000)
	    break;

	sprintf((char *)p, "%d", i);
    }

    if (hwnd != 0)
	vim_free(ok_name);
    else
    {
	serverName = ok_name;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
#endif
    }
}

    char_u *
serverGetVimNames(void)
{
    garray_T ga;

    ga_init2(&ga, 1, 100);

    /* Get all the server names */
    SetWindowLong(message_window, GWL_USERDATA, (unsigned long)&ga);
    SendMessage(HWND_BROADCAST, vim_broadcast, (WPARAM)message_window,
							       COPYDATA_LIST);
    SetWindowLong(message_window, GWL_USERDATA, 0);

    return ga.ga_data;
}

    int
serverSendReply(name, reply)
    char_u	 *name;			/* Where to send. */
    char_u	 *reply;		/* What to send. */
{
    HWND target;
    COPYDATASTRUCT data;
    int n = 0;

    /* The "name" argument is a magic cookie obtained from expand("<client>").
     * It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
     * value of the client's message window HWND.
     */
    sscanf((char *)name, "%x", &n);
    if (n == 0)
	return -1;

    target = (HWND)n;
    if (!IsWindow(target))
	return -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = STRLEN(reply) + 1;
    data.lpData = reply;

    if (SendMessage(target, WM_COPYDATA, (WPARAM)message_window,
							     (LPARAM)(&data)))
	return 0;

    return -1;
}

    int
serverSendToVim(name, cmd, result, ptarget, asExpr)
    char_u	 *name;			/* Where to send. */
    char_u	 *cmd;			/* What to send. */
    char_u	 **result;		/* Result of eval'ed expression */
    void	 *ptarget;		/* HWND of server */
    int		 asExpr;                /* Expression or keys? */
{
    HWND target = findServer(name);
    COPYDATASTRUCT data;
    char_u *retval = 0;

    if (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = asExpr ? COPYDATA_EXPR : COPYDATA_KEYS;
    data.cbData = STRLEN(cmd) + 1;
    data.lpData = cmd;

    if (SendMessage(target, WM_COPYDATA, (WPARAM)message_window,
							(LPARAM)(&data)) == 0)
	return -1;

    if (asExpr)
    {
	retval = (char_u *)GetWindowLong(message_window, GWL_USERDATA);
	SetWindowLong(message_window, GWL_USERDATA, 0);
    }

    if (result == NULL)
	vim_free(retval);
    else
	*result = retval; /* Caller assumes responsibility for freeing */

    return 0;
}

/* Replies from server need to be stored until the client picks them up via
 * serverreply_read(). So we maintain a list of server-id/reply pairs.
 * Note that there could be multiple replies from one server pending if the
 * client is slow picking them up.
 * We just store the replies in a simple list. When we remove an entry, we
 * move list entries down to fill the gap.
 * The server ID is simply the HWND, stored as a string (formatted in %x
 * format).
 */
typedef struct
{
    char_u *id;
    char_u *reply;
}
reply_T;

static garray_T reply_list = {0, 0, sizeof(reply_T), 5, 0};

#define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
#define REPLY_COUNT (reply_list.ga_len)

/* Flag which is used to wait for a reply */
static int reply_received = 0;

    static int
save_reply(char_u *id, char_u *reply)
{
    reply_T *rep;

    if (ga_grow(&reply_list, 1) == FAIL)
	return FAIL;

    rep = REPLY_ITEM(REPLY_COUNT);
    rep->id = vim_strsave(id);
    rep->reply = vim_strsave(reply);
    if (rep->id == 0 || rep->reply == 0)
    {
	vim_free(rep->id);
	vim_free(rep->reply);
	return FAIL;
    }

    ++REPLY_COUNT;
    reply_received = 1;
    return OK;
}

    char_u *
serverGetReply(char_u *id, int remove, int wait)
{
    int i;
    char_u *p;
    char_u *reply;
    reply_T *rep;

    for (i = 0; i < REPLY_COUNT; ++i)
    {
	rep = REPLY_ITEM(i);
	if (STRCMP(rep->id, id) == 0)
	{
	    size_t bytes;

	    /* If we aren't going to remove the reply from the list,
	     * just return the value we've found
	     */
	    if (!remove)
		return rep->reply;

	    /* Save the values we've found for later */
	    p = rep->id;
	    reply = rep->reply;

	    /* Move the rest of the list down to fill the gap */
	    bytes = (REPLY_COUNT - i - 1) * sizeof(reply_T);
	    mch_memmove(rep, rep + 1, bytes);
	    --REPLY_COUNT;

	    /* Free the ID, which was allocated when it was stored. Return the
	     * reply to the caller, who takes on responsibility for freeing
	     * it.
	     */
	    vim_free(p);
	    return reply;
	}
    }

    /* If we got here, we didn't find a reply. Return immediately unless the
     * "wait" parameter is set.
     */
    if (!wait)
	return 0;

    /* We need to wait for a reply. Enter a message loop until the
     * "reply_received" flag gets set, then check if this is for us.
     */
    for (;;)
    {
	MSG msg;
	HWND server;
	int n = 0;

	/* The server's HWND is encoded in the 'id' parameter */
	sscanf((char *)id, "%x", &n);
	server = (HWND)n;

	/* Nothing found so far... */
	reply_received = 0;

	/* Loop until we receive a reply */
	while (reply_received == 0)
	{
	    /* Wait for a SendMessage() call to us.  This could be the reply we
	     * are waiting for.  */
	    MsgWaitForMultipleObjects(0, NULL, TRUE, INFINITE, QS_SENDMESSAGE);

	    /* If the server has died, give up */
	    if (!IsWindow(server))
		return 0;

	    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}

	/* Check the one we just received */
	rep = REPLY_ITEM(REPLY_COUNT - 1);
	if (STRCMP(rep->id, id) == 0)
	    break;
    }

    /* We got a reply. Return it. */
    reply = rep->reply;

    if (remove)
    {
	vim_free(rep->id);
	--REPLY_COUNT;
    }

    return reply;
}

#if !defined(FEAT_GUI) || defined(PROTO)
/*
 * Process any messages in the Windows message queue. This is only required
 * for the console version, as the GUI version has a message loop anyway.
 */
    void
serverProcessPendingMessages(void)
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }
}
#endif

#endif /* FEAT_CLIENTSERVER */

#if defined(FEAT_GUI) || defined(FEAT_PRINTER) || defined(PROTO)

struct charset_pair
{
    char	*name;
    BYTE	charset;
};

static struct charset_pair
charset_pairs[] =
{
    {"ANSI",		ANSI_CHARSET},
    {"CHINESEBIG5",	CHINESEBIG5_CHARSET},
    {"DEFAULT",		DEFAULT_CHARSET},
    {"HANGEUL",		HANGEUL_CHARSET},
    {"OEM",		OEM_CHARSET},
    {"SHIFTJIS",	SHIFTJIS_CHARSET},
    {"SYMBOL",		SYMBOL_CHARSET},
#ifdef WIN3264
    {"ARABIC",		ARABIC_CHARSET},
    {"BALTIC",		BALTIC_CHARSET},
    {"EASTEUROPE",	EASTEUROPE_CHARSET},
    {"GB2312",		GB2312_CHARSET},
    {"GREEK",		GREEK_CHARSET},
    {"HEBREW",		HEBREW_CHARSET},
    {"JOHAB",		JOHAB_CHARSET},
    {"MAC",		MAC_CHARSET},
    {"RUSSIAN",		RUSSIAN_CHARSET},
    {"THAI",		THAI_CHARSET},
    {"TURKISH",		TURKISH_CHARSET},
# if !defined(_MSC_VER) || (_MSC_VER > 1010)
    {"VIETNAMESE",	VIETNAMESE_CHARSET},
# endif
#endif
    {NULL,		0}
};

/*
 * Convert a charset ID to a name.
 * Return NULL when not recognized.
 */
    char *
charset_id2name(int id)
{
    struct charset_pair *cp;

    for (cp = charset_pairs; cp->name != NULL; ++cp)
	if ((BYTE)id == cp->charset)
	    break;
    return cp->name;
}

static const LOGFONT s_lfDefault =
{
    -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    "Fixedsys"	/* see _ReadVimIni */
};

/* Initialise the "current height" to -12 (same as s_lfDefault) just
 * in case the user specifies a font in "guifont" with no size before a font
 * with an explicit size has been set. This defaults the size to this value
 * (-12 equates to roughly 9pt).
 */
int current_font_height = -12;		/* also used in gui_w48.c */

/* Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    static int
points_to_pixels(char_u *str, char_u **end, int vertical, int pprinter_dc)
{
    int		pixels;
    int		points = 0;
    int		divisor = 0;
    HWND	hwnd;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    while (*str != NUL)
    {
	if (*str == '.' && divisor == 0)
	{
	    /* Start keeping a divisor, for later */
	    divisor = 1;
	}
	else
	{
	    if (!isdigit(*str))
		break;

	    points *= 10;
	    points += *str - '0';
	    divisor *= 10;
	}
	++str;
    }

    if (divisor == 0)
	divisor = 1;

    if (printer_dc == NULL)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    else
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * divisor);

    if (printer_dc == NULL)
	ReleaseDC(hwnd, hdc);

    *end = str;
    return pixels;
}

    static int CALLBACK
font_enumproc(
    ENUMLOGFONT	    *elf,
    NEWTEXTMETRIC   *ntm,
    int		    type,
    LPARAM	    lparam)
{
    /* Return value:
     *	  0 = terminate now (monospace & ANSI)
     *	  1 = continue, still no luck...
     *	  2 = continue, but we have an acceptable LOGFONT
     *	      (monospace, not ANSI)
     * We use these values, as EnumFontFamilies returns 1 if the
     * callback function is never called. So, we check the return as
     * 0 = perfect, 2 = OK, 1 = no good...
     * It's not pretty, but it works!
     */

    LOGFONT *lf = (LOGFONT *)(lparam);

#ifndef FEAT_PROPORTIONAL_FONTS
    /* Ignore non-monospace fonts without further ado */
    if ((ntm->tmPitchAndFamily & 1) != 0)
	return 1;
#endif

    /* Remember this LOGFONT as a "possible" */
    *lf = elf->elfLogFont;

    /* Terminate the scan as soon as we find an ANSI font */
    if (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	return 0;

    /* Continue the scan - we have a non-ANSI font */
    return 2;
}

    static int
init_logfont(LOGFONT *lf)
{
    int		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamilies(hdc,
			 (LPCSTR)lf->lfFaceName,
			 (FONTENUMPROC)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    /* If we couldn't find a useable font, return failure */
    if (n == 1)
	return FAIL;

    /* Tidy up the rest of the LOGFONT structure. We set to a basic
     * font - get_logfont() sets bold, italic, etc based on the user's
     * input.
     */
    lf->lfHeight = current_font_height;
    lf->lfWidth = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;
    lf->lfWeight = FW_NORMAL;

    /* Return success */
    return OK;
}

    int
get_logfont(
    LOGFONT *lf,
    char_u  *name,
    HDC printer_dc)
{
    char_u	*p;
    int		i;
    static LOGFONT *lastlf = NULL;

    *lf = s_lfDefault;
    if (name == NULL)
	return 1;

    if (STRCMP(name, "*") == 0)
#if defined(FEAT_GUI_W32)
    {
	CHOOSEFONT	cf;
	/* if name is "*", bring up std font dialog: */
	memset(&cf, 0, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	if (lastlf != NULL)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	if (ChooseFont(&cf))
	    goto theend;
    }
#else
	return 0;
#endif

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    for (p = name; *p && *p != ':'; p++)
    {
	if (p - name + 1 > LF_FACESIZE)
	    return 0;			/* Name too long */
	lf->lfFaceName[p - name] = *p;
    }
    if (p != name)
	lf->lfFaceName[p - name] = NUL;

    /* First set defaults */
    lf->lfHeight = -12;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    if (init_logfont(lf) == FAIL)
    {
	int	did_replace = FALSE;

	for (i = 0; lf->lfFaceName[i]; ++i)
	    if (lf->lfFaceName[i] == '_')
	    {
		lf->lfFaceName[i] = ' ';
		did_replace = TRUE;
	    }
	if (!did_replace || init_logfont(lf) == FAIL)
	    return 0;
    }

    while (*p == ':')
	p++;

    /* Set the values found after ':' */
    while (*p)
    {
	switch (*p++)
	{
	    case 'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (int)printer_dc);
		break;
	    case 'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (int)printer_dc);
		break;
	    case 'b':
#ifndef MSWIN16_FASTTEXT
		lf->lfWeight = FW_BOLD;
#endif
		break;
	    case 'i':
#ifndef MSWIN16_FASTTEXT
		lf->lfItalic = TRUE;
#endif
		break;
	    case 'u':
		lf->lfUnderline = TRUE;
		break;
	    case 's':
		lf->lfStrikeOut = TRUE;
		break;
	    case 'c':
		{
		    struct charset_pair *cp;

		    for (cp = charset_pairs; cp->name != NULL; ++cp)
			if (STRNCMP(p, cp->name, strlen(cp->name)) == 0)
			{
			    lf->lfCharSet = cp->charset;
			    p += strlen(cp->name);
			    break;
			}
		    if (cp->name == NULL)
		    {
			sprintf((char *)IObuff, _("E244: Illegal charset name \"%s\" in font name \"%s\""), p, name);
			EMSG(IObuff);
			break;
		    }
		    break;
		}
	    default:
		sprintf((char *)IObuff,
			_("E245: Illegal char '%c' in font name \"%s\""),
			p[-1], name);
		EMSG(IObuff);
		break;
	}
	while (*p == ':')
	    p++;
    }

#if defined(FEAT_GUI_W32)
theend:
#endif
    /* ron: init lastlf */
    vim_free(lastlf);
    lastlf = (LOGFONT *)alloc(sizeof(LOGFONT));
    if (lastlf != NULL)
	mch_memmove(lastlf, lf, sizeof(LOGFONT));

    return 1;
}

#endif /* defined(FEAT_GUI) || defined(FEAT_PRINTER) */
