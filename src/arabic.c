/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * arabic.c: functions for Arabic language
 *
 * Included by main.c, when FEAT_ARABIC & FEAT_GUI is defined.
 *
 * --
 *
 * Author: Nadim Shaikli & Isam Bayazidi
 *
 */

static int  A_is_a __ARGS((int cur_c));
static int  A_is_s __ARGS((int cur_c));
static int  A_is_f __ARGS((int cur_c));
static int  chg_c_a2s __ARGS((int cur_c));
static int  chg_c_a2i __ARGS((int cur_c));
static int  chg_c_a2m __ARGS((int cur_c));
static int  chg_c_a2f __ARGS((int cur_c));
static int  chg_c_i2m __ARGS((int cur_c));
static int  chg_c_f2m __ARGS((int cur_c));
static int  chg_c_laa2i __ARGS((int hid_c));
static int  chg_c_laa2f __ARGS((int hid_c));
static void A_redo_shape __ARGS((int loc, int cur_c));
static int  A_firstc_laa __ARGS((int offset, int c));
static int  A_is_harakat __ARGS((int c));
static int  A_is_iso __ARGS((int c));
static int  A_is_formb __ARGS((int c));
static int  A_is_ok __ARGS((int c));
static int  A_is_valid __ARGS((int c));
static int  A_is_special __ARGS((int c));


/*
 * Returns True if c is an ISO-8859-6 shaped ARABIC letter (user entered)
 */
    static int
A_is_a(cur_c)
    int cur_c;
{
    switch (cur_c)
    {
	case a_HAMZA:
	case a_ALEF_MADDA:
	case a_ALEF_HAMZA_ABOVE:
	case a_WAW_HAMZA:
	case a_ALEF_HAMZA_BELOW:
	case a_YEH_HAMZA:
	case a_ALEF:
	case a_BEH:
	case a_TEH_MARBUTA:
	case a_TEH:
	case a_THEH:
	case a_JEEM:
	case a_HAH:
	case a_KHAH:
	case a_DAL:
	case a_THAL:
	case a_REH:
	case a_ZAIN:
	case a_SEEN:
	case a_SHEEN:
	case a_SAD:
	case a_DAD:
	case a_TAH:
	case a_ZAH:
	case a_AIN:
	case a_GHAIN:
	case a_TATWEEL:
	case a_FEH:
	case a_QAF:
	case a_KAF:
	case a_LAM:
	case a_MEEM:
	case a_NOON:
	case a_HEH:
	case a_WAW:
	case a_ALEF_MAKSURA:
	case a_YEH:
	    return TRUE;
    }

    return FALSE;
}


/*
 * Returns True if c is an Isolated Form-B ARABIC letter
 */
    static int
A_is_s(cur_c)
    int cur_c;
{
    switch (cur_c)
    {
	case a_s_HAMZA:
	case a_s_ALEF_MADDA:
	case a_s_ALEF_HAMZA_ABOVE:
	case a_s_WAW_HAMZA:
	case a_s_ALEF_HAMZA_BELOW:
	case a_s_YEH_HAMZA:
	case a_s_ALEF:
	case a_s_BEH:
	case a_s_TEH_MARBUTA:
	case a_s_TEH:
	case a_s_THEH:
	case a_s_JEEM:
	case a_s_HAH:
	case a_s_KHAH:
	case a_s_DAL:
	case a_s_THAL:
	case a_s_REH:
	case a_s_ZAIN:
	case a_s_SEEN:
	case a_s_SHEEN:
	case a_s_SAD:
	case a_s_DAD:
	case a_s_TAH:
	case a_s_ZAH:
	case a_s_AIN:
	case a_s_GHAIN:
	case a_s_FEH:
	case a_s_QAF:
	case a_s_KAF:
	case a_s_LAM:
	case a_s_MEEM:
	case a_s_NOON:
	case a_s_HEH:
	case a_s_WAW:
	case a_s_ALEF_MAKSURA:
	case a_s_YEH:
	    return TRUE;
    }

    return FALSE;
}


/*
 * Returns True if c is a Final shape of an ARABIC letter
 */
    static int
