#define CONFIG_OPT__FIREWALL_TYPE			 "firewall_type"
#define CONFIG_OPT__FIREWALL_LOCK_TYPE		 "firewall_lock_type"
#define CONFIG_OPT__FIREWALL_LOCK_MODE 		 "firewall_lock_mode"
#define CONFIG_OPT__FIREWALL_LOCK_TIME 		 "firewall_lock_time"
#define CONFIG_OPT__FIREWALL_LOCK_OCCURANCES "firewall_lock_occurances"
#define CONFIG_OPT__FIREWALL_LOCK_EVENTS 	 "firewall_lock_events"

static const KeywordFunc barnyard2_conf_keywords[] = {
	...
    {CONFIG_OPT__FIREWALL_TYPE, 1, 1, ConfigFirewallType},
    {CONFIG_OPT__FIREWALL_LOCK_TYPE, 1, 1, ConfigFirewallLockType},
    {CONFIG_OPT__FIREWALL_LOCK_MODE, 1, 1, ConfigFirewallLockMode},
    {CONFIG_OPT__FIREWALL_LOCK_TIME, 1, 1, ConfigFirewallLockTime},
    {CONFIG_OPT__FIREWALL_LOCK_OCCURANCES, 1, 1, ConfigFirewallLockOccurances},
    {CONFIG_OPT__FIREWALL_LOCK_EVENTS, 1, 0, ConfigFirewallLockEvents},
	...
};

void ConfigFirewallType(Barnyard2Config *bc, char *args) {
	if(bc == NULL) return;
	if(args != NULL) {
	    if(strcasecmp(args, "firewalld") == 0) {
	    	bc->firewall_type = FIREWALLD;
	    } else if(strcasecmp(args, "iptables") == 0) {
	    	bc->firewall_type = IPTABLES;
	    } else if(strcasecmp(args, "nftables") == 0) {
	    	bc->firewall_type = NFTABLES;
	    } else {
	        ParseError("Non supported firewall type: %s.", args);
	    }
	} else {
    	bc->firewall_type = FIREWALLD;
	}
}

void ConfigFirewallLockType(Barnyard2Config *bc, char *args) {
	if(bc == NULL) return;
	if(args != NULL) {
		if(strcasecmp(args, "immediate") == 0) {
			bc->firewall_lock_type = IMMEDIATE;
		} else if(strcasecmp(args, "occurances_dependent") == 0) {
			bc->firewall_lock_type = OCCURANCES_DEPENDENT;
		} else {
			ParseError("Non supported firewall lock type: %s.", args);
		}
	} else {
		bc->firewall_lock_type = IMMEDIATE;
	}
}

void ConfigFirewallLockMode(Barnyard2Config *bc, char *args) {
	if(bc == NULL) return;
	if(args != NULL) {
		if(strcasecmp(args, "temporary") == 0) {
			bc->firewall_lock_mode = TEMPORARY;
		} else if(strcasecmp(args, "permanent") == 0) {
			bc->firewall_lock_mode = PERMANENT;
		} else {
			ParseError("Non supported firewall lock mode: %s.", args);
		}
	} else {
		bc->firewall_lock_mode = PERMANENT;
	}
}

void ConfigFirewallLockTime(Barnyard2Config *bc, char *args) {
    if ((args == NULL) || (bc == NULL)) return;
    int lock_time = atoi(args);
    if ((errno == ERANGE) || (lock_time < 0)) {
        ParseError("Bad firewall lock time: %s", args);
    }
    bc->firewall_lock_time = lock_time;
}

void ConfigFirewallLockOccurances(Barnyard2Config *bc, char *args) {
    if ((args == NULL) || (bc == NULL)) return;
    int lock_occurances = atoi(args);
    if ((errno == ERANGE) || (lock_occurances < 0)) {
        ParseError("Bad firewall lock occurances: %s", args);
    }
    bc->firewall_lock_occurances = lock_occurances;
}

void ConfigFirewallLockEvents(Barnyard2Config *bc, char *args) {
    if((bc == NULL) || (args == NULL)) return;

    int num_events = 0, i = 0;
    bc->firewall_lock_events = mSplit(args, ",", 0, &num_events, 0);
    bc->firewall_lock_num_events = num_events;

    firewall_events = (FirewallLockEvent *)SnortAlloc(num_events);
    for(i = 0; i < num_events; i++) {
    	int num_parameters = 0;
    	char **ids = mSplit(bc->firewall_lock_events[i], ":", 2, &num_parameters, 0);

    	if(num_parameters != 2) {
        	mSplitFree(&ids, num_parameters);
            ParseError("Bad firewall lock event id: %s", bc->firewall_lock_events[i]);
    	}

    	firewall_events[i].occurences = 0;
    	firewall_events[i].generator_id = atoi(ids[0]);
    	firewall_events[i].signature_id	= atoi(ids[1]);
    	mSplitFree(&ids, num_parameters);
    }
}