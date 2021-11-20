#include <stdio.h>
#include <string.h>
#include <kisstag.h>
#include <kisstag_png.h>
#include <kisstag_util.h>

int main()
{
    struct KTImage* image = KT_OpenImage("test.png");
    KT_SetImageTags(image, "1girl butt looking_down");
    printf("%d\n", KT_ImageHasAny(image, "butt"));
    KT_CloseImage(image);

    return 0;
}