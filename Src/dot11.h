#ifndef DOT11_H
#define DOT11_H
#include <stdint.h>
#include <stdio.h>
#include <list>
#include <string>
#include <iostream>
#include <memory>
#include <set>

using namespace std;

typedef enum frame_control_field_flag {
    TO_DS           = (0x01<<0),    // 1
    FROM_DS         = (0x01<<1),    // 2
    MORE_FRAGMENT   = (0x01<<2),    // 4
    RETRY           = (0x01<<3),    // 8
    POWER_MGMT      = (0x01<<4),    // 16  // power management
    MORE_DATA       = (0x01<<5),    // 32  // [a]
    PROTECTED_FRAME = (0x01<<6),    // 64
    ORDER_FLAG      = (0x01<<7)     // 128
} FCF_FLAG;

typedef enum frame_control_field_capabilities_info_flag {
    ESS                 = (0x01<<0),
    IBSS_FLAG           = (0x01<<1),
    CF_POLLABLE         = (0x01<<2),
    CF_POLL_REQUEST     = (0x01<<3),
    PRIVACY             = (0x01<<4),  // WEP FLAG
    SHORT_PREAMBLE      = (0x01<<5),
    PBCC                = (0x01<<6),
    CHANNEL_AGILITY     = (0x01<<7),
    SPECTRUM_MANAGEMENT = (0x01<<8),
    QOS                 = (0x01<<9),
    SHORT_SLOT_TIME     = (0x01<<10),
    APSD                = (0x01<<11),
    BOOK_FLAG           = (0x01<<12), // 예약 FLAG
    DSSS_OFDM           = (0x01<<13),
    DELAYED_BLOCK_ACK   = (0x01<<14),
    IMMEDIATE_BLOCK_ACK = (0x01<<15)
} FCF_CI_FLAG;

typedef enum frame_control_field_type {
    MANAGEMENT_FRAME, // 00
    CONTROL_FRAME,    // 01
    DATA_FRAME        // 11
} FCF_TYPE;

typedef enum fcf_management_frame_subtype {
    AID_REQUEST_FRAME       = 0,
    AID_RESPONSE_FRAME      = 1,
    AID_RE_REQUEST_FRAME    = 2,
    AID_RE_RESPONSE_FRAME   = 3,
    PROBE_REQUEST_FRAME     = 4,
    PROBE_RESPONSE_FRAME    = 5,
    BECONE_FRAME            = 8,
    ATIM_FRAME              = 9,
    DISASSOCIATION_FRAME    = 10,
    AUTHENTICATION_FRAME    = 11,
    DEAUTHENTICATION_FRAME  = 12
} FCF_MGM_SUBTYPE;

typedef enum fcf_control_frame_subtype {
    BEAMFORMING_REPORT_POLL = 4,
    VHT_NDP_ANNOUNCEMENT    = 5,
    CONTROL_WRAPPER         = 7,
    BAR                     = 8, // BLOCK_ACK_REQUEST
    BA                      = 9, // BLOCK_ACK
    PS_POLL                 = 10,
    RTS_FRAME               = 11,
    CTS_FRAME               = 12,
    ACK_FRAME               = 13,
    CF_END                  = 14,
    CF_END_ACT              = 15
} FCF_CTL_SUBTYPE;

typedef enum fcf_data_frame_subtype {
    DATA                    = 0,
    DATA_CF_ACK             = 1,
    DATA_CF_POLL            = 2,
    DATA_CF_ACK_POLL        = 3,
    NULL_DATA               = 4, // no data
    CF_ACK                  = 5, // no data
    CF_POLL                 = 6, // no data
    CF_ACK_POLL             = 7, // no data

    // QoS Data Frame
    QOS_DATA                = 8,
    QOS_DATA_CF_ACK         = 9,
    QOS_DATA_CF_POLL        = 10,
    QOS_DATA_CF_ACK_POLL    = 11,
    QOS_NULL_DATA           = 12, // no data
    QOS_CF_POLL             = 14, // no data
    QOS_CF_ACK_POLL         = 15, // no data
} FCF_DATA_SUBTYPE;

