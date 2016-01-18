#include "barnyard2.h"
#include "debug.h"
#include "spo_mgr_utils.h"

static const char* ADD_PORT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --permanent";
static const char* RELOAD_COMMAND = "firewall-cmd --reload";

static void MgrNftablesInit(char *);
static void handleNftables(Packet *, void *, uint32_t, void *);

void MgrNftablesSetup(void) {
    RegisterOutputPlugin("mgr_nftables", OUTPUT_TYPE_FLAG__ALERT, MgrNftablesInit);
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output plugin: Magisterka is setup...\n"););
}

static void MgrNftablesInit(char *args) {
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: AlertFast Initialized\n"););
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Linking AlertFast functions to call lists...\n"););
    
    /* Set the preprocessor function into the function list */
    //AddFuncToOutputList(handleNftables, OUTPUT_TYPE__ALERT, args);
}

static void handleNftables(Packet *p, void *event, uint32_t event_type, void *arg) {
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
