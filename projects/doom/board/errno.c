/* Newlib libm error slot, no heap, files or OS. */
static int math_errno;
int *__errno(void){return &math_errno;}