typedef enum tag_number{
    SSID                     = 0,
    DSSS_PARAMETER_SET       = 2,
    DS                       = 3,
    CF                       = 4,
    TIM                      = 5,
    IBSS                     = 6,
    COUNTRY_CODE             = 7,
    POWER_CONSTRAINT         = 32,
    IBSS_DFS                 = 41,
    EXTENDED_SUPPORTED_RATES = 50,
    RSN_IE                   = 48,
    QOS_CAPABILITY           = 46,
    HT_CAPABILITIES          = 45,
    HT_OPERATION             = 61,
    EXTENDED_CAPABILITIES    = 127,
    VENDOR_SPECIFIC_ELEMENT  = 221  // 항상 태그의 마지막
}TAG_NUMBER;

typedef struct ieee80211_radiotap_header {
    uint8_t  it_version;     /* set to 0 */
    uint8_t  it_pad;
    unsigned short it_len;   /* entire length */
    uint32_t it_present;     /* fields present */ // 여러개 있다..
} DOT11_HEADER;

typedef struct frame_control_field{ // 2byte
    uint8_t version; // static : 0, 2bit
    uint8_t type;    // frame type, 2bit
    uint8_t subtype; // sub type  , 4bit
    uint8_t flag;    // FCF_FLAG  , 8bit
} FCF;

typedef struct management_frame{
    FCF      fcf;          // frame_control_field
    uint16_t duration;
    uint8_t  dest_addr[6];
    uint8_t  src_addr[6];
    uint8_t  BSSID[6];
    uint16_t  number;       // sequence number(12bit) + fragment number(4bit)
} MGM_FRAME_STRUCT;

typedef struct tag_struct {
    uint8_t tag_number;
    uint8_t tag_length = 0;
    u_char * data;
    /*
    u_char * data(tag_struct * current_tag){ // data addr return // legacy code
        u_char * current_tag_data = (u_char *)current_tag + 2;
        return current_tag_data;
    }
    */
} TAG_STRUCT;

typedef struct management_frame_beacon_frame { // size : 0x18
    uint64_t         timestamp;
    uint16_t         beacon_interval;
    uint16_t         capa_info;   // capabilities information // FCF_CI_FLAG
    TAG_STRUCT       SSID;
    // list<TAG_STRUCT> option_tags;
} MGM_BEACON_FRAME;

typedef struct data_frame {
    FCF       fcf;          // frame_control_field
    uint16_t  duration;
    uint8_t   dest_addr[6];
    uint8_t   BSSID[6];
    uint8_t   src_addr[6];
    uint16_t  number;       // sequence number(12bit) + fragment number(4bit)

    //uint64_t  CCMP;         // 있을 수도 있고 없을 수도 있음
    //uint16_t  qos_control;  // 있을 수도 있고 없을 수도 있음
    //uint32_t  HT_control;   // 있을 수도 있고 없을 수도 있음

    u_char * frame_body;
} DATA_FRAME_STUCT;

typedef struct ap_information{
    uint8_t            BSSID[6];
    unsigned long long beacon_count = 1;   // beacons
    unsigned long long data_count = 0;     // #DATA
    uint8_t            ENC;            // ENC
    string             SSID;
} AP;

class Dot11
{
private:
    DOT11_HEADER     * dot11_header     = new DOT11_HEADER;
    MGM_FRAME_STRUCT * mgm_frame_struct = new MGM_FRAME_STRUCT;
    MGM_BEACON_FRAME * mgm_beacon_frame = new MGM_BEACON_FRAME;

    DATA_FRAME_STUCT * data_frame_struct = new DATA_FRAME_STUCT;

    const u_char * packet;

    list<AP *> AP_list;

    int get();
    void get_dot11_header();
    void get_mgm_frame_struct(FCF fcf, uint8_t * start_addr);
    void get_mgm_beacon_frame();
    void get_data_frame_struct(FCF fcf, uint8_t * start_addr);
    void push_tags(u_char * tag_start_addr);
    void print_BSSID(u_char * BSSID);
public:
    Dot11();
    Dot11(const u_char * packet);
    ~Dot11();
    void set(const u_char * packet);
    void save_info(int flag);
    void reset();
    void print();     // test
    void set_screen();
    string print_ESSID(u_char *data_addr, unsigned int len);
};


#endif // DOT11_H
