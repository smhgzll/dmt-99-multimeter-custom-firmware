#include "resource_volume.h"
/* ddb0: an open success counts as a logo, even if the JPEG is invalid. */
bool resource_optional_logo(dmt_resources *r,FATFS *fs,FRESULT *status)
{
 *status=filesystem_mount(fs,0);
 bool failed=true;
 if(*status==FR_OK)failed=resource_draw_jpeg(r,"0:/LOGO.jpg",0,0)!=0;
 *status=filesystem_mount(fs,1);
 return failed;
}
/* de12: write protection is enforced by filesystem_bind's capability. */
void resource_remove_logo(FATFS *fs,FRESULT *status)
{
 *status=filesystem_mount(fs,0);
 if(*status==FR_OK)(void)f_unlink("0:/LOGO.jpg");
 *status=filesystem_mount(fs,1);
}
