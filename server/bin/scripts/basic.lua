function onScriptStart()
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
	if isbanned then
		outputConsole("[QUERY:BANNED] " .. ip .. ":" .. srcport .. "")
	else
		outputConsole("[QUERY] " .. ip .. ":" .. srcport .. "")
	end
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
	
	setPlayerPos(playerID, 2337.71, -97.92, 5.86)
	--setPlayerPos(playerID, 903.55, -504.32, 14.96)
	setPlayerRotation(playerID, 330.0)
	
	setPlayerScore(playerID, 69)
end

function onPlayerDisconnect(playerID, name, reason)
	outputConsole("[" .. playerID ..":LEAVE] " .. name .. " (" .. reason .. ").")
	for i = 0, MAX_PLAYERS do
		if i ~= playerID then
			sendPlayerMessage(i, -1, "" .. name .. " (ID: " .. playerID .. ") has left the game (" .. reason .. ").")
		end
	end
end

function onPlayerSpawn(playerID)
	outputConsole("[" .. playerID ..":SPAWN] " .. getPlayerName(playerID) .. "")
	clearPlayerWeapons(playerID)
end

function onPlayerDeath(playerID, killerID, weaponID)
	--outputConsole("[" .. playerID ..":DEATH] " .. getPlayerName(playerID) .. " -> " .. getPlayerName(killerID) .. " (" .. getWeaponName(weaponID) ..")")
	outputConsole("[" .. playerID ..":DEATH] " .. getPlayerName(playerID) .. " -> ")
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

function onPlayerCommand(playerID, command)
	--outputConsole("[" .. playerID ..":CMD] " .. getPlayerName(playerID) .. ": " .. command .."")
	
	command = string.split(command, " ")

	if command[1] == "kill" or command[1] == "KILL" then
		setPlayerHealth(playerID, 0)
		return 1
	end
	if command[1] == "pm" or command[1] == "PM" then
		local toplayerid = command[2]
		local message = command[3]
		outputConsole("" .. isPlayerConnected(toplayerid) .. " " .. toplayerid .. " " .. playerID .. "")
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
		return 1
	end
	if command[1] == "v" or command[1] == "V" then
		local vehid = command[2]
		local x, y, z = getPlayerPos(playerID)
		createVehicle(vehid, x + 5.0, y, z, 330.0, 0, 0)
		return 1
	end
	if command[1] == "money" or command[1] == "MONEY" then
		setPlayerMoney(playerID, 696969)
		return 1
	end
	if command[1] == "ha" or command[1] == "HA" then
		setPlayerHealth(playerID, 69)
		setPlayerArmour(playerID, 69)
		return 1
	end
	if command[1] == "w" or command[1] == "W" then
		giveWeapon(playerID, WEAPON_RLAUNCHER, 69)
		setWeaponAmmo(playerID, WEAPON_RLAUNCHER, 69)
		return 1
	end
	if command[1] == "clearw" or command[1] == "CLEARW" then
		clearPlayerWeapons(playerID)
		return 1
	end
	
	sendPlayerMessage(playerID, -1, "Unknown command.")
	return 0
end
