#include "barnyard2.h"
#include "debug.h"
#include "spo_mgr_utils.h"

static const char* ADD_PORT_COMMAND = "iptables -A INPUT -p %s --destination-port %u -j DROP";
static const char* RELOAD_COMMAND = "systemctl restart iptables";

static void MgrIptablesInit(char *);
static void handleIptables(Packet *, void *, uint32_t, void *);

void MgrIptablesSetup(void) {
    RegisterOutputPlugin("mgr_iptables", OUTPUT_TYPE_FLAG__ALERT, MgrIptablesInit);
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output plugin: Magisterka is setup...\n"););
}

static void MgrIptablesInit(char *args) {
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: AlertFast Initialized\n"););
    DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Linking AlertFast functions to call lists...\n"););
    
    /* Set the preprocessor function into the function list */
    //AddFuncToOutputList(handleIptables, OUTPUT_TYPE__ALERT, args);
}

static void handleIptables(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = prepareFirewallData(p);
	if(data != NULL) {
		//LogMessage("[%s Port: %u] \n", data->protocol, data->dport);
		char *command = format(ADD_PORT_COMMAND, data->protocol, data->dport);
		LogMessage(command);
		LogMessage("\n");
		system(command);
		system(RELOAD_COMMAND);
	}
}
