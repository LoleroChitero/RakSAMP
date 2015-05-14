ICON_CONNECT = 200;
ICON_DISCONNECT = 201;

function onScriptStart()
	setGameModeText("Basic LUA script")
	setWebURL("github.com/P3ti/RakSAMP")
	setMapName("ugbase.eu")

	addStaticVehicle(Infernus, 2033.4061, 990.7976, 10.3924, 0.0, 0, 0, 1, 5000)
	addStaticVehicle(Hydra, 391.11, 2527.03, 16.52, 0.0, 0, 0, 1, 5000)
	addStaticVehicle(NRG500, 368.34, 2521.56, 16.59, 0.0, 0, 0, 1, 5000)
	addStaticVehicle(Cheetah, 356.01, 2536.30, 16.71, 0.0, 0, 0, 1, 5000)
	addStaticVehicle(Vortex, 367.57, 2553.27, 16.59, 0.0, 27, 27, 1, 5000)
	addStaticVehicle(PoliceCarLS, 387.44, 2557.16, 16.60, 0.0, 27, 27, 1, 5000)
	addStaticVehicle(Linerunner, 399.90, 2554.39, 16.51, 0.0, 27, 27, 1, 5000)
	addStaticVehicle(Camper, 405.90, 2557.39, 16.51, 0.0, 27, 27, 1, 5000)
	addStaticVehicle(PetrolTrailer, 411.90, 2547.39, 16.51, 0.0, 27, 27, 1, 5000)
end

function onScriptExit()
	for i = 0, MAX_VEHICLES do
		removeVehicle(i)
	end	
end

function onNewQuery(ip, srcport, isbanned)
--[[
	if isbanned then
		outputConsole("[QUERY:BANNED] " .. ip .. ":" .. srcport .. "")
	else
		outputConsole("[QUERY] " .. ip .. ":" .. srcport .. "")
	end
--]]
end

function onNewConnection(playerID, ip, srcport)
	outputConsole("[" .. playerID ..":CONN] " .. ip .. ":" .. srcport .. "")
end

function onPlayerJoin(playerID, name, ip, srcport)
	outputConsole("[" .. playerID ..":JOIN] " .. name .. " (" .. ip .. ":" .. srcport .. ")")
	
	for i = 0, MAX_PLAYERS do
		if i ~= playerID then
			sendPlayerMessage(i, -1, "" .. name .. " (ID: " .. playerID .. ") has joined the game.")
		end
	end
	
	sendDeathMessage(0xFFFF, playerID, ICON_CONNECT)
end

function onPlayerDisconnect(playerID, name, reason)
	sendDeathMessage(0xFFFF, playerID, ICON_DISCONNECT)

	outputConsole("[" .. playerID ..":LEAVE] " .. name .. " (" .. reason .. ").")
	for i = 0, MAX_PLAYERS do
		if i ~= playerID then
			sendPlayerMessage(i, -1, "" .. name .. " (ID: " .. playerID .. ") has left the game (" .. reason .. ").")
		end
	end
end

function onPlayerRequestClass(playerID, classID)
	outputConsole("[" .. playerID ..":REQUESTCLASS] " .. getPlayerName(playerID) .. " (" .. classID .. ").")

	setupPlayerForClassSelection(playerID)
end

function onPlayerSpawn(playerID)
	outputConsole("[" .. playerID ..":SPAWN] " .. getPlayerName(playerID) .. "")
	
	setPlayerInterior(playerID, 0)
	setCameraBehindPlayer(playerID)
	
	clearPlayerWeapons(playerID)
	resetPlayerMoney(playerID)
	
	setPlayerScore(playerID, 69)
	
	setPlayerPos(playerID, 405.3287, 2534.4934, 16.5461)
	setPlayerRotation(playerID, 90.4686)
	
	setPlayerHealth(playerID, 100)
	setPlayerArmour(playerid, 100)
end

