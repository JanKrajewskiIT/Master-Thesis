#include <stdlib.h>
#include "barnyard2.h"
#include "debug.h"
#include "spo_lock_utils.h"

static const char* ADD_PORT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --permanent";
static const char* RELOAD_COMMAND = "firewall-cmd --reload";

static void firewalldLockPluginInit(char *);
static void handleFirewalldLock(Packet *, void *, uint32_t, void *);

void FirewalldLockPluginSetup(void) {
	RegisterOutputPlugin("firewalld_lock", OUTPUT_TYPE_FLAG__ALERT, firewalldLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Firewalld lock plugin is setup...\n"));
}

/* Set the preprocessor function into the function list */
static void firewalldLockPluginInit(char *args) {
	if(canRegisterPlugin(FIREWALLD)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking FirewalldLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleFirewalldLock, OUTPUT_TYPE__ALERT, args);
	}
}

static void handleFirewalldLock(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = prepareFirewallData(p, event);
	if(data != NULL) {
		//LogMessage("[%s Port: %u] \n", data->protocol, data->dport);
		char *command = format(ADD_PORT_COMMAND, data->dport, data->protocol);
		LogMessage(command);
		LogMessage("\n");
		system(command);
		system(RELOAD_COMMAND);
	}
}

/**
 * firewall-cmd --zone=block --list-ports
 * systemctl firewalld iptables
 * systemctl restart iptables
 */
