#include <stdlib.h>
#include "barnyard2.h"
#include "debug.h"
#include "spo_lock_utils.h"

static const char* ADD_PORT_COMMAND = "iptables -A INPUT -p %s --destination-port %u -j DROP";
static const char* RELOAD_COMMAND = "systemctl restart iptables";

static void iptablesLockPluginInit(char *);
static void handleIptablesLock(Packet *, void *, uint32_t, void *);

void IptablesLockPluginSetup(void) {
	RegisterOutputPlugin("iptables_lock", OUTPUT_TYPE_FLAG__ALERT, iptablesLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Iptables lock plugin is setup...\n"));
}

/* Set the preprocessor function into the function list */
static void iptablesLockPluginInit(char *args) {
	if(canRegisterPlugin(IPTABLES)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking IptablesLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleIptablesLock, OUTPUT_TYPE__ALERT, args);
		turnOnProperFirewall();
	}
}

static void handleIptablesLock(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = prepareFirewallData(p, event);
	if(data != NULL) {
		//LogMessage("[%s Port: %u] \n", data->protocol, data->dport);
		char *command = format(ADD_PORT_COMMAND, data->protocol, data->dport);
		LogMessage(command);
		LogMessage("\n");
		system(command);
		system(RELOAD_COMMAND);
	}
}
