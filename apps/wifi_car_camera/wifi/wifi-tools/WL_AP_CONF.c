
#include <string.h>
#include "generic/typedef.h"

int sprintf(char *out, const char *format, ...);
int printf(const char *format, ...);

#if 0
WirelessMode:
PHY_11BG_MIXED = 0,
PHY_11B = 1,
PHY_11A = 2,,
PHY_11ABG_MIXED = 3,,
PHY_11G = 4,
PHY_11ABGN_MIXED,	/* both band   5 */
PHY_11N_2_4G,		/* 11n-only with 2.4G band      6 */
PHY_11GN_MIXED,		/* 2.4G band      7 */
PHY_11AN_MIXED,		/* 5G  band       8 */
PHY_11BGN_MIXED,	/* if check 802.11b.      9 */
PHY_11AGN_MIXED,	/* if check 802.11b.      10 */
PHY_11N_5G,		/* 11n-only with 5G band                11 */


#endif

/*
 MaxStaNum  最大连接数不能超过 MAX_LEN_OF_MAC_TABLE(5)
 */
static char WLAP_DAT[] = {
    "\
#The word of \"Default\" must not be removed\n\
Default\n\
MacAddress=00:00:00:00:00:00\n\
CountryRegion=5\n\
CountryRegionABand=7\n\
CountryCode=TW\n\
BssidNum=1\n\
MaxStaNum=2\n\
SSID=####SSID_LENTH_MUST_LESS_THAN_32\n\
WirelessMode=9\n\
TxRate=0\n\
Channel=6#\n\
BasicRate=15\n\
BeaconPeriod=100\n\
DtimPeriod=1\n\
TxPower=100\n\
DisableOLBC=0\n\
BGProtection=0\n\
TxAntenna=\n\
RxAntenna=\n\
TxPreamble=0\n\
RTSThreshold=2347\n\
FragThreshold=2346\n\
TxBurst=1\n\
PktAggregate=0\n\
TurboRate=0\n\
WmmCapable=0\n\
APSDCapable=0\n\
DLSCapable=0\n\
APAifsn=3;7;1;1\n\
APCwmin=4;4;3;2\n\
APCwmax=6;10;4;3\n\
APTxop=0;0;94;47\n\
APACM=0;0;0;0\n\
BSSAifsn=3;7;2;2\n\
BSSCwmin=4;4;3;2\n\
BSSCwmax=10;10;4;3\n\
BSSTxop=0;0;94;47\n\
BSSACM=0;0;0;0\n\
AckPolicy=0;0;0;0\n\
NoForwarding=0\n\
NoForwardingBTNBSSID=0\n\
HideSSID=0\n\
StationKeepAlive=0\n\
ShortSlot=1\n\
AutoChannelSelect=0\n\
IEEE8021X=0\n\
IEEE80211H=0\n\
CSPeriod=10\n\
WirelessEvent=0\n\
IdsEnable=0\n\
AuthFloodThreshold=32\n\
AssocReqFloodThreshold=32\n\
ReassocReqFloodThreshold=32\n\
ProbeReqFloodThreshold=32\n\
DisassocFloodThreshold=32\n\
DeauthFloodThreshold=32\n\
EapReqFooldThreshold=32\n\
PreAuth=0\n\
AuthMode=################\n\
EncrypType=################\n\
RekeyInterval=0\n\
RekeyMethod=DISABLE\n\
PMKCachePeriod=10\n\
WPAPSK=#########wpa_passphrase_lenth_must_more_than_7_and_less_than_63\n\
DefaultKeyID=1\n\
Key1Type=0\n\
Key1Str=\n\
Key2Type=0\n\
Key2Str=\n\
Key3Type=0\n\
Key3Str=\n\
Key4Type=0\n\
Key4Str=\n\
HSCounter=0\n\
AccessPolicy0=0\n\
AccessControlList0=\n\
AccessPolicy1=0\n\
AccessControlList1=\n\
AccessPolicy2=0\n\
AccessControlList2=\n\
AccessPolicy3=0\n\
AccessControlList3=\n\
WdsEnable=0\n\
WdsEncrypType=NONE\n\
WdsList=\n\
WdsKey=\n\
RADIUS_Server=192.168.2.3\n\
RADIUS_Port=1812\n\
RADIUS_Key=ralink\n\
own_ip_addr=192.168.5.234\n\
EAPifname=br0\n\
PreAuthifname=br0\n\
HT_HTC=0\n\
HT_RDG=0\n\
HT_EXTCHA=0\n\
HT_LinkAdapt=0\n\
HT_OpMode=0\n\
HT_MpduDensity=5\n\
HT_BW=0\n\
HT_AutoBA=1\n\
HT_AMSDU=0\n\
HT_BAWinSize=64\n\
HT_GI=0\n\
HT_MCS=33\n\
MeshId=MESH\n\
MeshAutoLink=1\n\
MeshAuthMode=OPEN\n\
MeshEncrypType=NONE\n\
MeshWPAKEY=\n\
MeshDefaultkey=1\n\
MeshWEPKEY=\n\
WscManufacturer=\n\
WscModelName=\n\
WscDeviceName=\n\
WscModelNumber=\n\
WscSerialNumber=\n\
RadioOn=1\n\
PMFMFPC=0\n\
PMFMFPR=0\n\
PMFSHA256=0"
};

