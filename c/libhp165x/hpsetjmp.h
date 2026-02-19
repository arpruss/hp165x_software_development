#ifndef _HP_SETJMP_H
#define _HP_SETJMP_H

typedef uint32_t hpjmp_buf[13];
int hpsetjmp(hpjmp_buf env);
void hplongjmp(hpjmp_buf env, int val);

#endif