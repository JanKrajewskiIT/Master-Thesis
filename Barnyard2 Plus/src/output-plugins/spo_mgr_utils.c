#include "barnyard2.h"
#include "spo_mgr_utils.h"

#include <stdio.h>
#include <stdarg.h>

static char* PROTOCOL_TCP = "tcp";
static char* PROTOCOL_UDP = "udp";

FirewallData *prepareFirewallData(Packet *p) {
	FirewallData *data = NULL;

	if (p == NULL || p->frag_flag || !IPH_IS_VALID(p)) {
		return NULL;
	}

	switch (GET_IPH_PROTO(p)) {
		case IPPROTO_TCP:
			data = (FirewallData *)SnortAlloc(sizeof(FirewallData));
			data->dport  = ntohs(p->tcph->th_dport);
			data->protocol = PROTOCOL_TCP;
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
