#include <string.h>

#if 0
WirelessMode:
PHY_11BG_MIXED = 0,
PHY_11B,
PHY_11A,
PHY_11ABG_MIXED,
PHY_11G,
PHY_11ABGN_MIXED,	/* both band   5 */
PHY_11N_2_4G,		/* 11n-only with 2.4G band      6 */
PHY_11GN_MIXED,		/* 2.4G band      7 */
PHY_11AN_MIXED,		/* 5G  band       8 */
PHY_11BGN_MIXED,	/* if check 802.11b.      9 */
PHY_11AGN_MIXED,	/* if check 802.11b.      10 */
PHY_11N_5G,		/* 11n-only with 5G band                11 */


#endif

/*PSMode=CAM\n\*/
/*PSMode=Legacy_PSP\n\//STA休眠*/
#define WL_STA_SLEEP	0

static const char WL_STA_DAT[] = {
    "\
#The word of \"Default\" must not be removed\n\
Default\n\
MacAddress=00:00:00:00:00:00\n\
CountryRegion=5\n\
CountryRegionABand=7\n\
CountryCode=\n\
ChannelGeography=1\n\
SSID=DEFAULT_CONNECT_SSID\n\
NetworkType=Infra\n\
WirelessMode=9\n\
Channel=6\n\
BeaconPeriod=100\n\
TxPower=100\n\
BGProtection=0\n\
TxPreamble=0\n\
RTSThreshold=2347\n\
FragThreshold=2346\n\
TxBurst=1\n\
PktAggregate=0\n\
WmmCapable=1\n\
AckPolicy=0;0;0;0\n\
AuthMode=OPEN\n\
EncrypType=NONE\n\
WPAPSK=\n\
DefaultKeyID=1\n\
Key1Type=0\n\
Key1Str=\n\
Key2Type=0\n\
Key2Str=\n\
Key3Type=0\n\
Key3Str=\n\
Key4Type=0\n\
Key4Str=\n"
#if WL_STA_SLEEP
    "PSMode=Legacy_PSP\n"
#else
    "PSMode=CAM\n"
#endif
    "AutoRoaming=0\n\
RoamThreshold=70\n\
APSDCapable=1\n\
APSDAC=0;0;0;0\n\
HT_RDG=1\n\
HT_EXTCHA=0\n\
HT_OpMode=0\n\
HT_MpduDensity=4\n\
HT_BW=0\n\
HT_BADecline=0\n\
HT_AutoBA=1\n\
HT_AMSDU=0\n\
HT_BAWinSize=64\n\
HT_GI=0\n\
HT_MCS=33\n\
HT_MIMOPSMode=3\n\
HT_DisallowTKIP=1\n\
HT_STBC=0\n\
EthConvertMode=\n\
EthCloneMac=\n\
IEEE80211H=0\n\
TGnWifiTest=0\n\
WirelessEvent=0\n\
MeshId=MESH\n\
MeshAutoLink=1\n\
MeshAuthMode=OPEN\n\
MeshEncrypType=NONE\n\
MeshWPAKEY=\n\
MeshDefaultkey=1\n\
MeshWEPKEY=\n\
CarrierDetect=0\n\
AntDiversity=0\n\
BeaconLostTime=16\n\
FtSupport=0\n\
Wapiifname=ra0\n\
WapiPsk=\n\
WapiPskType=\n\
WapiUserCertPath=\n\
WapiAsCertPath=\n\
PSP_XLINK_MODE=0\n\
WscManufacturer=\n\
WscModelName=\n\
WscDeviceName=\n\
WscModelNumber=\n\
WscSerialNumber=\n\
RadioOn=1\n\
WIDIEnable=0\n\
P2P_L2SD_SCAN_TOGGLE=8\n\
Wsc4digitPinCode=0\n\
P2P_WIDIEnable=0\n\
PMFMFPC=0\n\
PMFMFPR=0\n\
PMFSHA256=0"
};

const char *GET_WL_STA_DAT(void)
{
    return WL_STA_DAT;
}

