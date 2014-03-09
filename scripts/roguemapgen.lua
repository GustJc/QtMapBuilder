--Baseado em script de CookieClicker(Dungeon Generator):
--http://orteil.dashnet.org/experiments/dungeongenerator/
w 		= 40
h 		= 40
mapId = 0
ts 		= 4
mapRoom = {}
drawTime = 40

TILE_WALL = 30 -- na verdade, bloco, n?o cavado e n?o parede
TILE_DOOR = 4
TILE_START = 0
TILE_END   = 2
--Map variables
walls = {}
dug   = {}
rooms = {}
tilesDug = 0
roomId   = 0
mirror	 = 0
roomSize = 15
corridorSize = 5
percentToFill = 0.33
corridorPercent= 0.4
pillarPercent  = 0.1
waterPercent   = 0.1

roomSize = math.floor(math.random(5,10))
corridorSize = math.floor(math.random(1,6))
corridorPercent = math.random(0.1, 0.5)
--pillarPercent = math.random(0,0.3)
percentToFill = 0.25 + math.random(0,0.3)
--waterPercent = math.random()

if(mirror == 1) then percentToFill = percentToFill/4 end

stuck = 0
tooMuchStuck = 150
digs = 0

--Create mapRoom
for i = 1, w do
	mapRoom[i] = {}
