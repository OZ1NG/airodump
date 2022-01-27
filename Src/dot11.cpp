#include "dot11.h"
#include <cstring>

int Dot11::get()
{
    this->get_dot11_header(); // get dot11 header length

    uint8_t * tmp_fcf_addr = (uint8_t *)(this->packet + this->dot11_header->it_len);
    FCF tmp_fcf = {
        static_cast<uint8_t>(*tmp_fcf_addr & 3),          // version // 00000011
        static_cast<uint8_t>((*tmp_fcf_addr & 12) >> 2),  // type    // 00001100
        static_cast<uint8_t>(*tmp_fcf_addr >> 4),         // subtype
        *(tmp_fcf_addr + 1)                               // flags
    };

    if((tmp_fcf.type == MANAGEMENT_FRAME) && (tmp_fcf.subtype == BECONE_FRAME)){
        this->get_mgm_frame_struct(tmp_fcf, tmp_fcf_addr);
        this->get_mgm_beacon_frame();
        return 1;
    } else if ((tmp_fcf.type == DATA_FRAME) && ((tmp_fcf.subtype == DATA) || (tmp_fcf.subtype == DATA_CF_ACK) || (tmp_fcf.subtype == DATA_CF_POLL) || (tmp_fcf.subtype == DATA_CF_ACK_POLL) || (tmp_fcf.subtype == QOS_DATA) || (tmp_fcf.subtype == QOS_DATA_CF_ACK) || (tmp_fcf.subtype == QOS_DATA_CF_POLL) || (tmp_fcf.subtype == QOS_DATA_CF_ACK_POLL))) {
        this->get_data_frame_struct(tmp_fcf, tmp_fcf_addr);
        return 2;
    } else
        return 0;
}

void Dot11::get_dot11_header()
{
    this->dot11_header = (DOT11_HEADER *) packet;
}

void Dot11::get_mgm_frame_struct(FCF fcf, uint8_t * start_addr)
{
    memcpy(&this->mgm_frame_struct->fcf, &fcf, sizeof(FCF));
    memcpy(&this->mgm_frame_struct->duration, start_addr+2, sizeof(MGM_FRAME_STRUCT)-sizeof(FCF));
}

void Dot11::get_mgm_beacon_frame()
{
    u_char * mgm_beacon_frame_addr = (u_char *)(this->packet + this->dot11_header->it_len + sizeof(MGM_FRAME_STRUCT) - 2); // -2 : 늘어난 FCF 크기를 다시 빼준 것
    memcpy(&this->mgm_beacon_frame->timestamp, mgm_beacon_frame_addr, sizeof(uint64_t));
    memcpy(&this->mgm_beacon_frame->beacon_interval, mgm_beacon_frame_addr+sizeof(uint64_t), sizeof(uint16_t));
    memcpy(&this->mgm_beacon_frame->capa_info, mgm_beacon_frame_addr+sizeof(uint64_t)+sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&this->mgm_beacon_frame->SSID, mgm_beacon_frame_addr+sizeof(uint64_t)+sizeof(uint16_t)+sizeof(uint16_t), 2); // 2 == sizeof(TAG_STRUCT.tag_number) + sizeof(TAG_STRUCT.tag_length)
    this->mgm_beacon_frame->SSID.data = mgm_beacon_frame_addr+sizeof(uint64_t)+sizeof(uint16_t)+sizeof(uint16_t)+2;
}

void Dot11::get_data_frame_struct(FCF fcf, uint8_t *start_addr)
{
    memcpy(&this->data_frame_struct->fcf, &fcf, sizeof(FCF));
    memcpy(&this->data_frame_struct->duration, start_addr+2, sizeof(DATA_FRAME_STUCT)-sizeof(FCF));
}

Dot11::Dot11()
{

}

/*
void Dot11::push_tags(u_char * tag_start_addr)
{
    // VENDOR_SPECIFIC_ELEMENT;
    unsigned int idx = 0;
    while(true){
        u_char * tmp = (TAG_STRUCT * )tag_start_addr + idx;
        this->mgm_beacon_frame->option_tags.push_back(tmp);
    }
}
*/

Dot11::Dot11(const u_char * packet)
{
    this->packet = packet;
    int flag = this->get();
    if(flag){
        this->save_info(flag);
        this->set_screen();
        // this->print(); // test
    }
}

