#include "g_local.h"
#ifdef CLEANCTF_ENABLED

#ifdef TEMP
/*----------------------------------------------------------------------------------*/
/* ADMIN */

struct admin_settings_t
{
	int matchlen;
	int matchsetuplen;
	int matchstartlen;
	bool weaponsstay;
	bool instantitems;
	bool quaddrop;
	bool instantweap;
	bool matchlock;
};

#define SETMENU_SIZE (7 + 5)

void CTFAdmin_UpdateSettings(edict_t *ent, pmenuhnd_t *setmenu);
void CTFOpenAdminMenu(edict_t *ent);

void CTFAdmin_SettingsApply(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;
	char st[80];
	int i;

	if (settings->matchlen != matchtime->value) {
		gi.bprintf(PRINT_HIGH, "%s changed the match length to %d minutes.\n",
			ent->client->Persistent.netname, settings->matchlen);
		if (ctfgame.match == MATCH_GAME) {
			// in the middle of a match, change it on the fly
			ctfgame.matchtime = (ctfgame.matchtime - matchtime->value*60) + settings->matchlen*60;
		} 
		sprintf(st, "%d", settings->matchlen);
		gi.cvar_set("matchtime", st);
	}

	if (settings->matchsetuplen != matchsetuptime->value) {
		gi.bprintf(PRINT_HIGH, "%s changed the match setup time to %d minutes.\n",
			ent->client->Persistent.netname, settings->matchsetuplen);
		if (ctfgame.match == MATCH_SETUP) {
			// in the middle of a match, change it on the fly
			ctfgame.matchtime = (ctfgame.matchtime - matchsetuptime->value*60) + settings->matchsetuplen*60;
		} 
		sprintf(st, "%d", settings->matchsetuplen);
		gi.cvar_set("matchsetuptime", st);
	}

	if (settings->matchstartlen != matchstarttime->value) {
		gi.bprintf(PRINT_HIGH, "%s changed the match start time to %d seconds.\n",
			ent->client->Persistent.netname, settings->matchstartlen);
		if (ctfgame.match == MATCH_PREGAME) {
			// in the middle of a match, change it on the fly
			ctfgame.matchtime = (ctfgame.matchtime - matchstarttime->value) + settings->matchstartlen;
		} 
		sprintf(st, "%d", settings->matchstartlen);
		gi.cvar_set("matchstarttime", st);
	}

	if (settings->weaponsstay != !!((int)dmflags->value & DF_WEAPONS_STAY)) {
		gi.bprintf(PRINT_HIGH, "%s turned %s weapons stay.\n",
			ent->client->Persistent.netname, settings->weaponsstay ? "on" : "off");
		i = (int)dmflags->value;
		if (settings->weaponsstay)
			i |= DF_WEAPONS_STAY;
		else
			i &= ~DF_WEAPONS_STAY;
		sprintf(st, "%d", i);
		gi.cvar_set("dmflags", st);
	}

	if (settings->instantitems != !!((int)dmflags->value & DF_INSTANT_ITEMS)) {
		gi.bprintf(PRINT_HIGH, "%s turned %s instant items.\n",
			ent->client->Persistent.netname, settings->instantitems ? "on" : "off");
		i = (int)dmflags->value;
		if (settings->instantitems)
			i |= DF_INSTANT_ITEMS;
		else
			i &= ~DF_INSTANT_ITEMS;
		sprintf(st, "%d", i);
		gi.cvar_set("dmflags", st);
	}

	if (settings->quaddrop != !!((int)dmflags->value & DF_QUAD_DROP)) {
		gi.bprintf(PRINT_HIGH, "%s turned %s quad drop.\n",
			ent->client->Persistent.netname, settings->quaddrop ? "on" : "off");
		i = (int)dmflags->value;
		if (settings->quaddrop)
			i |= DF_QUAD_DROP;
		else
			i &= ~DF_QUAD_DROP;
		sprintf(st, "%d", i);
		gi.cvar_set("dmflags", st);
	}

	if (settings->instantweap != !!((int)instantweap->value)) {
		gi.bprintf(PRINT_HIGH, "%s turned %s instant weapons.\n",
			ent->client->Persistent.netname, settings->instantweap ? "on" : "off");
		sprintf(st, "%d", (int)settings->instantweap);
		gi.cvar_set("instantweap", st);
	}

	if (settings->matchlock != !!((int)matchlock->value)) {
		gi.bprintf(PRINT_HIGH, "%s turned %s match lock.\n",
			ent->client->Persistent.netname, settings->matchlock ? "on" : "off");
		sprintf(st, "%d", (int)settings->matchlock);
		gi.cvar_set("matchlock", st);
	}

	PMenu_Close(ent);
	CTFOpenAdminMenu(ent);
}