int GET_WL_STA_DAT_LEN(void)
{
    return strlen(WL_STA_DAT);
}

#if 1
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;
const u8 RTDebugLevel = 2;
const char WL_TX_DEBUG = 0; //WIFI底层发送数据FIFO繁忙打印
const char WL_RX_DEBUG = 0; //WIFI底层接收FIFO塞满导致丢包打印
const char WL_TX_ERR_RATIO_DEBUG_SEC = 0; //统计每秒TX数据包成功/失败/重发和错误率的情况,配置每隔多少秒打印一次
const char WL_RX_ERR_RATIO_DEBUG_SEC = 0; //统计每秒RX数据包成功/失败/和错误率的情况,配置每隔多少秒打印一次
const u16 WL_RX_BACK_GROUND_ERR_CNT_PER_SECOND = 10; //硬件少了寄存器统计接收正确包,所以统计正确包和误报率不准,人工设定一个当前环境干扰每秒钟的本底错误包作为弥补,一开始需要先观察打印确认
const u8 WL_TX_PEND_DEBUG_SEC = 2; //WIFI底层FIFO塞满导致连续多少秒都发送不出数据时打印, 一般认为是干扰严重/wifi板TX性能差/CPU被挡等因素导致
const char WL_RX_PEND_DEBUG_SEC = 2; //统计WIFI底层连续多少秒都接收不到空中包时打印,一般认为是进了屏蔽房/加了MAC层过滤/板子硬件性能太差/CPU太繁忙来接收线程来不及取数因素导致
const char WL_RX_OVERFLOW_DEBUG = 0; //统计WIFI底层接收FIFO塞满导致丢包打印,一般认为对端发送太猛/空中干扰太强/CPU太繁忙来接收线程来不及取数因素导致, 使能后如果出现丢包打印每秒丢多少个数据包
const u8 RxReorderEnable = 1; //底层包乱序整理，0为关闭(关闭时UDP重发包也会上传到上层, 但关闭可以减少内存消耗)，1为开启
const u32 MAX_REORDERING_MPDU_NUM = 128; //当RxReorderEnable开启时需要内存较大，该值也需要根据实际设置大一点(128)
const u16 MAX_CHANNEL_TIME_BSS_INFRA = 200;//扫描每个信道停留时间,单位ms,最小20ms, 200-400ms最佳
const char WIFI_CHANNEL_QUALITY_INDICATION_BAD = 5; //STA模式下的信道通信质量差阈值,一旦低于这个值就断线重连,如果配置为-1则信号质量再差也不通知断线,但是太久不重连会被路由器认为死亡踢掉的风险
const char wifi_temperature_drift_trim_on = 0; //WiFi温度漂移校准开关,0为关闭，1为打开
const char wifi_ap_scan_support = 0; //ap扫描开关，0为关闭，1为开启
const u16 MAX_PACKETS_IN_QUEUE = 128; //配置WiFi驱动最大发送数据包队列
const u16 MAX_PACKETS_IN_MCAST_PS_QUEUE = 32;  //配置WiFi驱动最大发送MCAST-power-save包队列 //modify by lyx 32
const u16 MAX_PACKETS_IN_PS_QUEUE	= 128; //配置WiFi驱动最大发送power-save队列	//128	/*16 */
int get_bss_table_record(char *data, u32 len)
{
    puts("[err]get_bss_table_record");
    return 0;
}
void set_bss_table_record(char *data, u32 len)
{
    puts("[err]set_bss_table_record");
}
void wifi_set_target_mac_for_agc_recv(char *mac)
{
    puts("[err]wifi_set_target_mac_for_agc_recv");
}
void wifi_module_scan_completed_notify(void)
{
    puts("[err]wifi_module_scan_completed_notify");
}
int init_net_device_mac_addr(char *macaddr, char ap_mode)
{
    while (1) {
        printf("please init_net_device_mac_addr!!!");
        os_time_dly(30);
    }
    return 0;
}
#endif