Dot11::~Dot11()
{
    memset(this->dot11_header, 0, sizeof(DOT11_HEADER));
    delete this->dot11_header;
    memset(this->mgm_frame_struct, 0, sizeof(MGM_FRAME_STRUCT));
    delete this->mgm_frame_struct;
    memset(this->mgm_beacon_frame, 0, sizeof(MGM_BEACON_FRAME));
    delete this->mgm_beacon_frame;
}

void Dot11::set(const u_char *packet)
{
    this->packet = packet;
    int flag = this->get();
    if(flag){
        this->save_info(flag);
        this->set_screen();
        // this->print(); // test
    }
}

void Dot11::save_info(int flag)
{
    bool loop_flag = true;
    u_char * BSSID;
    if(flag == 1)
        BSSID = this->mgm_frame_struct->BSSID;
    else
        BSSID = this->data_frame_struct->BSSID;
    for (AP *ap_ptr : this->AP_list){
        if(!memcmp(ap_ptr->BSSID, BSSID, 6)){
            if(flag == 1) // beacon
                ap_ptr->beacon_count += 1;
            else          // data
                ap_ptr->data_count   += 1;

            loop_flag = false;
            break;
        }
    }
    if(loop_flag){ // new AP!
        AP * tmp = new AP;
        memcpy(tmp->BSSID, BSSID, 6);
        tmp->ENC = this->mgm_beacon_frame->capa_info && PRIVACY;
        if(flag == 1)
            tmp->beacon_count += 1;
        else
            tmp->data_count = 0;

        tmp->SSID = this->print_ESSID(this->mgm_beacon_frame->SSID.data, (unsigned int)this->mgm_beacon_frame->SSID.tag_length);
        this->AP_list.push_back(tmp);
    }
}

void Dot11::reset()
{
    memset(this->dot11_header, 0, sizeof(DOT11_HEADER));
    //delete this->dot11_header;
    memset(this->mgm_frame_struct, 0, sizeof(MGM_FRAME_STRUCT));
    //delete this->mgm_frame_struct;
    memset(this->mgm_beacon_frame, 0, sizeof(MGM_BEACON_FRAME));
    //delete this->mgm_beacon_frame;
}

void Dot11::print() // test
{
    cout << "===========================================" << endl;
    cout << "[-] BSSID   : "; this->print_BSSID(this->mgm_frame_struct->BSSID); puts("");
    cout << "[-] Beacons : " << 1 << endl;
    cout << "[-] #DATA   : " << "TEST" << endl;
    if(this->mgm_beacon_frame->capa_info && PRIVACY)
        cout << "[-] ENC     : WEP" << endl;
    else
        cout << "[-] ENC     : OPN" << endl;
    //cout << "[-] ESSID   : " << this->print_ESSID( this->mgm_beacon_frame->SSID.data(&this->mgm_beacon_frame->SSID), (unsigned int)this->mgm_beacon_frame->SSID.tag_length) << endl;
    cout << "[-] ESSID   : " << this->print_ESSID(this->mgm_beacon_frame->SSID.data, (unsigned int)this->mgm_beacon_frame->SSID.tag_length) << endl;
    cout << "===========================================" << endl;
}

void Dot11::set_screen()
{
    printf("\e[1;1H\e[2J"); // clear
    printf("====================================================================\n");
    printf("BSSID               Beacons    #DATA    ENC   ESSID\n");
    for (AP *ap_ptr : this->AP_list){
        this->print_BSSID(ap_ptr->BSSID);
        if(ap_ptr->ENC)
            printf("  %8llu %8llu    WEP   %s\n", ap_ptr->beacon_count, ap_ptr->data_count, ap_ptr->SSID.c_str());
        else
            printf("  %8llu %8llu    OPN   %s\n", ap_ptr->beacon_count, ap_ptr->data_count, ap_ptr->SSID.c_str());
    }
    printf("====================================================================\n");
}

void Dot11::print_BSSID(u_char * BSSID)
{
    printf("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            BSSID[0],
            BSSID[1],
            BSSID[2],
            BSSID[3],
            BSSID[4],
            BSSID[5]
            );
}

string Dot11::print_ESSID(u_char *data_addr, unsigned int len)
{
    string essid = "";
    for(unsigned int i = 0 ; i < len; i++){
        essid += data_addr[i];
    }
    return essid;
}
