#ifndef __SPO_MGR_UTILS_H__
#define __SPO_MGR_UTILS_H__

typedef struct _FirewallData {
	uint16_t dport;
	char* protocol;
    uint32_t signature_id;
    uint32_t generator_id;
} FirewallData;

FirewallData *prepareFirewallData(Packet *p, void *event);
char* format(const char *format,...);
void printParameters();
bool isAdverseAction(FirewallData *data);


#endif

