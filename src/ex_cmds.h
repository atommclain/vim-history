/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * This file defines the Ex commands.
 * When DO_DECLARE_EXCMD is defined, the table with ex command names and
 * options results.
 * When DO_DECLARE_EXCMD is NOT defined, the enum with all the Ex commands
 * results.
 * This clever trick was invented by Ron Aaron.
 */

/*
 * When adding an Ex command:
 * 1. Add an entry in the table below.  Keep it sorted on the shortest
 *    version of the command name that works.  If it doesn't start with a
 *    lower case letter, add it at the end.
 * 2. Add a "case: CMD_xxx" in the big switch in ex_docmd.c.
 * 3. Add an entry in the index for Ex commands at ":help ex-cmd-index".
 * 4. Add documentation in ../doc/xxx.txt.  Add a tag for both the short and
 *    long name of the command.
 */

#ifdef RANGE
# undef RANGE			/* SASC on Amiga defines it */
#endif

#define RANGE		0x001	/* allow a linespecs */
#define BANG		0x002	/* allow a ! after the command name */
#define EXTRA		0x004	/* allow extra args after command name */
#define XFILE		0x008	/* expand wildcards in extra part */
#define NOSPC		0x010	/* no spaces allowed in the extra part */
#define	DFLALL		0x020	/* default file range is 1,$ */
#define WHOLEFOLD	0x040	/* extend range to include whole fold also
				   when less than two numbers given */
#define NEEDARG		0x080	/* argument required */
#define TRLBAR		0x100	/* check for trailing vertical bar */
#define REGSTR		0x200	/* allow "x for register designation */
#define COUNT		0x400	/* allow count in argument, after command */
#define NOTRLCOM	0x800	/* no trailing comment allowed */
#define ZEROR	       0x1000	/* zero line number allowed */
#define USECTRLV       0x2000	/* do not remove CTRL-V from argument */
#define NOTADR	       0x4000	/* number before command is not an address */
#define EDITCMD	       0x8000	/* allow "+command" argument */
#define BUFNAME	      0x10000L	/* accepts buffer name */
#define BUFUNL	      0x20000L	/* accepts unlisted buffer too */
#define ARGOPT	      0x40000L	/* allow "++opt=val" argument */
#define SBOXOK	      0x80000L	/* allowed in the sandbox */
#define CMDWIN	     0x100000L	/* allowed in cmdline window */
#define FILES	(XFILE | EXTRA)	/* multiple extra files allowed */
#define WORD1	(EXTRA | NOSPC)	/* one extra word allowed */
#define FILE1	(FILES | NOSPC)	/* 1 file allowed, defaults to current file */

#ifndef DO_DECLARE_EXCMD
typedef struct exarg exarg_T;
#endif

/*
 * This array maps ex command names to command codes.
 * The order in which command names are listed below is significant --
 * ambiguous abbreviations are always resolved to be the first possible match
 * (e.g. "r" is taken to mean "read", not "rewind", because "read" comes
 * before "rewind").
 * Not supported commands are included to avoid ambiguities.
 */
#ifdef EX
# undef EX	    /* just in case */
#endif
#ifdef DO_DECLARE_EXCMD
# define EX(a, b, c, d)  {(char_u *)b, c, d}

typedef void (*ex_func_T) __ARGS((exarg_T *eap));

