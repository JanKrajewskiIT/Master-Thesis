#ifndef __SPO_MGR_UTILS_H__
#define __SPO_MGR_UTILS_H__

#include "barnyard2.h"

typedef struct _FirewallData {
	uint16_t dport;
	char* protocol;
    uint32_t signature_id;
    uint32_t generator_id;
} FirewallData;

FirewallData *getFirewallData(Packet *p, void *event);

void turnOnProperFirewall();
bool canHandlePlugin(FirewallType firewallType);
bool canHandleEvent(FirewallData *data);
bool isExceededOccurancesNumber(FirewallLockEvent *firewallLockEvent);
int getTimeOfLock();

char* getFirewallName(FirewallType firewallType);
char* format(const char *format,...);
void printParameters();
void printEvents();

#endif

