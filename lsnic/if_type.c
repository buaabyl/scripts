#include <windows.h>
#include <ipifcons.h>

const char* get_if_type_name(int type)
{
    if (type == IF_TYPE_OTHER) {
        return "IF_TYPE_OTHER"; //1
    }
    if (type == IF_TYPE_REGULAR_1822) {
        return "IF_TYPE_REGULAR_1822"; //2
    }
    if (type == IF_TYPE_HDH_1822) {
        return "IF_TYPE_HDH_1822"; //3
    }
    if (type == IF_TYPE_DDN_X25) {
        return "IF_TYPE_DDN_X25"; //4
    }
    if (type == IF_TYPE_RFC877_X25) {
        return "IF_TYPE_RFC877_X25"; //5
    }
    if (type == IF_TYPE_ETHERNET_CSMACD) {
        return "IF_TYPE_ETHERNET_CSMACD"; //6
    }
    if (type == IF_TYPE_IS088023_CSMACD) {
        return "IF_TYPE_IS088023_CSMACD"; //7
    }
    if (type == IF_TYPE_ISO88024_TOKENBUS) {
        return "IF_TYPE_ISO88024_TOKENBUS"; //8
    }
    if (type == IF_TYPE_ISO88025_TOKENRING) {
        return "IF_TYPE_ISO88025_TOKENRING"; //9
    }
    if (type == IF_TYPE_ISO88026_MAN) {
        return "IF_TYPE_ISO88026_MAN"; //10
    }
    if (type == IF_TYPE_STARLAN) {
        return "IF_TYPE_STARLAN"; //11
    }
    if (type == IF_TYPE_PROTEON_10MBIT) {
        return "IF_TYPE_PROTEON_10MBIT"; //12
    }
    if (type == IF_TYPE_PROTEON_80MBIT) {
        return "IF_TYPE_PROTEON_80MBIT"; //13
    }
    if (type == IF_TYPE_HYPERCHANNEL) {
        return "IF_TYPE_HYPERCHANNEL"; //14
    }
    if (type == IF_TYPE_FDDI) {
        return "IF_TYPE_FDDI"; //15
    }
    if (type == IF_TYPE_LAP_B) {
        return "IF_TYPE_LAP_B"; //16
    }
    if (type == IF_TYPE_SDLC) {
        return "IF_TYPE_SDLC"; //17
    }
    if (type == IF_TYPE_DS1) {
        return "IF_TYPE_DS1"; //18
    }
    if (type == IF_TYPE_E1) {
        return "IF_TYPE_E1"; //19
    }
    if (type == IF_TYPE_BASIC_ISDN) {
        return "IF_TYPE_BASIC_ISDN"; //20
    }
    if (type == IF_TYPE_PRIMARY_ISDN) {
        return "IF_TYPE_PRIMARY_ISDN"; //21
    }
    if (type == IF_TYPE_PROP_POINT2POINT_SERIAL) {
        return "IF_TYPE_PROP_POINT2POINT_SERIAL"; //22
    }
    if (type == IF_TYPE_PPP) {
        return "IF_TYPE_PPP"; //23
    }
    if (type == IF_TYPE_SOFTWARE_LOOPBACK) {
        return "IF_TYPE_SOFTWARE_LOOPBACK"; //24
    }
    if (type == IF_TYPE_EON) {
        return "IF_TYPE_EON"; //25
    }
    if (type == IF_TYPE_ETHERNET_3MBIT) {
        return "IF_TYPE_ETHERNET_3MBIT"; //26
    }
    if (type == IF_TYPE_NSIP) {
        return "IF_TYPE_NSIP"; //27
    }
    if (type == IF_TYPE_SLIP) {
        return "IF_TYPE_SLIP"; //28
    }
    if (type == IF_TYPE_ULTRA) {
        return "IF_TYPE_ULTRA"; //29
    }
    if (type == IF_TYPE_DS3) {
        return "IF_TYPE_DS3"; //30
    }
    if (type == IF_TYPE_SIP) {
        return "IF_TYPE_SIP"; //31
    }
    if (type == IF_TYPE_FRAMERELAY) {
        return "IF_TYPE_FRAMERELAY"; //32
    }
    if (type == IF_TYPE_RS232) {
        return "IF_TYPE_RS232"; //33
    }
    if (type == IF_TYPE_PARA) {
        return "IF_TYPE_PARA"; //34
    }
    if (type == IF_TYPE_ARCNET) {
        return "IF_TYPE_ARCNET"; //35
    }
    if (type == IF_TYPE_ARCNET_PLUS) {
        return "IF_TYPE_ARCNET_PLUS"; //36
    }
    if (type == IF_TYPE_ATM) {
        return "IF_TYPE_ATM"; //37
    }
    if (type == IF_TYPE_MIO_X25) {
        return "IF_TYPE_MIO_X25"; //38
    }
    if (type == IF_TYPE_SONET) {
        return "IF_TYPE_SONET"; //39
    }
    if (type == IF_TYPE_X25_PLE) {
        return "IF_TYPE_X25_PLE"; //40
    }
    if (type == IF_TYPE_ISO88022_LLC) {
        return "IF_TYPE_ISO88022_LLC"; //41
    }
    if (type == IF_TYPE_LOCALTALK) {
        return "IF_TYPE_LOCALTALK"; //42
    }
    if (type == IF_TYPE_SMDS_DXI) {
        return "IF_TYPE_SMDS_DXI"; //43
    }
    if (type == IF_TYPE_FRAMERELAY_SERVICE) {
        return "IF_TYPE_FRAMERELAY_SERVICE"; //44
    }
    if (type == IF_TYPE_V35) {
        return "IF_TYPE_V35"; //45
    }
    if (type == IF_TYPE_HSSI) {
        return "IF_TYPE_HSSI"; //46
    }
    if (type == IF_TYPE_HIPPI) {
        return "IF_TYPE_HIPPI"; //47
    }
    if (type == IF_TYPE_MODEM) {
        return "IF_TYPE_MODEM"; //48
    }
    if (type == IF_TYPE_AAL5) {
        return "IF_TYPE_AAL5"; //49
    }
    if (type == IF_TYPE_SONET_PATH) {
        return "IF_TYPE_SONET_PATH"; //50
    }
    if (type == IF_TYPE_SONET_VT) {
        return "IF_TYPE_SONET_VT"; //51
    }
    if (type == IF_TYPE_SMDS_ICIP) {
        return "IF_TYPE_SMDS_ICIP"; //52
    }
    if (type == IF_TYPE_PROP_VIRTUAL) {
        return "IF_TYPE_PROP_VIRTUAL"; //53
    }
    if (type == IF_TYPE_PROP_MULTIPLEXOR) {
        return "IF_TYPE_PROP_MULTIPLEXOR"; //54
    }
    if (type == IF_TYPE_IEEE80212) {
        return "IF_TYPE_IEEE80212"; //55
    }
    if (type == IF_TYPE_FIBRECHANNEL) {
        return "IF_TYPE_FIBRECHANNEL"; //56
    }
    if (type == IF_TYPE_HIPPIINTERFACE) {
        return "IF_TYPE_HIPPIINTERFACE"; //57
    }
    if (type == IF_TYPE_FRAMERELAY_INTERCONNECT) {
        return "IF_TYPE_FRAMERELAY_INTERCONNECT"; //58
    }
    if (type == IF_TYPE_AFLANE_8023) {
        return "IF_TYPE_AFLANE_8023"; //59
    }
    if (type == IF_TYPE_AFLANE_8025) {
        return "IF_TYPE_AFLANE_8025"; //60
    }
    if (type == IF_TYPE_CCTEMUL) {
        return "IF_TYPE_CCTEMUL"; //61
    }
    if (type == IF_TYPE_FASTETHER) {
        return "IF_TYPE_FASTETHER"; //62
    }
    if (type == IF_TYPE_ISDN) {
        return "IF_TYPE_ISDN"; //63
    }
    if (type == IF_TYPE_V11) {
        return "IF_TYPE_V11"; //64
    }
    if (type == IF_TYPE_V36) {
        return "IF_TYPE_V36"; //65
    }
    if (type == IF_TYPE_G703_64K) {
        return "IF_TYPE_G703_64K"; //66
    }
    if (type == IF_TYPE_G703_2MB) {
        return "IF_TYPE_G703_2MB"; //67
    }
    if (type == IF_TYPE_QLLC) {
        return "IF_TYPE_QLLC"; //68
    }
    if (type == IF_TYPE_FASTETHER_FX) {
        return "IF_TYPE_FASTETHER_FX"; //69
    }
    if (type == IF_TYPE_CHANNEL) {
        return "IF_TYPE_CHANNEL"; //70
    }
    if (type == IF_TYPE_IEEE80211) {
        return "IF_TYPE_IEEE80211"; //71
    }
    if (type == IF_TYPE_IBM370PARCHAN) {
        return "IF_TYPE_IBM370PARCHAN"; //72
    }
    if (type == IF_TYPE_ESCON) {
        return "IF_TYPE_ESCON"; //73
    }
    if (type == IF_TYPE_DLSW) {
        return "IF_TYPE_DLSW"; //74
    }
    if (type == IF_TYPE_ISDN_S) {
        return "IF_TYPE_ISDN_S"; //75
    }
    if (type == IF_TYPE_ISDN_U) {
        return "IF_TYPE_ISDN_U"; //76
    }
    if (type == IF_TYPE_LAP_D) {
        return "IF_TYPE_LAP_D"; //77
    }
    if (type == IF_TYPE_IPSWITCH) {
        return "IF_TYPE_IPSWITCH"; //78
    }
    if (type == IF_TYPE_RSRB) {
        return "IF_TYPE_RSRB"; //79
    }
    if (type == IF_TYPE_ATM_LOGICAL) {
        return "IF_TYPE_ATM_LOGICAL"; //80
    }
    if (type == IF_TYPE_DS0) {
        return "IF_TYPE_DS0"; //81
    }
    if (type == IF_TYPE_DS0_BUNDLE) {
        return "IF_TYPE_DS0_BUNDLE"; //82
    }
    if (type == IF_TYPE_BSC) {
        return "IF_TYPE_BSC"; //83
    }
    if (type == IF_TYPE_ASYNC) {
        return "IF_TYPE_ASYNC"; //84
    }
    if (type == IF_TYPE_CNR) {
        return "IF_TYPE_CNR"; //85
    }
    if (type == IF_TYPE_ISO88025R_DTR) {
        return "IF_TYPE_ISO88025R_DTR"; //86
    }
    if (type == IF_TYPE_EPLRS) {
        return "IF_TYPE_EPLRS"; //87
    }
    if (type == IF_TYPE_ARAP) {
        return "IF_TYPE_ARAP"; //88
    }
    if (type == IF_TYPE_PROP_CNLS) {
        return "IF_TYPE_PROP_CNLS"; //89
    }
    if (type == IF_TYPE_HOSTPAD) {
        return "IF_TYPE_HOSTPAD"; //90
    }
    if (type == IF_TYPE_TERMPAD) {
        return "IF_TYPE_TERMPAD"; //91
    }
    if (type == IF_TYPE_FRAMERELAY_MPI) {
        return "IF_TYPE_FRAMERELAY_MPI"; //92
    }
    if (type == IF_TYPE_X213) {
        return "IF_TYPE_X213"; //93
    }
    if (type == IF_TYPE_ADSL) {
        return "IF_TYPE_ADSL"; //94
    }
    if (type == IF_TYPE_RADSL) {
        return "IF_TYPE_RADSL"; //95
    }
    if (type == IF_TYPE_SDSL) {
        return "IF_TYPE_SDSL"; //96
    }
    if (type == IF_TYPE_VDSL) {
        return "IF_TYPE_VDSL"; //97
    }
    if (type == IF_TYPE_ISO88025_CRFPRINT) {
        return "IF_TYPE_ISO88025_CRFPRINT"; //98
    }
    if (type == IF_TYPE_MYRINET) {
        return "IF_TYPE_MYRINET"; //99
    }
    if (type == IF_TYPE_VOICE_EM) {
        return "IF_TYPE_VOICE_EM"; //100
    }
    if (type == IF_TYPE_VOICE_FXO) {
        return "IF_TYPE_VOICE_FXO"; //101
    }
    if (type == IF_TYPE_VOICE_FXS) {
        return "IF_TYPE_VOICE_FXS"; //102
    }
    if (type == IF_TYPE_VOICE_ENCAP) {
        return "IF_TYPE_VOICE_ENCAP"; //103
    }
    if (type == IF_TYPE_VOICE_OVERIP) {
        return "IF_TYPE_VOICE_OVERIP"; //104
    }
    if (type == IF_TYPE_ATM_DXI) {
        return "IF_TYPE_ATM_DXI"; //105
    }
    if (type == IF_TYPE_ATM_FUNI) {
        return "IF_TYPE_ATM_FUNI"; //106
    }
    if (type == IF_TYPE_ATM_IMA) {
        return "IF_TYPE_ATM_IMA"; //107
    }
    if (type == IF_TYPE_PPPMULTILINKBUNDLE) {
        return "IF_TYPE_PPPMULTILINKBUNDLE"; //108
    }
    if (type == IF_TYPE_IPOVER_CDLC) {
        return "IF_TYPE_IPOVER_CDLC"; //109
    }
    if (type == IF_TYPE_IPOVER_CLAW) {
        return "IF_TYPE_IPOVER_CLAW"; //110
    }
    if (type == IF_TYPE_STACKTOSTACK) {
        return "IF_TYPE_STACKTOSTACK"; //111
    }
    if (type == IF_TYPE_VIRTUALIPADDRESS) {
        return "IF_TYPE_VIRTUALIPADDRESS"; //112
    }
    if (type == IF_TYPE_MPC) {
        return "IF_TYPE_MPC"; //113
    }
    if (type == IF_TYPE_IPOVER_ATM) {
        return "IF_TYPE_IPOVER_ATM"; //114
    }
    if (type == IF_TYPE_ISO88025_FIBER) {
        return "IF_TYPE_ISO88025_FIBER"; //115
    }
    if (type == IF_TYPE_TDLC) {
        return "IF_TYPE_TDLC"; //116
    }
    if (type == IF_TYPE_GIGABITETHERNET) {
        return "IF_TYPE_GIGABITETHERNET"; //117
    }
    if (type == IF_TYPE_HDLC) {
        return "IF_TYPE_HDLC"; //118
    }
    if (type == IF_TYPE_LAP_F) {
        return "IF_TYPE_LAP_F"; //119
    }
    if (type == IF_TYPE_V37) {
        return "IF_TYPE_V37"; //120
    }
    if (type == IF_TYPE_X25_MLP) {
        return "IF_TYPE_X25_MLP"; //121
    }
    if (type == IF_TYPE_X25_HUNTGROUP) {
        return "IF_TYPE_X25_HUNTGROUP"; //122
    }
    if (type == IF_TYPE_TRANSPHDLC) {
        return "IF_TYPE_TRANSPHDLC"; //123
    }
    if (type == IF_TYPE_INTERLEAVE) {
        return "IF_TYPE_INTERLEAVE"; //124
    }
    if (type == IF_TYPE_FAST) {
        return "IF_TYPE_FAST"; //125
    }
    if (type == IF_TYPE_IP) {
        return "IF_TYPE_IP"; //126
    }
    if (type == IF_TYPE_DOCSCABLE_MACLAYER) {
        return "IF_TYPE_DOCSCABLE_MACLAYER"; //127
    }
    if (type == IF_TYPE_DOCSCABLE_DOWNSTREAM) {
        return "IF_TYPE_DOCSCABLE_DOWNSTREAM"; //128
    }
    if (type == IF_TYPE_DOCSCABLE_UPSTREAM) {
        return "IF_TYPE_DOCSCABLE_UPSTREAM"; //129
    }
    if (type == IF_TYPE_A12MPPSWITCH) {
        return "IF_TYPE_A12MPPSWITCH"; //130
    }
    if (type == IF_TYPE_TUNNEL) {
        return "IF_TYPE_TUNNEL"; //131
    }
    if (type == IF_TYPE_COFFEE) {
        return "IF_TYPE_COFFEE"; //132
    }
    if (type == IF_TYPE_CES) {
        return "IF_TYPE_CES"; //133
    }
    if (type == IF_TYPE_ATM_SUBINTERFACE) {
        return "IF_TYPE_ATM_SUBINTERFACE"; //134
    }
    if (type == IF_TYPE_L2_VLAN) {
        return "IF_TYPE_L2_VLAN"; //135
    }
    if (type == IF_TYPE_L3_IPVLAN) {
        return "IF_TYPE_L3_IPVLAN"; //136
    }
    if (type == IF_TYPE_L3_IPXVLAN) {
        return "IF_TYPE_L3_IPXVLAN"; //137
    }
    if (type == IF_TYPE_DIGITALPOWERLINE) {
        return "IF_TYPE_DIGITALPOWERLINE"; //138
    }
    if (type == IF_TYPE_MEDIAMAILOVERIP) {
        return "IF_TYPE_MEDIAMAILOVERIP"; //139
    }
    if (type == IF_TYPE_DTM) {
        return "IF_TYPE_DTM"; //140
    }
    if (type == IF_TYPE_DCN) {
        return "IF_TYPE_DCN"; //141
    }
    if (type == IF_TYPE_IPFORWARD) {
        return "IF_TYPE_IPFORWARD"; //142
    }
    if (type == IF_TYPE_MSDSL) {
        return "IF_TYPE_MSDSL"; //143
    }
    if (type == IF_TYPE_IEEE1394) {
        return "IF_TYPE_IEEE1394"; //144
    }
    if (type == IF_TYPE_RECEIVE_ONLY) {
        return "IF_TYPE_RECEIVE_ONLY"; //145
    }
    if (type == MIB_IF_TYPE_OTHER) {
        return "MIB_IF_TYPE_OTHER"; //1
    }
    if (type == MIB_IF_TYPE_ETHERNET) {
        return "MIB_IF_TYPE_ETHERNET"; //6
    }
    if (type == MIB_IF_TYPE_TOKENRING) {
        return "MIB_IF_TYPE_TOKENRING"; //9
    }
    if (type == MIB_IF_TYPE_FDDI) {
        return "MIB_IF_TYPE_FDDI"; //15
    }
    if (type == MIB_IF_TYPE_PPP) {
        return "MIB_IF_TYPE_PPP"; //23
    }
    if (type == MIB_IF_TYPE_LOOPBACK) {
        return "MIB_IF_TYPE_LOOPBACK"; //24
    }
    if (type == MIB_IF_TYPE_SLIP) {
        return "MIB_IF_TYPE_SLIP"; //28
    }
    return "Unknown";
}