A_is_f(cur_c)
    int cur_c;
{
    switch (cur_c)
    {
	case a_f_ALEF_MADDA:
	case a_f_ALEF_HAMZA_ABOVE:
	case a_f_WAW_HAMZA:
	case a_f_ALEF_HAMZA_BELOW:
	case a_f_YEH_HAMZA:
	case a_f_ALEF:
	case a_f_BEH:
	case a_f_TEH_MARBUTA:
	case a_f_TEH:
	case a_f_THEH:
	case a_f_JEEM:
	case a_f_HAH:
	case a_f_KHAH:
	case a_f_DAL:
	case a_f_THAL:
	case a_f_REH:
	case a_f_ZAIN:
	case a_f_SEEN:
	case a_f_SHEEN:
	case a_f_SAD:
	case a_f_DAD:
	case a_f_TAH:
	case a_f_ZAH:
	case a_f_AIN:
	case a_f_GHAIN:
	case a_f_FEH:
	case a_f_QAF:
	case a_f_KAF:
	case a_f_LAM:
	case a_f_MEEM:
	case a_f_NOON:
	case a_f_HEH:
	case a_f_WAW:
	case a_f_ALEF_MAKSURA:
	case a_f_YEH:
	case a_f_LAM_ALEF_MADDA_ABOVE:
	case a_f_LAM_ALEF_HAMZA_ABOVE:
	case a_f_LAM_ALEF_HAMZA_BELOW:
	case a_f_LAM_ALEF:
	    return TRUE;
    }
    return FALSE;
}


/*
 * Change shape - from ISO-8859-6/Isolated to Form-B Isolated
 */
    static int
