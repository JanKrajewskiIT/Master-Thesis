#include "barnyard2.h"
#include "spo_mgr_utils.h"
#include "unified2.h"

#include <stdio.h>
#include <stdarg.h>

static char* PROTOCOL_TCP = "tcp";
static char* PROTOCOL_UDP = "udp";

FirewallData *prepareFirewallData(Packet *p, void *event) {
	FirewallData *data = NULL;

	if (p == NULL || p->frag_flag || !IPH_IS_VALID(p)) {
		return NULL;
	}

	switch (GET_IPH_PROTO(p)) {
		case IPPROTO_TCP:
			data = (FirewallData *)SnortAlloc(sizeof(FirewallData));
			data->dport  = ntohs(p->tcph->th_dport);
			data->protocol = PROTOCOL_TCP;
			data->signature_id = ntohl(((Unified2EventCommon *)event)->signature_id);
			data->generator_id = ntohl(((Unified2EventCommon *)event)->generator_id);
			break;
		case IPPROTO_UDP:
			data = (FirewallData *)SnortAlloc(sizeof(FirewallData));
			data->dport  = ntohs(p->udph->uh_dport);
			data->protocol = PROTOCOL_UDP;
			break;
		case IPPROTO_ICMP:
			break;
		default:
			break;
	}

	return data;
}

char* format(const char *format,...) {
    char buf[STD_BUF+1];
    va_list ap;

    va_start(ap, format);
    vsnprintf(buf, STD_BUF, format, ap);
    va_end(ap);
    return buf;
}

bool isAdverseAction(FirewallData *data) {
	int i = 0;
	while(i < barnyard2_conf->firewall_lock_num_events) {
		char *event = data->generator_id + ":" + data->signature_id;
		if(barnyard2_conf->firewall_lock_events[i] == event) {
			return true;
		}
		i++;
	}
	return false;
}

void printParameters() {

    if(barnyard2_conf->firewall_type == FIREWALLD) {
        printf("Firewall type : FIREWALLD \n");
    } else if(barnyard2_conf->firewall_type == IPTABLES) {
        printf("Firewall type : IPTABLES \n");
    } else if(barnyard2_conf->firewall_type == NFTABLES) {
        printf("Firewall type : NFTABLES \n");
    } else {
        printf("Firewall type : NONE \n");
    }

    if(barnyard2_conf->firewall_lock_type == IMMEDIATE) {
    	printf("Firewall lock type : IMMEDIATE \n");
    } else if(barnyard2_conf->firewall_lock_type == OCCURANCES_DEPENDENT) {
    	printf("Firewall lock type : OCCURANCES_DEPENDENT \n");
    } else {
    	printf("Firewall lock type : NONE \n");
    }

    if(barnyard2_conf->firewall_lock_mode == TEMPORARY) {
        printf("Firewall lock mode : TEMPORARY \n");
    } else if(barnyard2_conf->firewall_lock_mode == PERMANENT) {
        printf("Firewall lock mode : PERMANENT \n");
    } else {
        printf("Firewall lock mode : NONE \n");
    }

    printf("Firewall lock time : %d \n", barnyard2_conf->firewall_lock_time);
    printf("Firewall lock occurances : %d \n", barnyard2_conf->firewall_lock_occurances);
    printEvents();

}


void printEvents() {
    int i;
    printf("Event statistics : \n");
    for(i = 0; i < barnyard2_conf->firewall_lock_num_events; i++) {
    	printf("Event with gid %d sid %d and count %d \n", firewall_events[i].generator_id,
    			firewall_events[i].signature_id, firewall_events[i].occurences);
    }
    printf("\n");
}
