void FirewalldLockPluginSetup(void) {
	RegisterOutputPlugin("firewalld_lock", OUTPUT_TYPE_FLAG__ALERT, firewalldLockPluginInit);
	DEBUG_WRAP(DebugMessage(DEBUG_INIT, "Output: Firewalld lock plugin is setup...\n"));
}

static void firewalldLockPluginInit(char *args) {
	if(canHandlePlugin(FIREWALLD)) {
		DEBUG_WRAP(DebugMessage(DEBUG_INIT,"Output: Linking FirewalldLockPlugin functions to call lists...\n"));
		AddFuncToOutputList(handleFirewalldLock, OUTPUT_TYPE__ALERT, args);
		turnOnProperFirewall();
	}
}

bool canHandlePlugin(FirewallType firewallType) {
	return (barnyard2_conf->firewall_type == firewallType)
			&& (barnyard2_conf->firewall_lock_num_events > 0);
}

void turnOnProperFirewall() {
	char *firewall_name = getFirewallName(barnyard2_conf->firewall_type);
	system(format("systemctl start %s", firewall_name));
	LogMessage("INFO firewall: Service %s turned on. \n", firewall_name);

	for(int i = FIREWALLD; i <= NFTABLES; i++) {
		if(i != barnyard2_conf->firewall_type) {
			system(format("systemctl stop %s", getFirewallName(i)));
		}
	}
}

static void handleFirewalldLock(Packet *p, void *event, uint32_t event_type, void *arg) {
	FirewallData *data = getFirewallData(p, event);
	if(data != NULL && canHandleEvent(data)) {
		callCommand(data);
	}
}

bool canHandleEvent(FirewallData *data) {
	for(int i = 0; i < barnyard2_conf->firewall_lock_num_events; i++) {
		if(firewall_events[i].generator_id == data->generator_id && firewall_events[i].signature_id == data->signature_id) {
			switch(barnyard2_conf->firewall_lock_type) {
				case OCCURANCES_DEPENDENT : return isExceededOccurancesNumber(&firewall_events[i]);
				case IMMEDIATE : return true;
				default : return false;
			}
		}
	}
	return false;
}

bool isExceededOccurancesNumber(FirewallLockEvent *firewallLockEvent) {
	firewallLockEvent->occurences++;
	if(firewallLockEvent->occurences == barnyard2_conf->firewall_lock_occurances) {
		firewallLockEvent->occurences %= barnyard2_conf->firewall_lock_occurances;
		return true;
	}
	return false;
}

static const char* ADD_PORT_PERMANENT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --permanent";
static const char* ADD_PORT_TIMEOUT_COMMAND = "firewall-cmd --zone=block --add-port=%u/%s --timeout=%d";
static const char* RELOAD_COMMAND = "firewall-cmd --reload";

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

static const char* ADD_PORT_COMMAND = "iptables -A INPUT -p %s --destination-port %u -j DROP";
static const char* RELOAD_COMMAND = "systemctl restart iptables";

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