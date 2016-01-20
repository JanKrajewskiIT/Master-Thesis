#ifndef __SPO_MGR_UTILS_H__
#define __SPO_MGR_UTILS_H__

#include "barnyard2.h"

typedef struct _FirewallData {
	uint16_t dport;
	char* protocol;
    uint32_t signature_id;
    uint32_t generator_id;
} FirewallData;

FirewallData *prepareFirewallData(Packet *p, void *event);
char* format(const char *format,...);
bool isAdverseAction(FirewallData *data);
void printParameters();
void printEvents();

void turnOnProperFirewall();
bool canRegisterPlugin(FirewallType firewallType);
char* getFirewallName(FirewallType firewallType);

#endif

