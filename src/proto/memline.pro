/* memline.c */
int ml_open __PARMS((struct buffer *buf));
void ml_close __PARMS((struct buffer *buf));
void ml_close_all __PARMS((void));
void ml_recover __PARMS((void));
void ml_sync_all __PARMS((int check_file));
void ml_preserve __PARMS((struct buffer *buf, int message));
unsigned char *ml_get __PARMS((long lnum));
unsigned char *ml_get_pos __PARMS((struct fpos *pos));
unsigned char *ml_get_cursor __PARMS((void));
unsigned char *ml_get_buf __PARMS((struct buffer *buf, long lnum, int will_change));
int ml_line_alloced __PARMS((void));
int ml_append __PARMS((long lnum, unsigned char *line, unsigned int len, int newfile));
int ml_replace __PARMS((long lnum, unsigned char *line, int copy));
int ml_delete __PARMS((long lnum));
void ml_setmarked __PARMS((long lnum));
long ml_firstmarked __PARMS((void));
int ml_has_mark __PARMS((long lnum));
void ml_clearmarked __PARMS((void));