function onPlayerDeath(playerID, killerID, reasonID)
	sendDeathMessage(killerID, playerID, reasonID)
	gameTextForPlayer(playerID, "Wasted", 5000, 2)
	
	if killerID == 0xFFFF then
		outputConsole("[" .. playerID ..":DEATH] " .. getPlayerName(playerID) .. " -> (" .. reasonID ..")")
	else
		outputConsole("[" .. playerID ..":DEATH] " .. getPlayerName(playerID) .. " -> " .. getPlayerName(killerID) .. " (" .. reasonID ..")")
	end
end

function onPlayerDamageVehicle(playerID, vehicleID)
	--outputConsole("[" .. playerID ..":VDAMAGE] " .. getPlayerName(playerID) .. " <- " .. vehicleID .. "")
end

function onPlayerWantsEnterVehicle(playerID, vehicleID, passenger)
	outputConsole("[" .. playerID ..":WVEHENTER] " .. getPlayerName(playerID) .. " -> " .. vehicleID .. ", passenger: " .. passenger .. "")
end

function onPlayerLeaveVehicle(playerID, vehicleID)
	outputConsole("[" .. playerID ..":VLEAVE] " .. getPlayerName(playerID) .. " <- " .. vehicleID .. "")
end

function onPlayerMessage(playerID, message)
	outputConsole("[" .. playerID ..":CHAT] " .. getPlayerName(playerID) .. ": " .. message .."")
	sendPlayerChatMessageToAll(playerID, message)
end

function split(s, delimiter)
    result = {};
    for match in (s..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match);
    end
    return result;
end

function onPlayerCommand(playerID, command)
	--outputConsole("[" .. playerID ..":CMD] " .. getPlayerName(playerID) .. ": " .. command .."")

	command = split(command, " ")

	if command[1] == "/kill" or command[1] == "/KILL" then
		setPlayerHealth(playerID, 0)
		return 1
	end
	
	if command[1] == "/pm" or command[1] == "/PM" then
		local toplayerid = command[2]
		local message = command[3]

		if toplayerid and message then
			if playerID == toplayerid then -- fix!
				sendPlayerMessage(playerID, -1, "You can't PM to yourself.")
				return 1
			end
			if isPlayerConnected(toplayerid) == 0 then
				sendPlayerMessage(playerID, -1, "The player is not connected.")
				return 1
			end
			outputConsole("[" .. playerID ..":PM] " .. getPlayerName(playerID) .. " to " .. getPlayerName(toplayerid) .." (" .. toplayerid .. "): " .. message .. "")
			sendPlayerMessage(toplayerid, -1, "PM from " .. getPlayerName(playerID) .. " (" .. playerID .."): " .. message .. "")
		else
			sendPlayerMessage(playerID, -1, "USAGE: /pm <toplayerid> <message>")
		end
		return 1
	end
	
	if command[1] == "/v" or command[1] == "/V" then
		local vehid = command[2]
		local x, y, z = getPlayerPos(playerID)
		createVehicle(vehid, x + 5.0, y, z, 330.0, 0, 0)
		return 1
	end
	
	if command[1] == "/money" or command[1] == "/MONEY" then
		givePlayerMoney(playerID, 696969)
		return 1
	end
	
	if command[1] == "/ha" or command[1] == "/HA" then
		setPlayerHealth(playerID, 69)
		setPlayerArmour(playerID, 69)
		return 1
	end
	
	if command[1] == "/w" or command[1] == "/W" then
		giveWeapon(playerID, 24, 1)
		setWeaponAmmo(playerID, 24, 69)
		
		giveWeapon(playerID, 31, 1)
		setWeaponAmmo(playerID, 31, 69)
		return 1
	end
	
	if command[1] == "/clearw" or command[1] == "/CLEARW" then
		clearPlayerWeapons(playerID)
		return 1
	end
	
	if command[1] == "/audiostream" or command[1] == "/AUDIOSTREAM" then
		playAudioStreamForPlayer(playerID, "http://somafm.com/tags.pls", 0.0, 0.0, 0.0, 0)
		return 1
	end
	
	if command[1] == "/stopaudiostream" or command[1] == "/STOPAUDIOSTREAM" then
		stopAudioStreamForPlayer(playerID)
		return 1
	end
	
	if command[1] == "/cp" or command[1] == "/CP" then
		local x, y, z = getPlayerPos(playerID)
		setPlayerCheckpoint(playerID, x, y, z, 2.5)
		sendPlayerMessage(playerID, -1, "Checkpoint set to your position.")
		return 1
	end
	
	if command[1] == "/disablecp" or command[1] == "/DISABLECP" then
		disablePlayerCheckpoint(playerID)
		sendPlayerMessage(playerID, -1, "Checkpoint disabled.")
		return 1
	end
	
	if command[1] == "/vr" or command[1] == "/VR" then
		repairVehicle(getPlayerVehicleID(playerID))
		return 1
	end	
	
	if command[1] == "/dmsgbox" or command[1] == "/DMSGBOX" then
		showPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, "DIALOG_STYLE_MSGBOX", "This is a message box dialog.\nIt can be used to display a message to a player.", "Button 1", "Button 2")
		return 1
	end	
	
	if command[1] == "/dinput" or command[1] == "/DINPUT" then
		showPlayerDialog(playerid, 1, DIALOG_STYLE_INPUT, "DIALOG_STYLE_INPUT", "The input dialog style allows players to input text into the dialog.", "Button 1", "Button 2")
		return 1
	end	
	
	if command[1] == "/dlist" or command[1] == "/DLIST" then
		showPlayerDialog(playerid, 2, DIALOG_STYLE_LIST, "DIALOG_STYLE_LIST", "Item 0\nItem 1\nItem 2", "Button 1", "Button 2")
		return 1
	end	
	
	if command[1] == "/dpassword" or command[1] == "/DPASSWORD" then
		showPlayerDialog(playerid, 3, DIALOG_STYLE_PASSWORD, "DIALOG_STYLE_PASSWORD", "The password dialog style allows players to input text into the dialog without revealing it.", "Button 1", "Button 2")
		return 1
	end	

	sendPlayerMessage(playerID, -1, "Unknown command.")
	return 0