void CTFAdmin_SettingsCancel(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	PMenu_Close(ent);
	CTFOpenAdminMenu(ent);
}

void CTFAdmin_ChangeMatchLen(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->matchlen = (settings->matchlen % 60) + 5;
	if (settings->matchlen < 5)
		settings->matchlen = 5;

	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeMatchSetupLen(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->matchsetuplen = (settings->matchsetuplen % 60) + 5;
	if (settings->matchsetuplen < 5)
		settings->matchsetuplen = 5;

	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeMatchStartLen(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->matchstartlen = (settings->matchstartlen % 600) + 10;
	if (settings->matchstartlen < 20)
		settings->matchstartlen = 20;

	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeWeapStay(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->weaponsstay = !settings->weaponsstay;
	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeInstantItems(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->instantitems = !settings->instantitems;
	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeQuadDrop(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->quaddrop = !settings->quaddrop;
	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeInstantWeap(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->instantweap = !settings->instantweap;
	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_ChangeMatchLock(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings = p->arg;

	settings->matchlock = !settings->matchlock;
	CTFAdmin_UpdateSettings(ent, p);
}

void CTFAdmin_UpdateSettings(edict_t *ent, pmenuhnd_t *setmenu)
{
	int i = 2;
	char text[64];
	admin_settings_t *settings = setmenu->arg;

	sprintf(text, "Match Len:       %2d mins", settings->matchlen);
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeMatchLen);
	i++;

	sprintf(text, "Match Setup Len: %2d mins", settings->matchsetuplen);
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeMatchSetupLen);
	i++;

	sprintf(text, "Match Start Len: %2d secs", settings->matchstartlen);
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeMatchStartLen);
	i++;

	sprintf(text, "Weapons Stay:    %s", settings->weaponsstay ? "Yes" : "No");
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeWeapStay);
	i++;

	sprintf(text, "Instant Items:   %s", settings->instantitems ? "Yes" : "No");
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeInstantItems);
	i++;

	sprintf(text, "Quad Drop:       %s", settings->quaddrop ? "Yes" : "No");
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeQuadDrop);
	i++;

	sprintf(text, "Instant Weapons: %s", settings->instantweap ? "Yes" : "No");
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeInstantWeap);
	i++;

	sprintf(text, "Match Lock:      %s", settings->matchlock ? "Yes" : "No");
	PMenu_UpdateEntry(setmenu->entries + i, text, PMENU_ALIGN_LEFT, CTFAdmin_ChangeMatchLock);
	i++;

	PMenu_Update(ent);
}

pmenu_t def_setmenu[] = {
	{ "*Settings Menu", PMENU_ALIGN_CENTER, NULL },
	{ NULL,				PMENU_ALIGN_CENTER, NULL },
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//int matchlen;         
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//int matchsetuplen;    
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//int matchstartlen;    
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//bool weaponsstay; 
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//bool instantitems;
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//bool quaddrop;    
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//bool instantweap; 
	{ NULL,				PMENU_ALIGN_LEFT, NULL },	//bool matchlock; 
	{ NULL,				PMENU_ALIGN_LEFT, NULL },
	{ "Apply",			PMENU_ALIGN_LEFT, CTFAdmin_SettingsApply },
	{ "Cancel",			PMENU_ALIGN_LEFT, CTFAdmin_SettingsCancel }
};

void CTFAdmin_Settings(edict_t *ent, pmenuhnd_t *p)
{
	admin_settings_t *settings;
	pmenuhnd_t *menu;

	PMenu_Close(ent);

	settings = malloc(sizeof(*settings));

	settings->matchlen = matchtime->value;
	settings->matchsetuplen = matchsetuptime->value;
	settings->matchstartlen = matchstarttime->value;
	settings->weaponsstay = !!((int)dmflags->value & DF_WEAPONS_STAY);
	settings->instantitems = !!((int)dmflags->value & DF_INSTANT_ITEMS);
	settings->quaddrop = !!((int)dmflags->value & DF_QUAD_DROP);
	settings->instantweap = instantweap->value != 0;
	settings->matchlock = matchlock->value != 0;

	menu = PMenu_Open(ent, def_setmenu, -1, sizeof(def_setmenu) / sizeof(pmenu_t), settings);
	CTFAdmin_UpdateSettings(ent, menu);
}