static struct cmdname
{
    char_u	*cmd_name;	/* name of the command */
    ex_func_T   cmd_func;	/* function for this command */
    long_u	cmd_argt;	/* flags declared above */
}
# if defined(FEAT_GUI_W16)
_far
# endif
cmdnames[] =
#else
# define EX(a, b, c, d)  a
enum CMD_index
#endif
{
EX(CMD_append,		"append",	ex_append,
			BANG|RANGE|ZEROR|TRLBAR|CMDWIN),
EX(CMD_abbreviate,	"abbreviate",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_abclear,		"abclear",	ex_abclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_all,		"all",		ex_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_amenu,		"amenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_anoremenu,	"anoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_args,		"args",		ex_args,
			BANG|FILES|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_argadd,		"argadd",	ex_argadd,
			BANG|NEEDARG|RANGE|NOTADR|COUNT|FILES|TRLBAR),
EX(CMD_argdelete,	"argdelete",	ex_argdelete,
			BANG|RANGE|NOTADR|COUNT|FILES|TRLBAR),
EX(CMD_argdo,		"argdo",	ex_listdo,
			BANG|NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_argedit,		"argedit",	ex_argedit,
			BANG|NEEDARG|RANGE|NOTADR|COUNT|FILE1|EDITCMD|TRLBAR),
EX(CMD_argglobal,	"argglobal",	ex_args,
			BANG|FILES|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_arglocal,	"arglocal",	ex_args,
			BANG|FILES|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_argument,	"argument",	ex_argument,
			BANG|RANGE|NOTADR|COUNT|EXTRA|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_ascii,		"ascii",	do_ascii,
			TRLBAR|CMDWIN),
EX(CMD_autocmd,		"autocmd",	ex_autocmd,
			BANG|EXTRA|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_augroup,		"augroup",	ex_autocmd,
			WORD1|TRLBAR|CMDWIN),
EX(CMD_aunmenu,		"aunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_buffer,		"buffer",	ex_buffer,
			BANG|RANGE|NOTADR|BUFNAME|BUFUNL|COUNT|EXTRA|TRLBAR),
EX(CMD_bNext,		"bNext",	ex_bprevious,
			BANG|RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_ball,		"ball",		do_buffer_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_badd,		"badd",		ex_edit,
			NEEDARG|FILE1|EDITCMD|TRLBAR|CMDWIN),
EX(CMD_bdelete,		"bdelete",	ex_bunload,
			BANG|RANGE|NOTADR|BUFNAME|COUNT|EXTRA|TRLBAR),
EX(CMD_behave,		"behave",	ex_behave,
			NEEDARG|WORD1|TRLBAR|CMDWIN),
EX(CMD_bfirst,		"bfirst",	ex_brewind,
			BANG|RANGE|NOTADR|TRLBAR),
EX(CMD_blast,		"blast",	ex_blast,
			BANG|RANGE|NOTADR|TRLBAR),
EX(CMD_bmodified,	"bmodified",	ex_bmodified,
			BANG|RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_bnext,		"bnext",	ex_bnext,
			BANG|RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_botright,	"botright",	ex_botright,
			NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_bprevious,	"bprevious",	ex_bprevious,
			BANG|RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_brewind,		"brewind",	ex_brewind,
			BANG|RANGE|NOTADR|TRLBAR),
EX(CMD_break,		"break",	ex_break,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_breakadd,	"breakadd",	ex_breakadd,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_breakdel,	"breakdel",	ex_breakdel,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_breaklist,	"breaklist",	ex_breaklist,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_browse,		"browse",	ex_browse,
			NEEDARG|EXTRA|NOTRLCOM|CMDWIN),
EX(CMD_buffers,		"buffers",	buflist_list,
			BANG|TRLBAR|CMDWIN),
EX(CMD_bufdo,		"bufdo",	ex_listdo,
			BANG|NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_bunload,		"bunload",	ex_bunload,
			BANG|RANGE|NOTADR|BUFNAME|COUNT|EXTRA|TRLBAR),
EX(CMD_bwipeout,	"bwipeout",	ex_bunload,
			BANG|RANGE|NOTADR|BUFNAME|BUFUNL|COUNT|EXTRA|TRLBAR),
EX(CMD_change,		"change",	ex_change,
			BANG|WHOLEFOLD|RANGE|COUNT|TRLBAR|CMDWIN),
EX(CMD_cNext,		"cNext",	ex_cnext,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cabbrev,		"cabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cabclear,	"cabclear",	ex_abclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_call,		"call",		ex_call,
			RANGE|NEEDARG|EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_cc,		"cc",		ex_cc,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cd,		"cd",		ex_cd,
			FILE1|TRLBAR|CMDWIN),
EX(CMD_center,		"center",	ex_align,
			TRLBAR|RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_cfile,		"cfile",	ex_cfile,
			TRLBAR|FILE1|BANG),
EX(CMD_cfirst,		"cfirst",	ex_cc,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_chdir,		"chdir",	ex_cd,
			FILE1|TRLBAR|CMDWIN),
EX(CMD_checkpath,	"checkpath",	ex_checkpath,
			TRLBAR|BANG|CMDWIN),
EX(CMD_checktime,	"checktime",	ex_checktime,
			RANGE|NOTADR|BUFNAME|COUNT|EXTRA|TRLBAR),
EX(CMD_clist,		"clist",	qf_list,
			BANG|EXTRA|TRLBAR|CMDWIN),
EX(CMD_clast,		"clast",	ex_cc,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_close,		"close",	ex_close,
			BANG|TRLBAR|CMDWIN),
EX(CMD_cmap,		"cmap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cmapclear,	"cmapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_cmenu,		"cmenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cnext,		"cnext",	ex_cnext,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cnewer,		"cnewer",	qf_age,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_cnfile,		"cnfile",	ex_cnext,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cnoremap,	"cnoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cnoreabbrev,	"cnoreabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cnoremenu,	"cnoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_copy,		"copy",		ex_copymove,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),
EX(CMD_colder,		"colder",	qf_age,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_command,		"command",	ex_command,
			EXTRA|BANG|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_comclear,	"comclear",	ex_comclear,
			TRLBAR|CMDWIN),
EX(CMD_continue,	"continue",	ex_continue,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_confirm,		"confirm",	ex_confirm,
			NEEDARG|EXTRA|NOTRLCOM|CMDWIN),
EX(CMD_cprevious,	"cprevious",	ex_cnext,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cquit,		"cquit",	ex_cquit,
			TRLBAR|BANG),
EX(CMD_crewind,		"crewind",	ex_cc,
			RANGE|NOTADR|COUNT|TRLBAR|BANG),
EX(CMD_cscope,		"cscope",	do_cscope,
			EXTRA|NOTRLCOM|SBOXOK),
EX(CMD_cstag,		"cstag",	do_cstag,
			BANG|TRLBAR|WORD1),
EX(CMD_cunmap,		"cunmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cunabbrev,	"cunabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cunmenu,		"cunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_cwindow,		"cwindow",	ex_cwindow,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_delete,		"delete",	ex_operators,
			RANGE|WHOLEFOLD|REGSTR|COUNT|TRLBAR|CMDWIN),
EX(CMD_debug,		"debug",	ex_debug,
			NEEDARG|EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_delcommand,	"delcommand",	ex_delcommand,
			NEEDARG|WORD1|TRLBAR|CMDWIN),
EX(CMD_delfunction,	"delfunction",	ex_delfunction,
			NEEDARG|WORD1|TRLBAR|CMDWIN),
EX(CMD_display,		"display",	ex_display,
			EXTRA|NOTRLCOM|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_diffupdate,	"diffupdate",	ex_diffupdate,
			TRLBAR),
EX(CMD_diffget,		"diffget",	ex_diffgetput,
			RANGE|EXTRA|TRLBAR),
EX(CMD_diffpatch,	"diffpatch",	ex_diffpatch,
			EXTRA|NEEDARG|FILE1|TRLBAR),
EX(CMD_diffput,		"diffput",	ex_diffgetput,
			RANGE|EXTRA|TRLBAR),
EX(CMD_diffsplit,	"diffsplit",	ex_diffsplit,
			EXTRA|FILE1|TRLBAR),
EX(CMD_digraphs,	"digraphs",	ex_digraphs,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_djump,		"djump",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA),
EX(CMD_dlist,		"dlist",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_doautocmd,	"doautocmd",	ex_doautocmd,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_doautoall,	"doautoall",	ex_doautoall,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_drop,		"drop",		ex_drop,
			FILES|EDITCMD|NEEDARG|ARGOPT|TRLBAR),
EX(CMD_dsearch,		"dsearch",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_dsplit,		"dsplit",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA),
EX(CMD_edit,		"edit",		ex_edit,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_echo,		"echo",		ex_echo,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_echoerr,		"echoerr",	ex_execute,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_echohl,		"echohl",	ex_echohl,
			EXTRA|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_echomsg,		"echomsg",	ex_execute,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_echon,		"echon",	ex_echo,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_else,		"else",		ex_else,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_elseif,		"elseif",	ex_else,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_emenu,		"emenu",	ex_emenu,
			NEEDARG|EXTRA|TRLBAR|NOTRLCOM|RANGE|NOTADR|CMDWIN),
EX(CMD_endif,		"endif",	ex_endif,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_endfunction,	"endfunction",	ex_endfunction,
			TRLBAR|CMDWIN),
EX(CMD_endwhile,	"endwhile",	ex_endwhile,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_enew,		"enew",		ex_edit,
			BANG|TRLBAR),
EX(CMD_ex,		"ex",		ex_edit,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_execute,		"execute",	ex_execute,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_exit,		"exit",		ex_exit,
			RANGE|WHOLEFOLD|BANG|FILE1|ARGOPT|DFLALL|TRLBAR|CMDWIN),
EX(CMD_file,		"file",		ex_file,
			BANG|FILE1|TRLBAR),
EX(CMD_files,		"files",	buflist_list,
			BANG|TRLBAR|CMDWIN),
EX(CMD_filetype,	"filetype",	ex_filetype,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_find,		"find",		ex_find,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_finish,		"finish",	ex_finish,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_first,		"first",	ex_rewind,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_fixdel,		"fixdel",	do_fixdel,
			TRLBAR|CMDWIN),
EX(CMD_fold,		"fold",		ex_fold,
			RANGE|WHOLEFOLD|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_foldclose,	"foldclose",	ex_foldopen,
			RANGE|WHOLEFOLD|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_folddoopen,	"folddoopen",	ex_folddo,
			RANGE|DFLALL|NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_folddoclosed,	"folddoclosed",	ex_folddo,
			RANGE|DFLALL|NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_foldopen,	"foldopen",	ex_foldopen,
			RANGE|WHOLEFOLD|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_function,	"function",	ex_function,
			EXTRA|BANG|CMDWIN),
EX(CMD_global,		"global",	ex_global,
			RANGE|WHOLEFOLD|BANG|EXTRA|DFLALL|CMDWIN),
EX(CMD_goto,		"goto",		ex_goto,
			RANGE|NOTADR|COUNT|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_grep,		"grep",		ex_make,
			BANG|NEEDARG|EXTRA|NOTRLCOM|TRLBAR|XFILE),
EX(CMD_grepadd,		"grepadd",	ex_make,
			BANG|NEEDARG|EXTRA|NOTRLCOM|TRLBAR|XFILE),
EX(CMD_gui,		"gui",		ex_gui,
			BANG|FILES|EDITCMD|ARGOPT|TRLBAR|CMDWIN),
EX(CMD_gvim,		"gvim",		ex_gui,
			BANG|FILES|EDITCMD|ARGOPT|TRLBAR|CMDWIN),
EX(CMD_help,		"help",		ex_help,
			EXTRA|NOTRLCOM),
EX(CMD_helpfind,	"helpfind",	ex_helpfind,
			EXTRA|NOTRLCOM),
EX(CMD_helptags,	"helptags",	ex_helptags,
			NEEDARG|FILE1|TRLBAR|CMDWIN),
EX(CMD_highlight,	"highlight",	ex_highlight,
			BANG|EXTRA|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_hide,		"hide",		ex_hide,
			BANG|EXTRA|NOTRLCOM),
EX(CMD_history,		"history",	ex_history,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_insert,		"insert",	ex_append,
			BANG|RANGE|TRLBAR|CMDWIN),
EX(CMD_iabbrev,		"iabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_iabclear,	"iabclear",	ex_abclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_if,		"if",		ex_if,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_ijump,		"ijump",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA),
EX(CMD_ilist,		"ilist",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_imap,		"imap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_imapclear,	"imapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_imenu,		"imenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_inoremap,	"inoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_inoreabbrev,	"inoreabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_inoremenu,	"inoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_intro,		"intro",	ex_intro,
			TRLBAR|CMDWIN),
EX(CMD_isearch,		"isearch",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_isplit,		"isplit",	ex_findpat,
			BANG|RANGE|DFLALL|WHOLEFOLD|EXTRA),
EX(CMD_iunmap,		"iunmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_iunabbrev,	"iunabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_iunmenu,		"iunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_join,		"join",		ex_join,
			BANG|RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_jumps,		"jumps",	ex_jumps,
			TRLBAR|CMDWIN),
EX(CMD_k,		"k",		ex_mark,
			RANGE|WORD1|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_list,		"list",		ex_print,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_last,		"last",		ex_last,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_language,	"language",	ex_language,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_lcd,		"lcd",		ex_cd,
			FILE1|TRLBAR|CMDWIN),
EX(CMD_lchdir,		"lchdir",	ex_cd,
			FILE1|TRLBAR|CMDWIN),
EX(CMD_left,		"left",		ex_align,
			TRLBAR|RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_let,		"let",		ex_let,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_lmap,		"lmap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_lmapclear,	"lmapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_lnoremap,	"lnoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_loadview,	"loadview",	ex_loadview,
			FILE1|TRLBAR),
EX(CMD_loadkeymap,	"loadkeymap",	ex_loadkeymap,
			0),
EX(CMD_lunmap,		"lunmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_ls,		"ls",		buflist_list,
			BANG|TRLBAR|CMDWIN),
EX(CMD_move,		"move",		ex_copymove,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),
EX(CMD_mark,		"mark",		ex_mark,
			RANGE|WORD1|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_make,		"make",		ex_make,
			BANG|EXTRA|NOTRLCOM|TRLBAR|XFILE),
EX(CMD_map,		"map",		ex_map,
			BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_mapclear,	"mapclear",	ex_mapclear,
			BANG|TRLBAR|CMDWIN),
EX(CMD_marks,		"marks",	do_marks,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_match,		"match",	ex_match,
			EXTRA|CMDWIN),
EX(CMD_menu,		"menu",		ex_menu,
			RANGE|NOTADR|ZEROR|BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_menutrans,	"menutrans",	ex_menutrans,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_messages,	"messages",	ex_messages,
			TRLBAR|CMDWIN),
EX(CMD_mkexrc,		"mkexrc",	ex_mkrc,
			BANG|FILE1|TRLBAR|CMDWIN),
EX(CMD_mksession,	"mksession",	ex_mkrc,
			BANG|FILE1|TRLBAR),
EX(CMD_mkvimrc,		"mkvimrc",	ex_mkrc,
			BANG|FILE1|TRLBAR|CMDWIN),
EX(CMD_mkview,		"mkview",	ex_mkrc,
			BANG|FILE1|TRLBAR),
EX(CMD_mode,		"mode",		ex_mode,
			WORD1|TRLBAR|CMDWIN),
EX(CMD_next,		"next",		ex_next,
			RANGE|NOTADR|BANG|FILES|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_new,		"new",		ex_splitview,
			BANG|FILE1|RANGE|NOTADR|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_nmap,		"nmap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_nmapclear,	"nmapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_nmenu,		"nmenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_nnoremap,	"nnoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_nnoremenu,	"nnoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_noremap,		"noremap",	ex_map,
			BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_nohlsearch,	"nohlsearch",	ex_nohlsearch,
			TRLBAR|SBOXOK|CMDWIN),
EX(CMD_noreabbrev,	"noreabbrev",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_noremenu,	"noremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_normal,		"normal",	ex_normal,
			RANGE|BANG|EXTRA|NEEDARG|NOTRLCOM|USECTRLV|SBOXOK|CMDWIN),
EX(CMD_number,		"number",	ex_print,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_nunmap,		"nunmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_nunmenu,		"nunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_open,		"open",		ex_ni,
			TRLBAR),	/* not supported */
EX(CMD_omap,		"omap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_omapclear,	"omapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_omenu,		"omenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_only,		"only",		ex_only,
			BANG|TRLBAR),
EX(CMD_onoremap,	"onoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_onoremenu,	"onoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_options,		"options",	ex_options,
			TRLBAR),
EX(CMD_ounmap,		"ounmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_ounmenu,		"ounmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_print,		"print",	ex_print,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_pclose,		"pclose",	ex_pclose,
			BANG|TRLBAR),
EX(CMD_perl,		"perl",		ex_perl,
			RANGE|EXTRA|DFLALL|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_perldo,		"perldo",	ex_perldo,
			RANGE|EXTRA|DFLALL|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_pedit,		"pedit",	ex_pedit,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_pop,		"pop",		ex_tag,
			RANGE|NOTADR|BANG|COUNT|TRLBAR|ZEROR),
EX(CMD_popup,		"popup",	ex_popup,
			NEEDARG|EXTRA|TRLBAR|NOTRLCOM|CMDWIN),
EX(CMD_ppop,		"ppop",		ex_ptag,
			RANGE|NOTADR|BANG|COUNT|TRLBAR|ZEROR),
EX(CMD_preserve,	"preserve",	ex_preserve,
			TRLBAR),
EX(CMD_previous,	"previous",	ex_previous,
			EXTRA|RANGE|NOTADR|COUNT|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_promptfind,	"promptfind",	gui_mch_find_dialog,
			EXTRA|NOTRLCOM|CMDWIN),
EX(CMD_promptrepl,	"promptrepl",	gui_mch_replace_dialog,
			EXTRA|NOTRLCOM|CMDWIN),
EX(CMD_psearch,		"psearch",	ex_psearch,
			BANG|RANGE|WHOLEFOLD|DFLALL|EXTRA),
EX(CMD_ptag,		"ptag",		ex_ptag,
			RANGE|NOTADR|BANG|WORD1|TRLBAR|ZEROR),
EX(CMD_ptNext,		"ptNext",	ex_ptag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_ptfirst,		"ptfirst",	ex_ptag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_ptjump,		"ptjump",	ex_ptag,
			BANG|TRLBAR|WORD1),
EX(CMD_ptlast,		"ptlast",	ex_ptag,
			BANG|TRLBAR),
EX(CMD_ptnext,		"ptnext",	ex_ptag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_ptprevious,	"ptprevious",	ex_ptag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_ptrewind,	"ptrewind",	ex_ptag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_ptselect,	"ptselect",	ex_ptag,
			BANG|TRLBAR|WORD1),
EX(CMD_put,		"put",		ex_put,
			RANGE|WHOLEFOLD|BANG|REGSTR|TRLBAR|ZEROR|CMDWIN),
EX(CMD_pwd,		"pwd",		ex_pwd,
			TRLBAR|CMDWIN),
EX(CMD_python,		"python",	ex_python,
			RANGE|EXTRA|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_pyfile,		"pyfile",	ex_pyfile,
			RANGE|FILE1|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_quit,		"quit",		ex_quit,
			BANG|TRLBAR|CMDWIN),
EX(CMD_quitall,		"quitall",	ex_quit_all,
			BANG|TRLBAR),
EX(CMD_qall,		"qall",		ex_quit_all,
			BANG|TRLBAR|CMDWIN),
EX(CMD_read,		"read",		ex_read,
			BANG|RANGE|WHOLEFOLD|FILE1|ARGOPT|TRLBAR|ZEROR|CMDWIN),
EX(CMD_recover,		"recover",	ex_recover,
			BANG|FILE1|TRLBAR),
EX(CMD_redo,		"redo",		ex_redo,
			TRLBAR|CMDWIN),
EX(CMD_redir,		"redir",	ex_redir,
			BANG|FILES|TRLBAR|CMDWIN),
EX(CMD_registers,	"registers",	ex_display,
			EXTRA|NOTRLCOM|TRLBAR|CMDWIN),
EX(CMD_resize,		"resize",	ex_resize,
			TRLBAR|WORD1),
EX(CMD_retab,		"retab",	ex_retab,
			TRLBAR|RANGE|WHOLEFOLD|DFLALL|BANG|WORD1|CMDWIN),
EX(CMD_return,		"return",	ex_return,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_rewind,		"rewind",	ex_rewind,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_right,		"right",	ex_align,
			TRLBAR|RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_runtime,		"runtime",	ex_runtime,
			BANG|NEEDARG|FILES|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_ruby,		"ruby",		ex_ruby,
			RANGE|EXTRA|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_rubydo,		"rubydo",	ex_rubydo,
			RANGE|DFLALL|EXTRA|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_rubyfile,	"rubyfile",	ex_rubyfile,
			RANGE|FILE1|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_rviminfo,	"rviminfo",	ex_viminfo,
			BANG|FILE1|TRLBAR|CMDWIN),
EX(CMD_substitute,	"substitute",	do_sub,
			RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_sNext,		"sNext",	ex_previous,
			EXTRA|RANGE|NOTADR|COUNT|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_sargument,	"sargument",	ex_argument,
			BANG|RANGE|NOTADR|COUNT|EXTRA|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_sall,		"sall",		ex_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_saveas,		"saveas",	ex_write,
			BANG|DFLALL|FILE1|ARGOPT|CMDWIN|TRLBAR),
EX(CMD_sbuffer,		"sbuffer",	ex_buffer,
			BANG|RANGE|NOTADR|BUFNAME|BUFUNL|COUNT|EXTRA|TRLBAR),
EX(CMD_sbNext,		"sbNext",	ex_bprevious,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_sball,		"sball",	do_buffer_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_sbfirst,		"sbfirst",	ex_brewind,
			TRLBAR),
EX(CMD_sblast,		"sblast",	ex_blast,
			TRLBAR),
EX(CMD_sbmodified,	"sbmodified",	ex_bmodified,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_sbnext,		"sbnext",	ex_bnext,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_sbprevious,	"sbprevious",	ex_bprevious,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_sbrewind,	"sbrewind",	ex_brewind,
			TRLBAR),
EX(CMD_scriptnames,	"scriptnames",	ex_scriptnames,
			TRLBAR|CMDWIN),
EX(CMD_scriptencoding,	"scriptencoding", ex_scriptencoding,
			WORD1|TRLBAR|CMDWIN),
EX(CMD_set,		"set",		ex_set,
			TRLBAR|EXTRA|CMDWIN),
EX(CMD_setfiletype,	"setfiletype",	ex_setfiletype,
			TRLBAR|EXTRA|NEEDARG|CMDWIN),
EX(CMD_setglobal,	"setglobal",	ex_set,
			TRLBAR|EXTRA|CMDWIN),
EX(CMD_setlocal,	"setlocal",	ex_set,
			TRLBAR|EXTRA|CMDWIN),
EX(CMD_sfind,		"sfind",	ex_splitview,
			BANG|FILE1|RANGE|NOTADR|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_sfirst,		"sfirst",	ex_rewind,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_shell,		"shell",	ex_shell,
			TRLBAR|CMDWIN),
EX(CMD_simalt,		"simalt",	gui_simulate_alt_key,
			NEEDARG|WORD1|TRLBAR|CMDWIN),
EX(CMD_sign,		"sign",		ex_sign,
			RANGE|NOTADR|EXTRA|NOSPC|CMDWIN),
EX(CMD_signs,		"signs",	ex_sign,
			TRLBAR|CMDWIN),
EX(CMD_silent,		"silent",	ex_silent,
			NEEDARG|EXTRA|BANG|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_sleep,		"sleep",	ex_sleep,
			RANGE|NOTADR|COUNT|EXTRA|TRLBAR|CMDWIN),
EX(CMD_slast,		"slast",	ex_last,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_smagic,		"smagic",	ex_submagic,
			RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_snext,		"snext",	ex_next,
			RANGE|NOTADR|BANG|FILES|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_sniff,		"sniff",	ex_sniff,
			EXTRA|TRLBAR),
EX(CMD_snomagic,	"snomagic",	ex_submagic,
			RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_source,		"source",	ex_source,
			BANG|FILE1|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_split,		"split",	ex_splitview,
			BANG|FILE1|RANGE|NOTADR|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_sprevious,	"sprevious",	ex_previous,
			EXTRA|RANGE|NOTADR|COUNT|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_srewind,		"srewind",	ex_rewind,
			EXTRA|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_stop,		"stop",		ex_stop,
			TRLBAR|BANG|CMDWIN),
EX(CMD_stag,		"stag",		ex_stag,
			RANGE|NOTADR|BANG|WORD1|TRLBAR|ZEROR),
EX(CMD_startinsert,	"startinsert",	ex_startinsert,
			BANG|TRLBAR|CMDWIN),
EX(CMD_stjump,		"stjump",	ex_stag,
			BANG|TRLBAR|WORD1),
EX(CMD_stselect,	"stselect",	ex_stag,
			BANG|TRLBAR|WORD1),
EX(CMD_sunhide,		"sunhide",	do_buffer_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_suspend,		"suspend",	ex_stop,
			TRLBAR|BANG|CMDWIN),
EX(CMD_sview,		"sview",	ex_splitview,
			NEEDARG|RANGE|NOTADR|BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_swapname,	"swapname",	ex_swapname,
			TRLBAR|CMDWIN),
EX(CMD_syntax,		"syntax",	ex_syntax,
			EXTRA|NOTRLCOM|CMDWIN),
EX(CMD_syncbind,	"syncbind",	ex_syncbind,
			TRLBAR),
EX(CMD_t,		"t",		ex_copymove,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),
EX(CMD_tNext,		"tNext",	ex_tag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_tag,		"tag",		ex_tag,
			RANGE|NOTADR|BANG|WORD1|TRLBAR|ZEROR),
EX(CMD_tags,		"tags",		do_tags,
			TRLBAR|CMDWIN),
EX(CMD_tcl,		"tcl",		ex_tcl,
			RANGE|EXTRA|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_tcldo,		"tcldo",	ex_tcldo,
			RANGE|DFLALL|EXTRA|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_tclfile,		"tclfile",	ex_tclfile,
			RANGE|FILE1|NEEDARG|SBOXOK|CMDWIN),
EX(CMD_tearoff,		"tearoff",	ex_tearoff,
			NEEDARG|EXTRA|TRLBAR|NOTRLCOM|CMDWIN),
EX(CMD_tfirst,		"tfirst",	ex_tag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_tjump,		"tjump",	ex_tag,
			BANG|TRLBAR|WORD1),
EX(CMD_tlast,		"tlast",	ex_tag,
			BANG|TRLBAR),
EX(CMD_tmenu,		"tmenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_tnext,		"tnext",	ex_tag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_topleft,		"topleft",	ex_topleft,
			NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_tprevious,	"tprevious",	ex_tag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_trewind,		"trewind",	ex_tag,
			RANGE|NOTADR|BANG|TRLBAR|ZEROR),
EX(CMD_tselect,		"tselect",	ex_tag,
			BANG|TRLBAR|WORD1),
EX(CMD_tunmenu,		"tunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_undo,		"undo",		ex_undo,
			TRLBAR|CMDWIN),
EX(CMD_unabbreviate,	"unabbreviate",	ex_abbreviate,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_unsign,		"unsign",	ex_unsign,
			RANGE|NOTADR|EXTRA|NOSPC|CMDWIN),
EX(CMD_unhide,		"unhide",	do_buffer_all,
			RANGE|NOTADR|COUNT|TRLBAR),
EX(CMD_unlet,		"unlet",	ex_unlet,
			BANG|EXTRA|NEEDARG|TRLBAR|SBOXOK|CMDWIN),
EX(CMD_unmap,		"unmap",	ex_unmap,
			BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_unmenu,		"unmenu",	ex_menu,
			BANG|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_update,		"update",	ex_update,
			RANGE|WHOLEFOLD|BANG|FILE1|ARGOPT|DFLALL|TRLBAR),
EX(CMD_vglobal,		"vglobal",	ex_global,
			RANGE|WHOLEFOLD|EXTRA|DFLALL|CMDWIN),
EX(CMD_version,		"version",	ex_version,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_verbose,		"verbose",	ex_verbose,
			NEEDARG|RANGE|NOTADR|EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_vertical,	"vertical",	ex_vertical,
			NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_visual,		"visual",	ex_edit,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_view,		"view",		ex_edit,
			BANG|FILE1|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_vmap,		"vmap",		ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_vmapclear,	"vmapclear",	ex_mapclear,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_vmenu,		"vmenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_vnoremap,	"vnoremap",	ex_map,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_vnew,		"vnew",		ex_splitview,
			BANG|FILE1|RANGE|NOTADR|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_vnoremenu,	"vnoremenu",	ex_menu,
			RANGE|NOTADR|ZEROR|EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_vsplit,		"vsplit",	ex_splitview,
			BANG|FILE1|RANGE|NOTADR|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_vunmap,		"vunmap",	ex_unmap,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_vunmenu,		"vunmenu",	ex_menu,
			EXTRA|TRLBAR|NOTRLCOM|USECTRLV|CMDWIN),
EX(CMD_write,		"write",	ex_write,
			RANGE|WHOLEFOLD|BANG|FILE1|ARGOPT|DFLALL|TRLBAR|CMDWIN),
EX(CMD_wNext,		"wNext",	ex_wnext,
			RANGE|WHOLEFOLD|NOTADR|BANG|FILE1|ARGOPT|TRLBAR),
EX(CMD_wall,		"wall",		do_wqall,
			BANG|TRLBAR|CMDWIN),
EX(CMD_while,		"while",	ex_while,
			EXTRA|NOTRLCOM|SBOXOK|CMDWIN),
EX(CMD_winsize,		"winsize",	ex_winsize,
			EXTRA|NEEDARG|TRLBAR),
EX(CMD_wincmd,		"wincmd",	ex_wincmd,
			NEEDARG|WORD1|RANGE|NOTADR),
EX(CMD_windo,		"windo",	ex_listdo,
			BANG|NEEDARG|EXTRA|NOTRLCOM),
EX(CMD_winpos,		"winpos",	ex_winpos,
			EXTRA|TRLBAR|CMDWIN),
EX(CMD_wnext,		"wnext",	ex_wnext,
			RANGE|NOTADR|BANG|FILE1|ARGOPT|TRLBAR),
EX(CMD_wprevious,	"wprevious",	ex_wnext,
			RANGE|NOTADR|BANG|FILE1|ARGOPT|TRLBAR),
EX(CMD_wq,		"wq",		ex_exit,
			RANGE|WHOLEFOLD|BANG|FILE1|ARGOPT|DFLALL|TRLBAR),
EX(CMD_wqall,		"wqall",	do_wqall,
			BANG|FILE1|ARGOPT|DFLALL|TRLBAR),
EX(CMD_wsverb,		"wsverb",	ex_wsverb,
			EXTRA|NOTADR|NEEDARG),
EX(CMD_wviminfo,	"wviminfo",	ex_viminfo,
			BANG|FILE1|TRLBAR|CMDWIN),
EX(CMD_xit,		"xit",		ex_exit,
			RANGE|WHOLEFOLD|BANG|FILE1|ARGOPT|DFLALL|TRLBAR|CMDWIN),
EX(CMD_xall,		"xall",		do_wqall,
			BANG|TRLBAR),
EX(CMD_yank,		"yank",		ex_operators,
			RANGE|WHOLEFOLD|REGSTR|COUNT|TRLBAR|CMDWIN),
EX(CMD_z,		"z",		ex_z,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),

/* commands that don't start with a lowercase letter */
EX(CMD_bang,		"!",		ex_bang,
			RANGE|WHOLEFOLD|BANG|FILES|CMDWIN),
EX(CMD_pound,		"#",		ex_print,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_and,		"&",		do_sub,
			RANGE|WHOLEFOLD|EXTRA|CMDWIN),
EX(CMD_star,		"*",		ex_at,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),
EX(CMD_lshift,		"<",		ex_operators,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_equal,		"=",		ex_equal,
			TRLBAR|CMDWIN),
EX(CMD_rshift,		">",		ex_operators,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_at,		"@",		ex_at,
			RANGE|WHOLEFOLD|EXTRA|TRLBAR|CMDWIN),
EX(CMD_Next,		"Next",		ex_previous,
			EXTRA|RANGE|NOTADR|COUNT|BANG|EDITCMD|ARGOPT|TRLBAR),
EX(CMD_Print,		"Print",	ex_print,
			RANGE|WHOLEFOLD|COUNT|TRLBAR|CMDWIN),
EX(CMD_X,		"X",		ex_X,
			TRLBAR),
EX(CMD_tilde,		"~",		do_sub,
			RANGE|WHOLEFOLD|EXTRA|CMDWIN),

#ifndef DO_DECLARE_EXCMD
#ifdef FEAT_USR_CMDS
    CMD_SIZE,		/* MUST be after all real commands! */
    CMD_USER = -1,	/* User-defined command */
    CMD_USER_BUF = -2	/* User-defined command local to buffer */
#else
    CMD_SIZE	/* MUST be the last one! */
#endif
#endif
};

#define USER_CMDIDX(idx) ((int)(idx) < 0)

#ifndef DO_DECLARE_EXCMD
typedef enum CMD_index cmdidx_T;

/*
 * Arguments used for Ex commands.
 */
struct exarg
{
    char_u	*arg;		/* argument of the command */
    char_u	*nextcmd;	/* next command (NULL if none) */
    char_u	*cmd;		/* the name of the command (except for :make) */
    char_u	**cmdlinep;	/* pointer to pointer of allocated cmdline */
    cmdidx_T	cmdidx;		/* the index for the command */
    long	argt;		/* flags for the command */
    int		skip;		/* don't execute the command, only parse it */
    int		forceit;	/* TRUE if ! present */
    int		addr_count;	/* the number of addresses given */
    linenr_T	line1;		/* the first line number */
    linenr_T	line2;		/* the second line number or count */
    char_u	*do_ecmd_cmd;	/* +command arg to be used in edited file */
    linenr_T	do_ecmd_lnum;	/* the line number in an edited file */
    int		append;		/* TRUE with ":w >>file" command */
    int		usefilter;	/* TRUE with ":w !command" and ":r!command" */
    int		amount;		/* number of '>' or '<' for shift command */
    int		regname;	/* register name (NUL if none) */
    int		force_ff;	/* forced 'fileformat' (index in cmd[]) */
#ifdef FEAT_MBYTE
    int		force_enc;	/* forced 'encoding' (index in cmd[]) */
#endif
#ifdef FEAT_USR_CMDS
    int		useridx;	/* user command index */
#endif
    char_u	*errmsg;	/* returned error message */
    char_u	*(*getline) __ARGS((int, void *, int));
    void	*cookie;	/* argument for getline() */
#ifdef FEAT_EVAL
    struct condstack *cstack;	/* condition stack for ":if" etc. */
#endif
};
#endif