end

function onPlayerWeaponShot(playerID, weaponID, hitType, hitID, X, Y, Z)
	--outputConsole("[" .. playerID ..":WEAPONSHOT] " .. getPlayerName(playerID) .. ": " .. weaponID .." " .. hitType .. " " .. hitID .. " " .. X .. " " .. Y .. " ".. Z .. "")
end

function onPlayerEnterCheckpoint(playerID)
	sendPlayerMessage(playerID, -1, "onPlayerEnterCheckpoint event called.")
end

function onPlayerLeaveCheckpoint(playerID)
	sendPlayerMessage(playerID, -1, "onPlayerLeaveCheckpoint event called.")
end

function onPlayerClickMap(playerID, X, Y, Z)
	setPlayerPosFindZ(playerID, X, Y, Z)
end

function onDialogResponse(playerID, dialogID, response, listitem, inputtext)
	outputConsole("[" .. playerID ..":DIALOGRESPONSE] " .. getPlayerName(playerID) .. " (" .. dialogID .. " " .. response .. " " .. listitem .. " " .. inputtext ..")")
end

function onPlayerInteriorChange(playerID, interiorID)
	outputConsole("[" .. playerID ..":INTERIOR_CHANGE] " .. getPlayerName(playerID) .. " (" .. interiorID ..")")
end

function onScmEvent(playerID, eventID, param1, param2, param3)
	outputConsole("[" .. playerID ..":SCM_EVENT] " .. getPlayerName(playerID) .. " (" .. eventID .. " " .. param1 .. " " .. param2 .. " " .. param3 .. ")")

	broadcastScmEvent(playerID, eventID, param1, param2, param3)
end

function setupPlayerForClassSelection(playerID)
 	setPlayerInterior(playerID, 14)
	setPlayerPos(playerID,258.4893, -41.4008, 1002.0234)
	setPlayerRotation(playerID, 270.0)
	setPlayerCameraPos(playerID, 256.0815, -43.0475, 1004.0234)
	setPlayerCameraLookAt(playerID, 258.4893, -41.4008, 1002.0234)
end

function split(inputstr, sep)
        if sep == nil then
                sep = "%s"
        end
        local t={} ; i=1
        for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
                t[i] = str
                i = i + 1
        end
        return t
end