#include <stdlib.h>
#include "barnyard2.h"
#include "debug.h"
#include "spo_lock_utils.h"

static const char* ADD_PORT_PERMANENT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --permanent";
static const char* ADD_PORT_TIMEOUT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --timeout=%d";
static const char* RELOAD_COMMAND = "firewall-cmd --reload";

static void nftablesLockPluginInit(char *);
static void handleNftablesLock(Packet *, void *, uint32_t, void *);
static void callCommand(FirewallData *data);

void NftablesLockPluginSetup(void) {
	RegisterOutputPlugin("nftables_lock", OUTPUT_TYPE_FLAG__ALERT, nftablesLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Nftables lock plugin is setup...\n"));
}

/* Set the preprocessor function into the function list */
static void nftablesLockPluginInit(char *args) {
	if(canHandlePlugin(NFTABLES)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking NftablesLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleNftablesLock, OUTPUT_TYPE__ALERT, args);
		turnOnProperFirewall();
	}
}

static void handleNftablesLock(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = getFirewallData(p, event);
	if(data != NULL && canHandleEvent(data)) {
		LogMessage("[Protocol: %s, Port: %u, GID: %u, SID: %u]\n", data->protocol, data->dport, data->generator_id, data->signature_id);
		callCommand(data);
	}
}

static void callCommand(FirewallData *data) {
	int timeOfLock = getTimeOfLock();
	if(timeOfLock > 0) {
		char *command = format(ADD_PORT_TIMEOUT_COMMAND, data->dport, data->protocol, timeOfLock);
		system(command);
	} else {
		char *command = format(ADD_PORT_PERMANENT_COMMAND, data->dport, data->protocol);
		system(command);
		system(RELOAD_COMMAND);
	}
}
