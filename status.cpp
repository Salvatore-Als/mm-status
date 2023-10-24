#include <stdio.h>
#include "status.h"
#include <string>

using namespace std;

char *ConvertSteamID(const char *usteamid)
{
	char *steamid = new char[strlen(usteamid) + 1];
	strcpy(steamid, usteamid);

	for (char *ch = steamid; *ch; ++ch)
	{
		if (*ch == '[' || *ch == ']')
		{
			memmove(ch, ch + 1, strlen(ch));
		}
	}

	char *token = strtok(steamid, ":");
	char *usteamid_split[3];

	int i = 0;
	while (token != nullptr)
	{
		usteamid_split[i++] = token;
		token = strtok(nullptr, ":");
	}

	char steamid_parts[3][12] = {"STEAM_1:", "", ""};

	int z = atoi(usteamid_split[2]);

	if (z % 2 == 0)
	{
		strcpy(steamid_parts[1], "0:");
	}
	else
	{
		strcpy(steamid_parts[1], "1:");
	}

	int steamacct = z / 2;

	snprintf(steamid_parts[2], sizeof(steamid_parts[2]), "%d", steamacct);

	char *result = new char[strlen(steamid_parts[0]) + strlen(steamid_parts[1]) + strlen(steamid_parts[2]) + 1];
	strcpy(result, steamid_parts[0]);
	strcat(result, steamid_parts[1]);
	strcat(result, steamid_parts[2]);

	delete[] steamid;

	return result;
}

Status g_Status;
IServerGameDLL *server = NULL;
ISource2GameClients *gameclients = NULL;
IVEngineServer2 *engine = NULL;
ICvar *icvar = NULL;

PLUGIN_EXPOSE(Status, g_Status);
bool Status::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_ANY(GetServerFactory, gameclients, ISource2GameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);

	g_pCVar = icvar;
	ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

	return true;
}

bool Status::Unload(char *error, size_t maxlen)
{
	return true;
}

void Status::AllPluginsLoaded()
{
}

CON_COMMAND_EXTERN(mm_status, Command_Status, "Display player name with Status");
void Command_Status(const CCommandContext &context, const CCommand &args)
{
	CPlayerSlot slot = context.GetPlayerSlot();

	if (slot.Get() < 0)
	{
		return;
	}

	g_SMAPI->ClientConPrintf(slot, "------------ STATUS ------------\n");

	for (int i = 0; i < 64; i++)
	{
		if (engine->GetPlayerUserId(i).Get() == -1)
		{
			continue;
		}

		const char *networkId = engine->GetPlayerNetworkIDString(i);
		const char *name = engine->GetClientConVarValue(i, "name");

		char szSteamID[64];
		char *steamid = ConvertSteamID(networkId);

		if (strcmp(steamid, "STEAM_1:0:0") == 0)
		{
			g_SMAPI->Format(szSteamID, sizeof(szSteamID), "BOT");
		}
		else
		{
			g_SMAPI->Format(szSteamID, sizeof(szSteamID), "%s", steamid);
		}

		g_SMAPI->ClientConPrintf(slot, "#%d %s %s\n", i, name, szSteamID);
	}

	g_SMAPI->ClientConPrintf(slot, "--------------------------------\n");
}

bool Status::Pause(char *error, size_t maxlen)
{
	return true;
}

bool Status::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *Status::GetLicense()
{
	return "Public Domain";
}

const char *Status::GetVersion()
{
	return "1.1";
}

const char *Status::GetDate()
{
	return __DATE__;
}

const char *Status::GetLogTag()
{
	return "STUB";
}

const char *Status::GetAuthor()
{
	return "Kriax";
}

const char *Status::GetDescription()
{
	return "Display player userid, name and Status";
}

const char *Status::GetName()
{
	return "Status";
}

const char *Status::GetURL()
{
	return "https://kriax.ovh/";
}
