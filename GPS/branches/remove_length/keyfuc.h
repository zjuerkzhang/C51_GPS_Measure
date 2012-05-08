 #ifndef _keyfuc_h_
#define _keyfuc_h_

#define CELIANG_DETAIL_PAGE  0
#define CELIANG_WORKING_PAGE 1
#define CELIANG_SN_PAGE      2

unsigned char keyscan();
void KeyOperate();
void wait_key_ok_release();
#endif