void CTFAdmin_MatchSet(edict_t *ent, pmenuhnd_t *p)
{
	PMenu_Close(ent);

	if (ctfgame.match == MATCH_SETUP) {
		gi.bprintf(PRINT_CHAT, "Match has been forced to start.\n");
		ctfgame.match = MATCH_PREGAME;
		ctfgame.matchtime = level.framenum + matchstarttime->value;
	} else if (ctfgame.match == MATCH_GAME) {
		gi.bprintf(PRINT_CHAT, "Match has been forced to terminate.\n");
		ctfgame.match = MATCH_SETUP;
		ctfgame.matchtime = level.framenum + matchsetuptime->value * 600;
		CTFResetAllPlayers();
	}
}

void CTFAdmin_MatchMode(edict_t *ent, pmenuhnd_t *p)
{
	PMenu_Close(ent);

	if (ctfgame.match != MATCH_SETUP) {
		if (competition->value < 3)
			gi.cvar_set("competition", "2");
		ctfgame.match = MATCH_SETUP;
		CTFResetAllPlayers();
	}
}

void CTFAdmin_Cancel(edict_t *ent, pmenuhnd_t *p)
{
	PMenu_Close(ent);
}


pmenu_t adminmenu[] = {
	{ "*Administration Menu",	PMENU_ALIGN_CENTER, NULL },
	{ NULL,						PMENU_ALIGN_CENTER, NULL }, // blank
	{ "Settings",				PMENU_ALIGN_LEFT, CTFAdmin_Settings },
	{ NULL,						PMENU_ALIGN_LEFT, NULL },
	{ NULL,						PMENU_ALIGN_LEFT, NULL },
	{ "Cancel",					PMENU_ALIGN_LEFT, CTFAdmin_Cancel },
	{ NULL,						PMENU_ALIGN_CENTER, NULL },
};

void CTFOpenAdminMenu(edict_t *ent)
{
	adminmenu[3].text = NULL;
	adminmenu[3].SelectFunc = NULL;
	if (ctfgame.match == MATCH_SETUP) {
		adminmenu[3].text = "Force start match";
		adminmenu[3].SelectFunc = CTFAdmin_MatchSet;
	} else if (ctfgame.match == MATCH_GAME) {
		adminmenu[3].text = "Cancel match";
		adminmenu[3].SelectFunc = CTFAdmin_MatchSet;
	} else if (ctfgame.match == MATCH_NONE && competition->value) {
		adminmenu[3].text = "Switch to match mode";
		adminmenu[3].SelectFunc = CTFAdmin_MatchMode;
	}

//	if (ent->client->menu)
//		PMenu_Close(ent->client->menu);

	PMenu_Open(ent, adminmenu, -1, sizeof(adminmenu) / sizeof(pmenu_t), NULL);
}

void CTFAdmin(edict_t *ent)
{
	char text[1024];

	if (gi.argc() > 1 && admin_password->string && *admin_password->string &&
		!ent->client->Respawn.admin && strcmp(admin_password->string, gi.argv(1)) == 0) {
		ent->client->Respawn.admin = true;
		gi.bprintf(PRINT_HIGH, "%s has become an admin.\n", ent->client->Persistent.netname);
		gi.cprintf(ent, PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
	}

	if (!ent->client->Respawn.admin) {
		sprintf(text, "%s has requested admin rights.",
			ent->client->Persistent.netname);
		CTFBeginElection(ent, ELECT_ADMIN, text);
		return;
	}

	if (ent->client->menu)
		PMenu_Close(ent);

	CTFOpenAdminMenu(ent);
}
#endif
bool CTFBeginElection(CPlayerEntity *ent, EElectState type, char *msg);

void CTFAdmin(CPlayerEntity *ent)
{
	char text[1024];

	if (ArgCount() > 1 && admin_password->String() && *admin_password->String() &&
		!ent->Client.Respawn.admin && strcmp(admin_password->String(), ArgGets(1).c_str()) == 0)
	{
		ent->Client.Respawn.admin = true;
		BroadcastPrintf(PRINT_HIGH, "%s has become an admin.\n", ent->Client.Persistent.netname);
		ent->PrintToClient (PRINT_HIGH, "Type 'admin' to access the adminstration menu.\n");
	}

	if (!ent->Client.Respawn.admin) {
		Q_snprintfz(text, sizeof(text), "%s has requested admin rights.",
			ent->Client.Persistent.netname);
		CTFBeginElection(ent, ELECT_ADMIN, text);
		return;
	}

}
#endif