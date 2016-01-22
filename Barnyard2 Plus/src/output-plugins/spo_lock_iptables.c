#include <stdlib.h>
#include "barnyard2.h"
#include "debug.h"
#include "spo_lock_utils.h"

static const char* ADD_PORT_COMMAND = "iptables -A INPUT -p %s --destination-port %u -j DROP";
static const char* RELOAD_COMMAND = "systemctl restart iptables";
//iptables -A INPUT -p tcp --destination-port 22 -j DROP -m recent --update --name timer --seconds 10
//jak tu z tym timeoutem

static void iptablesLockPluginInit(char *);
static void handleIptablesLock(Packet *, void *, uint32_t, void *);
static void callCommand(FirewallData *data);

void IptablesLockPluginSetup(void) {
	RegisterOutputPlugin("iptables_lock", OUTPUT_TYPE_FLAG__ALERT, iptablesLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Iptables lock plugin is setup...\n"));
}

/* Set the preprocessor function into the function list */
static void iptablesLockPluginInit(char *args) {
	if(canHandlePlugin(IPTABLES)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking IptablesLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleIptablesLock, OUTPUT_TYPE__ALERT, args);
		turnOnProperFirewall();
	}
}

/* Set the preprocessor function into the function list */
static void handleIptablesLock(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = getFirewallData(p, event);
	if(data != NULL && canHandleEvent(data)) {
		LogMessage("[Protocol: %s, Port: %u, GID: %u, SID: %u]\n", data->protocol, data->dport, data->generator_id, data->signature_id);
		callCommand(data);
	}
}

static void callCommand(FirewallData *data) {
	int timeOfLock = getTimeOfLock();
	if(timeOfLock > 0) {
		char *command = format(ADD_PORT_COMMAND, data->dport, data->protocol, timeOfLock);
		system(command);
	} else {
		char *command = format(ADD_PORT_COMMAND, data->dport, data->protocol);
		system(command);
		system(RELOAD_COMMAND);
	}
}