chg_c_a2s(cur_c)
    int cur_c;
{
    int tempc;

    switch (cur_c)
    {
	case a_HAMZA:
	    tempc = a_s_HAMZA;
	    break;
	case a_ALEF_MADDA:
	    tempc = a_s_ALEF_MADDA;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_ALEF_HAMZA_ABOVE;
	    break;
	case a_WAW_HAMZA:
	    tempc = a_s_WAW_HAMZA;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_s_ALEF_HAMZA_BELOW;
	    break;
	case a_YEH_HAMZA:
	    tempc = a_s_YEH_HAMZA;
	    break;
	case a_ALEF:
	    tempc = a_s_ALEF;
	    break;
	case a_TEH_MARBUTA:
	    tempc = a_s_TEH_MARBUTA;
	    break;
	case a_DAL:
	    tempc = a_s_DAL;
	    break;
	case a_THAL:
	    tempc = a_s_THAL;
	    break;
	case a_REH:
	    tempc = a_s_REH;
	    break;
	case a_ZAIN:
	    tempc = a_s_ZAIN;
	    break;
	case a_TATWEEL:			/* exceptions */
	    tempc = cur_c;
	    break;
	case a_WAW:
	    tempc = a_s_WAW;
	    break;
	case a_ALEF_MAKSURA:
	    tempc = a_s_ALEF_MAKSURA;
	    break;
	case a_BEH:
	    tempc = a_s_BEH;
	    break;
	case a_TEH:
	    tempc = a_s_TEH;
	    break;
	case a_THEH:
	    tempc = a_s_THEH;
	    break;
	case a_JEEM:
	    tempc = a_s_JEEM;
	    break;
	case a_HAH:
	    tempc = a_s_HAH;
	    break;
	case a_KHAH:
	    tempc = a_s_KHAH;
	    break;
	case a_SEEN:
	    tempc = a_s_SEEN;
	    break;
	case a_SHEEN:
	    tempc = a_s_SHEEN;
	    break;
	case a_SAD:
	    tempc = a_s_SAD;
	    break;
	case a_DAD:
	    tempc = a_s_DAD;
	    break;
	case a_TAH:
	    tempc = a_s_TAH;
	    break;
	case a_ZAH:
	    tempc = a_s_ZAH;
	    break;
	case a_AIN:
	    tempc = a_s_AIN;
	    break;
	case a_GHAIN:
	    tempc = a_s_GHAIN;
	    break;
	case a_FEH:
	    tempc = a_s_FEH;
	    break;
	case a_QAF:
	    tempc = a_s_QAF;
	    break;
	case a_KAF:
	    tempc = a_s_KAF;
	    break;
	case a_LAM:
	    tempc = a_s_LAM;
	    break;
	case a_MEEM:
	    tempc = a_s_MEEM;
	    break;
	case a_NOON:
	    tempc = a_s_NOON;
	    break;
	case a_HEH:
	    tempc = a_s_HEH;
	    break;
	case a_YEH:
	    tempc = a_s_YEH;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from ISO-8859-6/Isolated to Initial
 */
    static int
chg_c_a2i(cur_c)
    int cur_c;
{
    int tempc;

    switch (cur_c)
    {
	case a_YEH_HAMZA:
	    tempc = a_i_YEH_HAMZA;
	    break;
	case a_HAMZA:			/* exceptions */
	    tempc = a_s_HAMZA;
	    break;
	case a_ALEF_MADDA:		/* exceptions */
	    tempc = a_s_ALEF_MADDA;
	    break;
	case a_ALEF_HAMZA_ABOVE:	/* exceptions */
	    tempc = a_s_ALEF_HAMZA_ABOVE;
	    break;
	case a_WAW_HAMZA:		/* exceptions */
	    tempc = a_s_WAW_HAMZA;
	    break;
	case a_ALEF_HAMZA_BELOW:	/* exceptions */
	    tempc = a_s_ALEF_HAMZA_BELOW;
	    break;
	case a_ALEF:			/* exceptions */
	    tempc = a_s_ALEF;
	    break;
	case a_TEH_MARBUTA:		/* exceptions */
	    tempc = a_s_TEH_MARBUTA;
	    break;
	case a_DAL:			/* exceptions */
	    tempc = a_s_DAL;
	    break;
	case a_THAL:			/* exceptions */
	    tempc = a_s_THAL;
	    break;
	case a_REH:			/* exceptions */
	    tempc = a_s_REH;
	    break;
	case a_ZAIN:			/* exceptions */
	    tempc = a_s_ZAIN;
	    break;
	case a_TATWEEL:			/* exceptions */
	    tempc = cur_c;
	    break;
	case a_WAW:			/* exceptions */
	    tempc = a_s_WAW;
	    break;
	case a_ALEF_MAKSURA:		/* exceptions */
	    tempc = a_s_ALEF_MAKSURA;
	    break;
	case a_BEH:
	    tempc = a_i_BEH;
	    break;
	case a_TEH:
	    tempc = a_i_TEH;
	    break;
	case a_THEH:
	    tempc = a_i_THEH;
	    break;
	case a_JEEM:
	    tempc = a_i_JEEM;
	    break;
	case a_HAH:
	    tempc = a_i_HAH;
	    break;
	case a_KHAH:
	    tempc = a_i_KHAH;
	    break;
	case a_SEEN:
	    tempc = a_i_SEEN;
	    break;
	case a_SHEEN:
	    tempc = a_i_SHEEN;
	    break;
	case a_SAD:
	    tempc = a_i_SAD;
	    break;
	case a_DAD:
	    tempc = a_i_DAD;
	    break;
	case a_TAH:
	    tempc = a_i_TAH;
	    break;
	case a_ZAH:
	    tempc = a_i_ZAH;
	    break;
	case a_AIN:
	    tempc = a_i_AIN;
	    break;
	case a_GHAIN:
	    tempc = a_i_GHAIN;
	    break;
	case a_FEH:
	    tempc = a_i_FEH;
	    break;
	case a_QAF:
	    tempc = a_i_QAF;
	    break;
	case a_KAF:
	    tempc = a_i_KAF;
	    break;
	case a_LAM:
	    tempc = a_i_LAM;
	    break;
	case a_MEEM:
	    tempc = a_i_MEEM;
	    break;
	case a_NOON:
	    tempc = a_i_NOON;
	    break;
	case a_HEH:
	    tempc = a_i_HEH;
	    break;
	case a_YEH:
	    tempc = a_i_YEH;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from ISO-8859-6/Isolated to Medial
 */
    static int
chg_c_a2m(cur_c)
    int cur_c;
{
    int tempc;

    switch (cur_c)
    {
	case a_HAMZA:			/* exception */
	    tempc = a_s_HAMZA;
	    break;
	case a_ALEF_MADDA:		/* exception */
	    tempc = a_f_ALEF_MADDA;
	    break;
	case a_ALEF_HAMZA_ABOVE:	/* exception */
	    tempc = a_f_ALEF_HAMZA_ABOVE;
	    break;
	case a_WAW_HAMZA:		/* exception */
	    tempc = a_f_WAW_HAMZA;
	    break;
	case a_ALEF_HAMZA_BELOW:	/* exception */
	    tempc = a_f_ALEF_HAMZA_BELOW;
	    break;
	case a_YEH_HAMZA:
	    tempc = a_m_YEH_HAMZA;
	    break;
	case a_ALEF:			/* exception */
	    tempc = a_f_ALEF;
	    break;
	case a_BEH:
	    tempc = a_m_BEH;
	    break;
	case a_TEH_MARBUTA:		/* exception */
	    tempc = a_f_TEH_MARBUTA;
	    break;
	case a_TEH:
	    tempc = a_m_TEH;
	    break;
	case a_THEH:
	    tempc = a_m_THEH;
	    break;
	case a_JEEM:
	    tempc = a_m_JEEM;
	    break;
	case a_HAH:
	    tempc = a_m_HAH;
	    break;
	case a_KHAH:
	    tempc = a_m_KHAH;
	    break;
	case a_DAL:			/* exception */
	    tempc = a_f_DAL;
	    break;
	case a_THAL:			/* exception */
	    tempc = a_f_THAL;
	    break;
	case a_REH:			/* exception */
	    tempc = a_f_REH;
	    break;
	case a_ZAIN:			/* exception */
	    tempc = a_f_ZAIN;
	    break;
	case a_SEEN:
	    tempc = a_m_SEEN;
	    break;
	case a_SHEEN:
	    tempc = a_m_SHEEN;
	    break;
	case a_SAD:
	    tempc = a_m_SAD;
	    break;
	case a_DAD:
	    tempc = a_m_DAD;
	    break;
	case a_TAH:
	    tempc = a_m_TAH;
	    break;
	case a_ZAH:
	    tempc = a_m_ZAH;
	    break;
	case a_AIN:
	    tempc = a_m_AIN;
	    break;
	case a_GHAIN:
	    tempc = a_m_GHAIN;
	    break;
	case a_TATWEEL:			/* exception */
	    tempc = cur_c;
	    break;
	case a_FEH:
	    tempc = a_m_FEH;
	    break;
	case a_QAF:
	    tempc = a_m_QAF;
	    break;
	case a_KAF:
	    tempc = a_m_KAF;
	    break;
	case a_LAM:
	    tempc = a_m_LAM;
	    break;
	case a_MEEM:
	    tempc = a_m_MEEM;
	    break;
	case a_NOON:
	    tempc = a_m_NOON;
	    break;
	case a_HEH:
	    tempc = a_m_HEH;
	    break;
	case a_WAW:			/* exception */
	    tempc = a_f_WAW;
	    break;
	case a_ALEF_MAKSURA:		/* exception */
	    tempc = a_f_ALEF_MAKSURA;
	    break;
	case a_YEH:
	    tempc = a_m_YEH;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from ISO-8859-6/Isolated to final
 */
    static int
chg_c_a2f(cur_c)
    int cur_c;
{
    int tempc;

    /* NOTE: these encodings need to be accounted for

	a_f_ALEF_MADDA;
	a_f_ALEF_HAMZA_ABOVE;
	a_f_ALEF_HAMZA_BELOW;
	a_f_LAM_ALEF_MADDA_ABOVE;
	a_f_LAM_ALEF_HAMZA_ABOVE;
	a_f_LAM_ALEF_HAMZA_BELOW;
	*/

    switch (cur_c)
    {
	case a_HAMZA:			/* exception */
	    tempc = a_s_HAMZA;
	    break;
	case a_ALEF_MADDA:
	    tempc = a_f_ALEF_MADDA;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_ALEF_HAMZA_ABOVE;
	    break;
	case a_WAW_HAMZA:
	    tempc = a_f_WAW_HAMZA;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_f_ALEF_HAMZA_BELOW;
	    break;
	case a_YEH_HAMZA:
	    tempc = a_f_YEH_HAMZA;
	    break;
	case a_ALEF:
	    tempc = a_f_ALEF;
	    break;
	case a_BEH:
	    tempc = a_f_BEH;
	    break;
	case a_TEH_MARBUTA:
	    tempc = a_f_TEH_MARBUTA;
	    break;
	case a_TEH:
	    tempc = a_f_TEH;
	    break;
	case a_THEH:
	    tempc = a_f_THEH;
	    break;
	case a_JEEM:
	    tempc = a_f_JEEM;
	    break;
	case a_HAH:
	    tempc = a_f_HAH;
	    break;
	case a_KHAH:
	    tempc = a_f_KHAH;
	    break;
	case a_DAL:
	    tempc = a_f_DAL;
	    break;
	case a_THAL:
	    tempc = a_f_THAL;
	    break;
	case a_REH:
	    tempc = a_f_REH;
	    break;
	case a_ZAIN:
	    tempc = a_f_ZAIN;
	    break;
	case a_SEEN:
	    tempc = a_f_SEEN;
	    break;
	case a_SHEEN:
	    tempc = a_f_SHEEN;
	    break;
	case a_SAD:
	    tempc = a_f_SAD;
	    break;
	case a_DAD:
	    tempc = a_f_DAD;
	    break;
	case a_TAH:
	    tempc = a_f_TAH;
	    break;
	case a_ZAH:
	    tempc = a_f_ZAH;
	    break;
	case a_AIN:
	    tempc = a_f_AIN;
	    break;
	case a_GHAIN:
	    tempc = a_f_GHAIN;
	    break;
	case a_TATWEEL:			/* exception */
	    tempc = cur_c;
	    break;
	case a_FEH:
	    tempc = a_f_FEH;
	    break;
	case a_QAF:
	    tempc = a_f_QAF;
	    break;
	case a_KAF:
	    tempc = a_f_KAF;
	    break;
	case a_LAM:
	    tempc = a_f_LAM;
	    break;
	case a_MEEM:
	    tempc = a_f_MEEM;
	    break;
	case a_NOON:
	    tempc = a_f_NOON;
	    break;
	case a_HEH:
	    tempc = a_f_HEH;
	    break;
	case a_WAW:
	    tempc = a_f_WAW;
	    break;
	case a_ALEF_MAKSURA:
	    tempc = a_f_ALEF_MAKSURA;
	    break;
	case a_YEH:
	    tempc = a_f_YEH;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from Initial to Medial
 */
    static int
chg_c_i2m(cur_c)
    int cur_c;
{
    int tempc;

    switch (cur_c)
    {
	case a_i_YEH_HAMZA:
	    tempc = a_m_YEH_HAMZA;
	    break;
	case a_i_BEH:
	    tempc = a_m_BEH;
	    break;
	case a_i_TEH:
	    tempc = a_m_TEH;
	    break;
	case a_i_THEH:
	    tempc = a_m_THEH;
	    break;
	case a_i_JEEM:
	    tempc = a_m_JEEM;
	    break;
	case a_i_HAH:
	    tempc = a_m_HAH;
	    break;
	case a_i_KHAH:
	    tempc = a_m_KHAH;
	    break;
	case a_i_SEEN:
	    tempc = a_m_SEEN;
	    break;
	case a_i_SHEEN:
	    tempc = a_m_SHEEN;
	    break;
	case a_i_SAD:
	    tempc = a_m_SAD;
	    break;
	case a_i_DAD:
	    tempc = a_m_DAD;
	    break;
	case a_i_TAH:
	    tempc = a_m_TAH;
	    break;
	case a_i_ZAH:
	    tempc = a_m_ZAH;
	    break;
	case a_i_AIN:
	    tempc = a_m_AIN;
	    break;
	case a_i_GHAIN:
	    tempc = a_m_GHAIN;
	    break;
	case a_i_FEH:
	    tempc = a_m_FEH;
	    break;
	case a_i_QAF:
	    tempc = a_m_QAF;
	    break;
	case a_i_KAF:
	    tempc = a_m_KAF;
	    break;
	case a_i_LAM:
	    tempc = a_m_LAM;
	    break;
	case a_i_MEEM:
	    tempc = a_m_MEEM;
	    break;
	case a_i_NOON:
	    tempc = a_m_NOON;
	    break;
	case a_i_HEH:
	    tempc = a_m_HEH;
	    break;
	case a_i_YEH:
	    tempc = a_m_YEH;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from Final to Medial
 */
    static int
chg_c_f2m(cur_c)
    int cur_c;
{
    int tempc;

    switch (cur_c)
    {
	/* NOTE: these encodings are multi-positional, no ?
	   case a_f_ALEF_MADDA:
	   case a_f_ALEF_HAMZA_ABOVE:
	   case a_f_ALEF_HAMZA_BELOW:
	   */
	case a_f_YEH_HAMZA:
	    tempc = a_m_YEH_HAMZA;
	    break;
	case a_f_WAW_HAMZA:		/* exceptions */
	case a_f_ALEF:
	case a_f_TEH_MARBUTA:
	case a_f_DAL:
	case a_f_THAL:
	case a_f_REH:
	case a_f_ZAIN:
	case a_f_WAW:
	case a_f_ALEF_MAKSURA:
	    tempc = cur_c;
	    break;
	case a_f_BEH:
	    tempc = a_m_BEH;
	    break;
	case a_f_TEH:
	    tempc = a_m_TEH;
	    break;
	case a_f_THEH:
	    tempc = a_m_THEH;
	    break;
	case a_f_JEEM:
	    tempc = a_m_JEEM;
	    break;
	case a_f_HAH:
	    tempc = a_m_HAH;
	    break;
	case a_f_KHAH:
	    tempc = a_m_KHAH;
	    break;
	case a_f_SEEN:
	    tempc = a_m_SEEN;
	    break;
	case a_f_SHEEN:
	    tempc = a_m_SHEEN;
	    break;
	case a_f_SAD:
	    tempc = a_m_SAD;
	    break;
	case a_f_DAD:
	    tempc = a_m_DAD;
	    break;
	case a_f_TAH:
	    tempc = a_m_TAH;
	    break;
	case a_f_ZAH:
	    tempc = a_m_ZAH;
	    break;
	case a_f_AIN:
	    tempc = a_m_AIN;
	    break;
	case a_f_GHAIN:
	    tempc = a_m_GHAIN;
	    break;
	case a_f_FEH:
	    tempc = a_m_FEH;
	    break;
	case a_f_QAF:
	    tempc = a_m_QAF;
	    break;
	case a_f_KAF:
	    tempc = a_m_KAF;
	    break;
	case a_f_LAM:
	    tempc = a_m_LAM;
	    break;
	case a_f_MEEM:
	    tempc = a_m_MEEM;
	    break;
	case a_f_NOON:
	    tempc = a_m_NOON;
	    break;
	case a_f_HEH:
	    tempc = a_m_HEH;
	    break;
	case a_f_YEH:
	    tempc = a_m_YEH;
	    break;
	    /* NOTE: these encodings are multi-positional, no ?
		case a_f_LAM_ALEF_MADDA_ABOVE:
		case a_f_LAM_ALEF_HAMZA_ABOVE:
		case a_f_LAM_ALEF_HAMZA_BELOW:
		case a_f_LAM_ALEF:
		*/
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    static int
chg_c_laa2i(hid_c)
    int hid_c;
{
    int tempc;

    switch (hid_c)
    {
	case a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_s_LAM_ALEF_HAMZA_BELOW;
	    break;
	case a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Change shape - from Combination-Isolated to Final
 */
    static int
chg_c_laa2f(hid_c)
    int hid_c;
{
    int tempc;

    switch (hid_c)
    {
	case a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_f_LAM_ALEF_HAMZA_BELOW;
	    break;
	case a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    break;
	default:
	    tempc = 0;
    }

    return tempc;
}


/*
 * Redo the contents of what's displayed
 */
    static void
A_redo_shape(loc, cur_c)
    int loc;
    int cur_c;
{
    char_u	buf[MB_MAXBYTES + 1];

    /* Store actual unicode encoding */
    ScreenLinesUC[loc] = cur_c;

    /* Specify first byte of a multi-byte char */
    (*mb_char2bytes)(cur_c, buf);

    ScreenLines[loc] = buf[0];
}


/*
 * All the calls for the shaping functions go here.
 */
    int
arabic_shape(offset, c)
    int offset;
    int c;
{
    int		off_prev;
    int		off_next;
    int		curr_c;
    int		prev_c;
    int		next_c;
    int		shape_c;
    int		c1 = 0;
    int		c2 = 0;
    char_u	fub[MB_MAXBYTES + 1];
    int		curr_laa;
    int		prev_laa;

    /* Deal only with Arabic character, pass back all others */
    if (!A_is_ok(c))
	return c;

    /* Save away current character */
    curr_c = c;

    /* Deal with ":set rightleft" orientation
     * - Obtain the previous and next offsets */
    if (p_ri)
    {
	off_prev = offset - 1;
	off_next = offset + 1;
    }
    else
    {
	off_prev = offset + 1;
	off_next = offset - 1;
    }

    /* Get 'char' at previous offset */
    fub[utfc_char2bytes(off_prev, fub)] = NUL;
    prev_c = utfc_ptr2char(fub, &c1, &c2);

    /* Get 'char' at next     offset */
    fub[utfc_char2bytes(off_next, fub)] = NUL;
    next_c = utfc_ptr2char(fub, &c1, &c2);

    /* >>- Shape PREVIOUS character -<< */
    shape_c = 0;
    if (A_is_a(prev_c))
	shape_c = chg_c_a2i(prev_c);
    else if (A_is_valid(prev_c) && A_is_f(prev_c))
	shape_c = chg_c_f2m(prev_c);

    /* Do the actual re-shaping on previous character */
    if (A_is_formb(shape_c))
	A_redo_shape(off_prev, shape_c);

    /* >>- Shape CURRENT  character -<< */
    curr_laa = A_firstc_laa(offset, c);
    prev_laa = A_firstc_laa(off_prev, prev_c);

    if (curr_laa)
    {
	if (A_is_valid(prev_c) && !A_is_f(shape_c)
					 && !A_is_s(shape_c) && !prev_laa)
	    curr_c = chg_c_laa2f(curr_laa);
	else
	    curr_c = chg_c_laa2i(curr_laa);

	/* Nullify the hidden char */
	ScreenLinesC1[offset] = 0;
    }
    else if (!A_is_valid(prev_c) && A_is_valid(next_c))
	curr_c = chg_c_a2i(c);
    else if (!shape_c || A_is_f(shape_c) || A_is_s(shape_c) || prev_laa)
	curr_c = A_is_valid(next_c) ? chg_c_a2i(c) : chg_c_a2s(c);
    else if (A_is_valid(next_c))
	curr_c = A_is_iso(c)        ? chg_c_a2m(c) : chg_c_i2m(c);
    else if (A_is_valid(prev_c))
	curr_c = chg_c_a2f(c);
    else
	curr_c = chg_c_a2s(c);

    /* Sanity check -- curr_c should, in the future, never be 0.
     * We should, in the future, insert a fatal error here. */
    if (!curr_c)
	curr_c = c;

    /* Do the actual re-shaping on current character */
    if (A_is_formb(curr_c))
	A_redo_shape(offset, curr_c);

    return curr_c;
}


/*
 * A_firstc_laa returns first character of LAA combination if it exists
 */
    static int
A_firstc_laa(offset, c)
    int offset;
    int c;
{
    char_u	fub[MB_MAXBYTES + 1];
    int		temp_c;
    int		c1 = 0;
    int		c2 = 0;

    fub[utf_char2bytes(ScreenLinesC1[offset], fub)] = NUL;
    temp_c = utfc_ptr2char(fub, &c1, &c2);

    if (temp_c && !A_is_harakat(temp_c) && c == a_LAM)
	return temp_c;

    return 0;
}


/*
 * A_is_harakat returns TRUE if 'c' is an Arabic Harakat character
 *		(harakat/tanween)
 */
    static int
A_is_harakat(c)
    int c;
{
    return (c >= a_FATHATAN && c <= a_SUKUN);
}


/*
 * A_is_iso returns TRUE if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    static int
A_is_iso(c)
    int c;
{
    return ((c >= a_HAMZA && c <= a_GHAIN)
	    || (c >= a_TATWEEL && c <= a_HAMZA_BELOW)
	    || c == a_MINI_ALEF);
}


/*
 * A_is_formb returns TRUE if 'c' is an Arabic 10646-1 FormB character
 *		(alphabet/number/punctuation)
 */
    static int
A_is_formb(c)
    int c;
{
    return ((c >= a_s_FATHATAN && c <= a_s_DAMMATAN)
	    || c == a_s_KASRATAN
	    || (c >= a_s_FATHA && c <= a_f_LAM_ALEF)
	    || c == a_BYTE_ORDER_MARK);
}


/*
 * A_is_ok returns TRUE if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    static int
A_is_ok(c)
    int c;
{
    return (A_is_iso(c) || A_is_formb(c));
}


/*
 * A_is_valid returns TRUE if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    static int
A_is_valid(c)
    int c;
{
    return (A_is_ok(c) && !A_is_special(c));
}


/*
 * A_is_special returns TRUE if 'c' is not a special Arabic character.
 *		Specials don't adhere to most of the rules.
 */
    static int
A_is_special(c)
    int c;
{
    return (c == a_HAMZA || c == a_s_HAMZA);
}