end
function choose(arr) 
	return arr[math.floor(math.random()*#arr)+1];
end

function indexOf(mat, el1,el2)
	for i = 1, #mat do
		if mat[i][1] == el1 then
			if mat[i][2] == el2 then
				return i
			end
		end
	end
	
	return -1
end

function indexOfOne(mat, el1)
	for i = 1, #mat do
		if mat[i] == el1 then
			return i
		end
	end
	
	return -1
end

function GetMapGfx(x,y)
	if x<0 or x >=w or y<0 or y >= h then return -1 end
	
	return map:getTile(x,y).gfx
end

function GetMapType(x,y)
	if x<0 or x >=w or y<0 or y >= h then return -1 end
	
	return map:getTile(x,y).type
end

function SetMap(x,y, gfx, mType)
	if x<0 or x >=w or y<0 or y >= h then return false end
	
	map:setTile(x,y,gfx,mType)
	return true
end

function GetMapRoom(x,y)
	--Arruma conversao de array lua para C++
	x = x+1
	y = y+1
	if x<1 or x >w or y<1 or y > h then return -1 end
	return mapRoom[x][y]
end
function SetMapRoom(x,y, roomId)
	--Arruma conversao de array lua para C++
	x = x+1
	y = y+1
	if x<1 or x >w or y<1 or y > h then return false end
	
	mapRoom[x][y] = roomId
	return true
end

function CheckRoom(X,Y,W,H, thisRoomId)
	X = math.floor(X)
	Y = math.floor(Y)
	W = math.floor(W)
	H = math.floor(H)
	
	for x=X, X+W do
		for y=Y, Y+H do
		_=[[
			if(GetMapGfx(x,y) ~= TILE_WALL and thisRoomId == -1) then 
				return false 
			end
			if(GetMapRoom(x,y) ~= thisRoomId and thisRoomId ~= 1 and GetMapGfx(x,y) ~= TILE_WALL) then 
				return false 
			end
			if mirror == 0 and (x < 1 or x >= w-1 or y < 1 or y >= h-1) then 
				return false
			elseif (x<1 or x>=w/2-1 or y<1 or y>=h/2-1) then
				return false
			end
			]]
			if GetMapGfx(x,y) ~= TILE_WALL and GetMapRoom(x,y) ~= thisRoomId then
				return false
			end
			if(x<1 or x>=w-1 or y<1 or y>h-2) then
				return false
			end
			
		end
	end
	
	return true
	
end

function RandomIn(room)
	local tiles = {}
	
	for x=0, w do
		for y=0, h do
			if GetMapRoom(x,y) == room and GetMapType(x,y) == Tile.PASS then
			 tiles[#tiles+1] = {x,y}
			end
		end
	end
	return choose(tiles)
end
--Cria a sala
function MakeRoom(X, Y, W, H, tile, roomId)
	pillars = 0
	if math.random() < pillarPercent then
		pillars = choose({1,2,3})
	end
	
	watery = 0
	if math.random() < waterPercent then watery = 1 end
	X = math.floor(X)
	Y = math.floor(Y)
	W = math.floor(W)
	H = math.floor(H)

	--Cava sala
	for x=X, X+W do
		for y=Y, Y+H do
			if GetMapType(x,y) == Tile.BLOCK then
				local gfx = 1
				if tile then gfx = tile end
				
				SetMap(x,y,gfx,  Tile.PASS)-- tile or  1
				SetMapRoom(x,y,roomId)
				dug[#dug+1] = {x,y}
				tilesDug = tilesDug+1
			end
		end	
	end--end fors
	
	--Expande bordas em coloca paredes
	for x=X-1, X+W+1 do
		for y=Y, Y+H do
			--Se jah houver paredes, escurece grafico
			if GetMapGfx(x,y) == 5 and GetMapType(x,y) == Tile.BLOCK then
				SetMap(x,y, 6, Tile.BLOCK)
				table.remove(walls, indexOf(walls, x, y))
			elseif GetMapType(x,y) == Tile.BLOCK then
				SetMap(x,y,5, Tile.BLOCK)
				walls[#walls+1] = {x,y}
				SetMapRoom(x,y,roomId)
			end
		end
	end
	for x=X, X+W do
		for y=Y-1, Y+H+1 do
			--Se jah houver paredes, escurece grafico
			if GetMapGfx(x,y) == 5 and GetMapType(x,y) == Tile.BLOCK then
				SetMap(x,y, 6, Tile.BLOCK)
				table.remove(walls, indexOf(walls, x, y))
			elseif GetMapType(x,y) == Tile.BLOCK then
				SetMap(x,y,5, Tile.BLOCK)
				walls[#walls+1] = {x,y}
				SetMapRoom(x,y,roomId)
			end
		end
	end
	--Paredes nos cantos
	x = X-1
	y = Y-1
	if GetMapType(x,y) == Tile.BLOCK then
		SetMap(x,y,5,Tile.BLOCK)
		SetMapRoom(x,y,roomId)
	end
	x = X+W+1
	y = Y-1
	if GetMapType(x,y) == Tile.BLOCK then
		SetMap(x,y,5,Tile.BLOCK)
		SetMapRoom(x,y,roomId)
	end
	x = X-1
	y = Y+H+1
	if GetMapType(x,y) == Tile.BLOCK then
		SetMap(x,y,5,Tile.BLOCK)
		SetMapRoom(x,y,roomId)
	end
	x = X+W+1
	y = Y+H+1
	if GetMapType(x,y) == Tile.BLOCK then
		SetMap(x,y,5,Tile.BLOCK)
		SetMapRoom(x,y,roomId)
	end
	
	--Conteudo das salas, se nada pre-definido, random de acordo com parametros
	if(tile == null) then
		for x=X+1, X+W-1 do
			for y=Y+1, Y+H-1 do
				SetMap(x,y,3, Tile.PASS)
				if watery == 1 and math.random() < 0.8 then
					SetMap(x,y,9, Tile.BLOCK)
				end				
				
				local pillaring = 0
				if pillars == 1 and (x-X)%2 == 0 and (y-Y)%2 == 0 and math.random() < 0.8 then
					pillaring = 1
				elseif pillars == 2 and (x-X-1)%3 < 2 and (y-Y-1)%3 < 2 and math.random() < 0.8 then
					pillaring = 1
				elseif pillars == 3 and (x-X)%3 == 0 and (y-Y)%3 == 0 and math.random() < 0.8 then
					pillaring = 1
				end
				
				if(pillaring == 1) then
					table.remove(dug, indexOf(dug,x,y))
					SetMap(x,y,7,Tile.BLOCK)
				end
				
			end
		end
	end
end

function PickWall(x,y,iw,ih)
	-- Se tiver parametros, retorna uma das paredes que estiverem dentro da area
	if(x) then 
		myWalls = {}
		for i = 1, #walls do
			if walls[i][1] >= x and walls[i][1] < x+iw and walls[i][2] >= y and walls[i][2] < y+ih then
				myWalls[#myWalls+1] = walls[i]
			end
		end
		return choose(myWalls)
	else -- Sen?o, retorna uma parede aleat?ria
		return choose(walls)
	end
end

function Carve(wall, side, roomId)
	local rx = wall[1] + side[1]
	local ry = wall[2] + side[2]
	local rw = 0
	local rh = 0
	local expansions = {{0,1,2,3}}
	local corridor   = 0
	
	if math.random() < corridorPercent then corridor = 1 end
	
	if corridor == 1 then 
		expansions = {{0,2},{1,3}} 
	end
	
	expansions = choose(expansions) -- Fora no original
	
	--Impede que expans?o seja em dire??o a porta
	--if side[1] == 1 and side[2] == 0 then
	--	table.remove(expansions,indexOfOne(expansions,0))
	--elseif side[1] == 0 and side[2] == 1 then
	--	table.remove(expansions,indexOfOne(expansions,1))
	--elseif side[1] == -1 and side[2] == 0 then
	--	table.remove(expansions,indexOfOne(expansions,2))
	--elseif side[1] == 0 and side[2] == -1 then
	--	table.remove(expansions,indexOfOne(expansions,3))
	--end
	
	
	local steps = roomSize
	
	if corridor == 1 then steps = corridorSize end
	
	for i = 1, steps do
		if #expansions == 0 then 
			break 
		end
		
		local xd = 0
		local yd = 0
		local wd = 0
		local hd = 0
		local side = choose(expansions)
		if(side == 0) then xd=-1;wd=1 
		elseif(side == 1) then yd=-1;hd=1
		elseif(side == 2) then wd=1
		elseif(side == 3) then hd=1 end
		
		if CheckRoom(rx+xd,ry+yd,rw+wd,rh+hd,roomId) then
			rx= rx+xd
			ry= ry+yd
			rw= rw+wd
			rh= rh+hd
		else
			table.remove(expansions, indexOfOne(expansions, side))
		end
	end
	
	if(rw > 1 or rh > 1) then
		MakeRoom(rx, ry, rw, rh, null, roomId)
	end
	if(rw==0 or rh == 0) then corridor=1 end
	
	return {x=rx, y=ry, w=rw, h=rh, corridor=corridor}
end

--Cava o mapa
function Dig()
	carvedARoom = 0
	if digs == 0 then
		rw = math.random(2,8) -- room width
		rh = math.random(2,8) -- room height
		MakeRoom(w/2-rw/2, h/2-rh/2, rw, rh, null, roomId) -- Meio do mapa
		rw = math.random(2,8)
		rh = math.random(2,8)
		--Cria outra pra possibilitar formato irregular?
		MakeRoom(w/2-rw/2, h/2-rh/2, rw, rh, null, roomId)
		rooms[roomId] = {parent=-1, gen=1, corridor=0}
		roomId = roomId+1
		carvedARoom=1
	else
		wall = PickWall()
		if not wall then stuck = stuck+1 
		else
			--Pega sala pertencente a parece
			parentRoom = rooms[GetMapRoom(wall[1], wall[2])]
			local sides = {}
			if(GetMapGfx((wall[1])-1, wall[2]) == TILE_WALL) then sides[#sides+1] = {-1,0} end
			if(GetMapGfx((wall[1])+1, wall[2]) == TILE_WALL) then sides[#sides+1] = {1,0} end
			if(GetMapGfx(wall[1], (wall[2])-1) == TILE_WALL) then sides[#sides+1] = {0,-1} end
			if(GetMapGfx(wall[1], (wall[2])+1) == TILE_WALL) then sides[#sides+1] = {0,1} end
			--Pega espa?o adjacente n?o cavado
			local side = choose(sides)
			if not side then stuck = stuck+1 
			else
				local gen = parentRoom.gen+1
				MakeRoom(wall[1],wall[2],0,0, TILE_DOOR, roomId) --Door
				table.remove(walls, indexOf(walls, wall[1],wall[2]))
				
				local room = Carve(wall, side, roomId)
				thisIsACorridor = room.corridor
				
				if math.random() < 0.95 then --Branch
					for i=0, 1 do
						local wall = PickWall(room.x-1, room.y-1, room.w+2, room.h+2)
						
						if wall then
							local sides = {}
							sides = {{-1,0},{1,0},{0,-1},{0,1}}
							local side = choose(sides)
							if(side) then
								local room = Carve(wall, side, roomId)
								if room.corridor == 0 then thisIsACorridor = 0 end
							end
						end
					end
				end
				
				rooms[roomId] = {parent=parentRoom, gen=gen, corridor=thisIsACorridor}
				roomId = roomId+1
				carvedARoom=1
				
			end
		end
	end
	
	digs = digs+1
	
	if(tilesDug < (w*h)*percentToFill and stuck < tooMuchStuck) then
		_=[[
			remover para instant 
		]]
		sleep(drawTime)
		Dig()
	else
		if mirror == 1 then
			for x=0, w/2 do
				for y=0, h/2 do
					SetMap(w-x-1,y,GetMapGfx(x,y), GetMapType(x,y))
					SetMap(x,h-y-1,GetMapGfx(x,y), GetMapType(x,y))
					SetMap(w-x-1,h-y-1,GetMapGfx(x,y), GetMapType(x,y))
				end
			end
		end
		
		local highest = 0
		local highestId = 0
		for i=1, #rooms do
			if rooms[i].gen > highest and rooms[i].corridor == 0 then
				highest=rooms[i].gen
				highestId = i
			end
		end
		
		local spot = RandomIn(0)
		SetMap(spot[1], spot[2], TILE_START, Tile.START)
		
		local spot = RandomIn(highestId)
		SetMap(spot[1], spot[2], TILE_END, Tile.END)
		
	end
	
end

---------------------------- EXECUTION CODE ( MAIN )
--Create map
map:createMap(w,h, Tile.BLOCK, TILE_WALL)

Dig();
_=[[
-------------- SIMULATION
MakeRoom(5,5,10,10,1,1)
rooms[1] = {parent=-1, gen=1, corridor=0}
--MakeRoom(17,5,10,10,1,2)
--rooms[2] = {parent=-1, gen=1, corridor=0}
--MakeRoom(8,17,10,10,1,3)
--rooms[3] = {parent=-1, gen=1, corridor=0}




wall = PickWall()

parentRoom = rooms[GetMapRoom(wall[1], wall[2])]

--if wall then 
--	MakeRoom(wall[1],wall[2],0,0, 7, roomId)
--end
local sides = {}
if(GetMapGfx((wall[1])-1, wall[2]) == TILE_WALL) then sides[#sides+1] = {-1,0} end
if(GetMapGfx((wall[1])+1, wall[2]) == TILE_WALL) then sides[#sides+1] = {1,0} end
if(GetMapGfx(wall[1], (wall[2])-1) == TILE_WALL) then sides[#sides+1] = {0,-1} end
if(GetMapGfx(wall[1], (wall[2])+1) == TILE_WALL) then sides[#sides+1] = {0,1} end

local side = choose(sides)
if not side then 
	stuck = stuck+1 
	SetMap(1, GetMapGfx((wall[1])-1, wall[2]), 4, Tile.BLOCK)
else
	local gen = parentRoom.gen+1
	MakeRoom(wall[1],wall[2],0,0, TILE_DOOR, roomId) --Door
	table.remove(walls, indexOf(walls, wall[1],wall[2]))
	
	local room = Carve(wall, side, roomId)
	
	thisIsACorridor = room.corridor
	
	--SetMap(room.x, room.y, 8,1)
	--SetMap(room.x+room.w, room.y, 8,1)
	--SetMap(room.x, room.y+room.h, 8,1)
	--SetMap(room.x+room.w, room.y+room.h, 8,1)
	
	if math.random() < 1 then --Branch
		for i=0, 1 do
			local wall = PickWall(room.x-1, room.y-1, room.w+3, room.h+3)

			
			if wall then
				local sides = {}
				if(GetMapGfx((wall[1])-1, wall[2]) == TILE_WALL) then sides[#sides+1] = {-1,0} end
				if(GetMapGfx((wall[1])+1, wall[2]) == TILE_WALL) then sides[#sides+1] = {1,0} end
				if(GetMapGfx(wall[1], (wall[2])-1) == TILE_WALL) then sides[#sides+1] = {0,-1} end
				if(GetMapGfx(wall[1], (wall[2])+1) == TILE_WALL) then sides[#sides+1] = {0,1} end
				local side = choose(sides)
				SetMap(wall[1],wall[2],8,1)
				if(side) then
					local room = Carve(wall, side, roomId)
					if room.corridor == 0 then thisIsACorridor = 0 end
				end
			end
		end
	end
	
	
	rooms[roomId] = {parent=parentRoom, gen=gen, corridor=thisIsACorridor}
	roomId = roomId+1
	carvedARoom=1
end
]]
			
