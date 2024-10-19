#ifndef __USB_EP_CFG_H__
#define __USB_EP_CFG_H__

#define     EP0_SETUP_LEN           0x40

#ifndef INVALID
#define INVALID ((void *)-1)
#endif


///////////MassStorage Class

#define     BULK_EP_OUT                 USB_EP1
#define     BULK_EP_IN                  BULK_EP_OUT

// #define USB_FULL_SPEED

#ifdef USB_FULL_SPEED
#define     MAXP_SIZE_BULKOUT           64
#define     MAXP_SIZE_BULKIN            MAXP_SIZE_BULKOUT
#define     UAC_ISO_INTERVAL            1
#else
#define     MAXP_SIZE_BULKOUT           512
#define     MAXP_SIZE_BULKIN            MAXP_SIZE_BULKOUT
#define     UAC_ISO_INTERVAL            4
#endif


///////////MIC class

#define     MIC_ISO_EP_IN               USB_EP3

///////////SPEAKER class

#define     SPK_ISO_EP_OUT              USB_EP3


////////////HID Class

#define     HID_EP_IN                   USB_EP3
#define     HID_EP_OUT                  HID_EP_IN

#define     HID_FIFO_SIZE               64


///////////VIDEO class


#define     VIDEO_STATUS_EP_IN          USB_EP5


#define     VIDEO_STATUS_TXMAXP         64

#define     UVC_USER_ISO_PIPE           USB_EP2
#define     UVC_USER_INTR_PIPE          USB_EP6
#define     UVC_STREAM_EP_IN            USB_EP4
#define     UVC_H264_ISO_PIPE           USB_EP2
#define     UVC_ISO_MODE                0
#if UVC_ISO_MODE
#define     UVC_PKT_SPILT               1
#define     UVC_FIFO_TXMAXP             (1023)  //为了兼容usb1.1,设置为1023
#else
#define     UVC_PKT_SPILT               1
#define     UVC_FIFO_TXMAXP             (512)
#endif


////////////CDC Class
#define     CDC_DATA_EP_IN              USB_EP2
#define     CDC_DATA_EP_OUT             USB_EP2
#define     CDC_INTR_EP_IN              USB_EP6

#define     MAXP_SIZE_CDC_DATA          64
#define     MAXP_SIZE_CDC_INTRIN        64

//////////////////////////////////////////////////////////////////////
#endif
