#include <stdlib.h>
#include "barnyard2.h"
#include "debug.h"
#include "spo_lock_utils.h"

static const char* ADD_PORT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --permanent";
static const char* RELOAD_COMMAND = "firewall-cmd --reload";

static void nftablesLockPluginInit(char *);
static void handleNftablesLock(Packet *, void *, uint32_t, void *);

void NftablesLockPluginSetup(void) {
	RegisterOutputPlugin("nftables_lock", OUTPUT_TYPE_FLAG__ALERT, nftablesLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Nftables lock plugin is setup...\n"));
}

/* Set the preprocessor function into the function list */
static void nftablesLockPluginInit(char *args) {
	if(canRegisterPlugin(NFTABLES)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking NftablesLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleNftablesLock, OUTPUT_TYPE__ALERT, args);
	}
}

static void handleNftablesLock(Packet *p, void *event, uint32_t event_type, void *arg) {
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
