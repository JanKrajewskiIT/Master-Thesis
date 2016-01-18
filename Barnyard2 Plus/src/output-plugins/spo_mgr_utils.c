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
    printf("Firewall type %d \n", barnyard2_conf->firewall_type);
    printf("Firewall lock type %d \n", barnyard2_conf->firewall_lock_type);
    printf("Firewall lock mode %d \n", barnyard2_conf->firewall_lock_mode);
    printf("Firewall lock time %d \n", barnyard2_conf->firewall_lock_time);
    printf("Firewall lock occurances %d \n", barnyard2_conf->firewall_lock_occurances);
    printf("Firewall lock events %s \n", barnyard2_conf->firewall_lock_events[0]);
}
