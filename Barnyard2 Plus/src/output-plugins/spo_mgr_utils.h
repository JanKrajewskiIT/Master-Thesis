#ifndef __SPO_MGR_UTILS_H__
#define __SPO_MGR_UTILS_H__

typedef struct _FirewallData {
	uint16_t dport;
	char* protocol;
} FirewallData;

FirewallData *prepareFirewallData(Packet *p);
char* format(const char *format,...);

#endif

