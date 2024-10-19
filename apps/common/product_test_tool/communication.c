#include "product_main.h"

#ifdef PRODUCT_TEST_ENABLE

#define CDC_BUF_LEN (4*1024)

static OS_SEM cdc_sem;
static u8 product_usb_id = 0;
static u8 *cdc_buf = NULL;
static cbuffer_t cbuf;

u8 product_tool_cdc_post(void)
{
    if (is_product_mode()) {
        os_sem_post(&cdc_sem);
    }
    return is_product_mode();
}

static int cdc_user_output(u8 *buf, u32 len)
{
    u32 wlen;
    /* puts("cdc output\n"); */
    /* put_buf(buf, len); */
    wlen = cbuf_write(&cbuf, buf, len);
    if (wlen != len) {
        log_e("cbuf full");
    }

    os_sem_post(&cdc_sem);
    return len;
}

static s8 comm_dev_init(void)
{
    cdc_buf = malloc(CDC_BUF_LEN);
    if (cdc_buf) {
        cbuf_init(&cbuf, cdc_buf, CDC_BUF_LEN);
        int set_usb_cdc(int (*output)(u8 * obuf, u32 olen));
        set_usb_cdc(cdc_user_output);
        /* int usb_connect(u32 state); */
        /* usb_connect(USB_CDC); */
        os_sem_create(&cdc_sem, 0);
        return 0;
    } else {
        log_e("cdc buf malloc err\n");
        return -1;
    }
}


static s8 comm_dev_online(void)
{
    int usb_device_ready();
    return usb_device_ready();
}


static s32 comm_dev_read(u8 *data, u32 size)
{
    os_sem_pend(&cdc_sem, 0);
    os_sem_set(&cdc_sem, 0);

    u32 dlen = cbuf_get_data_size(&cbuf);


    if (dlen == 0) {
        return 0;
    }

    if (dlen <= size) {
        size = dlen;
    } else {
        os_sem_post(&cdc_sem);
    }

    return cbuf_read(&cbuf, data, size);
}


static s32 comm_dev_write(u8 *data, u32 size)
{
    u8 err = 0;
    int cdc_user_input(u8 * buf, u32 len);
    err = cdc_user_input(data, size);

    if (err) {
        return 0;
    } else {
        return size;
    }
}


static prod_comm_type comm = {
    .init   = comm_dev_init,
    .online = comm_dev_online,
    .read   = comm_dev_read,
    .write  = comm_dev_write,
};



prod_comm_type *comm_ops(void)
{
    return &comm;
}


#endif


