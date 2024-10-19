
#include "system/includes.h"
#include "server/usb_server.h"
#include "app_config.h"
#include "action.h"

/* extern int set_usb_mass_storage(void); */
/* extern int set_usb_camera(void); */
extern int back_to_video_rec(void);
extern int usb_connect(u32 state);




int usb_app_set_config(struct intent *it)
{
    puts("usb_app_set_config.\n");
    printf("it data : %s.\n", it->data);
    if (!strcmp(it->data, "usb:msd")) {
        /* set_usb_mass_storage(); */
        usb_connect(USB_MASS_STORAGE);
        return 1;
    } else if (!strcmp(it->data, "usb:uvc")) {
        /* set_usb_camera(); */
        usb_connect(USB_CAMERA);
#ifdef PRODUCT_TEST_ENABLE
        u8 product_main(void);
        product_main();
#endif
        return 2;
    } else if (!strcmp(it->data, "usb:rec")) {
        back_to_video_rec();
        return 3;
    }
    return 0;
}

int usb_app_get_config(struct intent *it)
{
    puts("usb_app_get_config.\n");

    return 0;
}
