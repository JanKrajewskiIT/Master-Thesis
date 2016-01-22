#include "spo_lock_utils.h"
#include "unified2.h"
#include "../sfutil/sf_textlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static char* PROTOCOL_TCP = "tcp";
static char* PROTOCOL_UDP = "udp";

FirewallData *prepareFirewallData(void *event, uint16_t port, char* protocol);

FirewallData *getFirewallData(Packet *p, void *event) {
	if (p == NULL || p->frag_flag || !IPH_IS_VALID(p)) return NULL;

	switch (GET_IPH_PROTO(p)) {
		case IPPROTO_TCP: return prepareFirewallData(event, p->tcph->th_dport, PROTOCOL_TCP);
		case IPPROTO_UDP: return prepareFirewallData(event, p->udph->uh_dport, PROTOCOL_UDP);
		case IPPROTO_ICMP: return NULL;
		default: return NULL;
	}
}

FirewallData *prepareFirewallData(void *event, uint16_t port, char* protocol) {
	FirewallData *data = (FirewallData *)SnortAlloc(sizeof(FirewallData));
	data->signature_id = ntohl(((Unified2EventCommon *)event)->signature_id);
	data->generator_id = ntohl(((Unified2EventCommon *)event)->generator_id);
	data->dport  = ntohs(port);
	data->protocol = protocol;
	return data;
}

void turnOnProperFirewall() {
	char *firewall_name = getFirewallName(barnyard2_conf->firewall_type);
	system(format("systemctl start %s", firewall_name));
	LogMessage("INFO firewall: Service %s turned on. \n", firewall_name);

	int i;
	for(i = FIREWALLD; i <= NFTABLES; i++) {
		if(i != barnyard2_conf->firewall_type) {
			system(format("systemctl stop %s", getFirewallName(i)));
		}
	}
}

bool canHandleEvent(FirewallData *data) {
	int i;
	for(i = 0; i < barnyard2_conf->firewall_lock_num_events; i++) {
		if(firewall_events[i].generator_id == data->generator_id && firewall_events[i].signature_id == data->signature_id) {
			switch(barnyard2_conf->firewall_lock_type) {
				case OCCURANCES_DEPENDENT : return isExceededOccurancesNumber(&firewall_events[i]);
				case IMMEDIATE : return true;
				default : return false;
			}
		}
	}
	return false;
}

bool isExceededOccurancesNumber(FirewallLockEvent *firewallLockEvent) {
	firewallLockEvent->occurences++;
	if(firewallLockEvent->occurences == barnyard2_conf->firewall_lock_occurances) {
		firewallLockEvent->occurences %= barnyard2_conf->firewall_lock_occurances;
		return true;
	}
	return false;
}

bool canHandlePlugin(FirewallType firewallType) {
	return (barnyard2_conf->firewall_type == firewallType)
			&& (barnyard2_conf->firewall_lock_num_events > 0);
}

int getTimeOfLock() {
	if(barnyard2_conf->firewall_lock_mode == TEMPORARY) {
		return barnyard2_conf->firewall_lock_time;
	}
	return 0;
}

char* getFirewallName(FirewallType firewallType) {
	switch(firewallType) {
		case FIREWALLD : return "firewalld";
		case IPTABLES : return "iptables";
		case NFTABLES : return "nftables";
		default : return NULL;
	}
}

char* format(const char *format, ...) {
    char *buf = (char *)SnortAlloc(STD_BUF + 1);
    va_list ap;

    va_start(ap, format);
    vsnprintf(buf, STD_BUF, format, ap);
    va_end(ap);
    return buf;
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
