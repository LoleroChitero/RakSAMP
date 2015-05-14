/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

struct stSettings settings;
TiXmlDocument xmlSettings;

int LoadSettings()
{
	// load xml
	if(!xmlSettings.LoadFile("RakSAMPClient.xml"))
	{
		MessageBox(NULL, "Failed to load the config file", "Error", MB_ICONERROR);
		ExitProcess(0);
	}

	TiXmlElement* rakSAMPElement = xmlSettings.FirstChildElement("RakSAMPClient");
	if(rakSAMPElement)
	{
		// get console
		rakSAMPElement->QueryIntAttribute("console", (int *)&settings.iConsole);

		// get runmode
		rakSAMPElement->QueryIntAttribute("runmode", (int *)&settings.runMode);

		// get autorun
		rakSAMPElement->QueryIntAttribute("autorun", (int *)&settings.iAutorun);

		// get find
		rakSAMPElement->QueryIntAttribute("find", (int *)&settings.iFind);

		// get selected class id
		rakSAMPElement->QueryIntAttribute("select_classid", (int *)&settings.iClassID);

		// get manual spawn
		rakSAMPElement->QueryIntAttribute("manual_spawn", (int *)&settings.iManualSpawn);

		// get print_timestamps
		rakSAMPElement->QueryIntAttribute("print_timestamps", (int *)&settings.iPrintTimestamps);

		// get fps simulation
		rakSAMPElement->QueryIntAttribute("updatestats", (int *)&settings.iUpdateStats);

		// get min simulated fps
		rakSAMPElement->QueryIntAttribute("minfps", (int *)&settings.iMinFPS);

		// get max simulated fps
		rakSAMPElement->QueryIntAttribute("maxfps", (int *)&settings.iMaxFPS);

		// get client version
		strcpy(settings.szClientVersion, (char *)rakSAMPElement->Attribute("clientversion"));

		// get chat color
		rakSAMPElement->QueryColorAttribute("chatcolor_rgb",
			(unsigned char *)&settings.bChatColorRed, (unsigned char *)&settings.bChatColorGreen, (unsigned char *)&settings.bChatColorBlue);

		// get client message color
		rakSAMPElement->QueryColorAttribute("clientmsg_rgb",
			(unsigned char *)&settings.bCMsgRed, (unsigned char *)&settings.bCMsgGreen, (unsigned char *)&settings.bCMsgBlue);
		
		// get checkpoint alert color
		rakSAMPElement->QueryColorAttribute("cpalert_rgb",
			(unsigned char *)&settings.bCPAlertRed, (unsigned char *)&settings.bCPAlertGreen, (unsigned char *)&settings.bCPAlertBlue);

		// get followplayer
		strcpy(settings.szFollowingPlayerName, (char *)rakSAMPElement->Attribute("followplayer"));
		rakSAMPElement->QueryIntAttribute("followplayerwithvehicleid", &settings.iFollowingWithVehicleID);
		rakSAMPElement->QueryFloatAttribute("followXOffset", &settings.fFollowXOffset);
		rakSAMPElement->QueryFloatAttribute("followYOffset", &settings.fFollowYOffset);
		rakSAMPElement->QueryFloatAttribute("followZOffset", &settings.fFollowZOffset);

		// get the first server
		TiXmlElement* serverElement = rakSAMPElement->FirstChildElement("server");
		if(serverElement)
		{
			char *pszAddr = (char *)serverElement->GetText();
			if(pszAddr)
			{
				int iPort;
				char *pszAddrBak = pszAddr;

				while(*pszAddrBak)
				{
					if(*pszAddrBak == ':')
					{
						*pszAddrBak = 0;
						pszAddrBak++;
						iPort = atoi(pszAddrBak);
					}
					pszAddrBak++;
				}

				strcpy(settings.server.szAddr, pszAddr);
				settings.server.iPort = iPort;
				strcpy(settings.server.szNickname, (char *)serverElement->Attribute("nickname"));
				strcpy(settings.server.szPassword, (char *)serverElement->Attribute("password"));
			}
		}

		// get intervals
		TiXmlElement* intervalsElement = rakSAMPElement->FirstChildElement("intervals");
		if(intervalsElement)
		{
			intervalsElement->QueryIntAttribute("spam", (int *)&settings.uiSpamInterval);
			intervalsElement->QueryIntAttribute("fakekill", (int *)&settings.uiFakeKillInterval);
			intervalsElement->QueryIntAttribute("lag", (int *)&settings.uiLagInterval);
			intervalsElement->QueryIntAttribute("joinflood", (int *)&settings.uiJoinFloodInterval);
			intervalsElement->QueryIntAttribute("chatflood", (int *)&settings.uiChatFloodInterval);
			intervalsElement->QueryIntAttribute("classflood", (int *)&settings.uiClassFloodInterval);
			intervalsElement->QueryIntAttribute("bulletflood", (int *)&settings.uiBulletFloodInterval);
		}

		// get logging settings
		TiXmlElement* logElement = rakSAMPElement->FirstChildElement("log");
		if(logElement)
		{
			logElement->QueryIntAttribute("objects", (int *)&settings.uiObjectsLogging);
			logElement->QueryIntAttribute("pickups", (int *)&settings.uiPickupsLogging);
			logElement->QueryIntAttribute("textlabels", (int *)&settings.uiTextLabelsLogging);
			logElement->QueryIntAttribute("textdraws", (int *)&settings.uiTextDrawsLogging);
		}

		// get sendrates settings
		TiXmlElement* sendratesElement = rakSAMPElement->FirstChildElement("sendrates");
		if(sendratesElement)
		{
			sendratesElement->QueryIntAttribute("force", (int *)&settings.uiForceCustomSendRates);
			sendratesElement->QueryIntAttribute("onfoot", (int *)&iNetModeNormalOnfootSendRate);
			sendratesElement->QueryIntAttribute("incar", (int *)&iNetModeNormalIncarSendRate);
			sendratesElement->QueryIntAttribute("firing", (int *)&iNetModeFiringSendRate);
			sendratesElement->QueryIntAttribute("multiplier", (int *)&iNetModeSendMultiplier);
		}

		// get normal mode pos
		TiXmlElement* normalPosElement = rakSAMPElement->FirstChildElement("normal_pos");
		if(normalPosElement)
		{
			normalPosElement->QueryVectorAttribute("position", (float *)&settings.fNormalModePos);
			normalPosElement->QueryFloatAttribute("rotation", &settings.fNormalModeRot);
			normalPosElement->QueryIntAttribute("force", (int *)&settings.iNormalModePosForce);
		}

		// get auto run commands
		TiXmlElement* autorunElement = rakSAMPElement->FirstChildElement("autorun");
		if(autorunElement)
		{
			for(int i = 0; i < MAX_AUTORUN_CMDS; i++)
			{
				if(autorunElement)
				{
					settings.autoRunCMDs[i].iExists = 1;
					strcpy(settings.autoRunCMDs[i].szCMD, autorunElement->GetText());
					autorunElement = autorunElement->NextSiblingElement("autorun");
				}
				else
					break;
			}
		}

		TiXmlElement* findElement = rakSAMPElement->FirstChildElement("find");
		if(findElement)
		{
			for(int i = 0; i < MAX_FIND_ITEMS; i++)
			{
				if(findElement)
				{
					settings.findItems[i].iExists = 1;
					strcpy(settings.findItems[i].szFind, findElement->Attribute("text"));
					strcpy(settings.findItems[i].szSay, findElement->Attribute("say"));
					findElement->QueryColorAttribute("bk_color",
						(unsigned char *)&settings.findItems[i].bBkRed,
						(unsigned char *)&settings.findItems[i].bBkGreen,
						(unsigned char *)&settings.findItems[i].bBkBlue);
					findElement->QueryColorAttribute("text_color",
						(unsigned char *)&settings.findItems[i].bTextRed,
						(unsigned char *)&settings.findItems[i].bTextGreen,
						(unsigned char *)&settings.findItems[i].bTextBlue);

					findElement = findElement->NextSiblingElement("find");
				}
				else
					break;
			}
		}

		// get teleport locations
		TiXmlElement* teleportElement = rakSAMPElement->FirstChildElement("teleport");
		if(teleportElement)
		{
			for(int i = 0; i < MAX_TELEPORT_ITEMS; i++)
			{
				if(teleportElement)
				{
					settings.TeleportLocations[i].bCreated = 1;

					strcpy(settings.TeleportLocations[i].szName, (char *)teleportElement->Attribute("name"));
					teleportElement->QueryVectorAttribute("position", (float *)&settings.TeleportLocations[i].fPosition);

					teleportElement = teleportElement->NextSiblingElement("teleport");
				}
				else
					break;
			}
		}
	}

	xmlSettings.Clear();

	PCHAR szCmdLine = GetCommandLineA();
	CHAR szPort[20];

	while(*szCmdLine)
	{
		if(*szCmdLine == '-' || *szCmdLine == '/')
		{
			szCmdLine++;
			switch(*szCmdLine)
			{
				case 'h':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine, settings.server.szAddr);
					break;
				case 'p':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine, szPort); settings.server.iPort = atoi(szPort);
					break;
				case 'n':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine, settings.server.szNickname);
					break;
				case 'z':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine, settings.server.szPassword);
					break;
			}
		}
		szCmdLine++;
	}

	return 1;
}

int UnLoadSettings()
{
	memset(&settings, 0, sizeof(settings));

	return 1;
}

int ReloadSettings()
{
	if(UnLoadSettings() && LoadSettings())
	{
		Log("Settings reloaded");
		return 1;
	}

	Log("Failed to reload settings");

	return 0;
}