const char *GET_WL_AP_DAT(void)
{
    return WLAP_DAT;
}

int GET_WL_AP_DAT_LEN(void)
{
    return strlen(WLAP_DAT);
}



int wl_set_wifi_channel(int channel)
{
    char channel_str[3] = {0};

    if (channel < 1 || channel > 14) {
        return -1;
    }

    sprintf(channel_str, "%d", channel);

    const char *find_channel = "Channel=";
    char *channel_position = strstr(GET_WL_AP_DAT(), find_channel) + strlen(find_channel);

    strcpy(channel_position, channel_str);
    channel_position[strlen(channel_str)] = '\n';
    memset(channel_position + strlen(channel_str) + 1, '#', 3 - strlen(channel_str) - 1);

    return 0;
}

static int wl_set_ssid(const char *ssid)
{
    if (ssid == 0) {
        return -1;
    }

    const char *find_ssid = "SSID=";
    char *ssid_position = strstr(GET_WL_AP_DAT(), find_ssid) + strlen(find_ssid);

    if (strlen(ssid) > 32) {
        printf("set_ssid_passphrase fail,ssid len (0x%x) longer than 32!\n", strlen(ssid));
        return -1;
    }

    strcpy(ssid_position, ssid);
    ssid_position[strlen(ssid)] = '\n';
    memset(ssid_position + strlen(ssid) + 1, '#', 32 - strlen(ssid) - 1);

    return 0;
}

static int wl_set_passphrase(const char *passphrase)
{
    if (passphrase == 0) {
        return -1;
    }

    const char *AUTH_MODE, *ENCRYP_TYPE;
    const char *find_wpa_passphrase = "WPAPSK=";
    const char *find_AuthMode = "AuthMode=";
    const char *find_EncrypType = "EncrypType=";

    char *wpa_passphrase_position = strstr(GET_WL_AP_DAT(), find_wpa_passphrase) + strlen(find_wpa_passphrase);
    char *AuthMode_position = strstr(GET_WL_AP_DAT(), find_AuthMode) + strlen(find_AuthMode);
    char *EncrypType_position = strstr(GET_WL_AP_DAT(), find_EncrypType) + strlen(find_EncrypType);

    if (strcmp(passphrase, "")) {
        if ((strlen(passphrase) < 8) || (strlen(passphrase) > 63)) {
            printf("set_ssid_passphrase fail,passphrase len (0x%x) must more than 7 and less than 63!\n", strlen(passphrase));
            return -1;
        }

        strcpy(wpa_passphrase_position, passphrase);
        wpa_passphrase_position[strlen(passphrase)] = '\n';
        memset(wpa_passphrase_position + strlen(passphrase) + 1, '#', 63 - strlen(passphrase) - 1);

        AUTH_MODE = "WPA2PSK";
        strcpy(AuthMode_position, AUTH_MODE);
        AuthMode_position[strlen(AUTH_MODE)] = '\n';
        memset(AuthMode_position + strlen(AUTH_MODE) + 1, '#', 16 - strlen(AUTH_MODE) - 1);

        ENCRYP_TYPE = "AES";
        strcpy(EncrypType_position, ENCRYP_TYPE);
        EncrypType_position[strlen(ENCRYP_TYPE)] = '\n';
        memset(EncrypType_position + strlen(ENCRYP_TYPE) + 1, '#', 16 - strlen(ENCRYP_TYPE) - 1);
    } else {
        AUTH_MODE = "OPEN";
        strcpy(AuthMode_position, AUTH_MODE);
        AuthMode_position[strlen(AUTH_MODE)] = '\n';
        memset(AuthMode_position + strlen(AUTH_MODE) + 1, '#', 16 - strlen(AUTH_MODE) - 1);

        ENCRYP_TYPE = "NONE";
        strcpy(EncrypType_position, ENCRYP_TYPE);
        EncrypType_position[strlen(ENCRYP_TYPE)] = '\n';
        memset(EncrypType_position + strlen(ENCRYP_TYPE) + 1, '#', 16 - strlen(ENCRYP_TYPE) - 1);
    }

    return 0;
}



int wl_ap_init(const char *ssid, const char *passphrase)
{
    int ret;


    wl_set_ssid(ssid);
    wl_set_passphrase(passphrase);

//    printf("WLAP_DAT = \r\n %s \r\n",GET_WL_AP_DAT());

    return 0;
}


