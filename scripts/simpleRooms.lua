-- Baseado em um video de Rachel Morris
-- http://www.youtube.com/watch?v=XlSlKwZit8g:

drawTime = 10
--Parametros possiveis
--		Numero de salas
-- 		Tamanho das salas 
--		Largura e Altura das salas

n_Salas = 4
tamanho = 3
min_width = 3
max_width = 3
map_w = 40
map_h = 40

TILE_FLOOR = 3
TILE_CENTER = 0
TILE_END = 4
TILE_START = 2
coolDebug = true

Sala = {}
Sala.__index = Sala

function Sala.create(x, y)
   local sala = {}             -- our new object
   setmetatable(sala,Sala)  -- make Account handle lookup
   
	 sala.x = x
	 sala.y = y
   return sala
end

function isChance(chance)
	mChance = chance/100;
	if mChance > math.random() then
		return true
	else 
		return false
	end
end

map:createMap(map_w, map_h, Tile.BLOCK, 1)

mSalas = {}

--Cria salas
for i = 1, n_Salas do
	s = Sala.create( math.floor(math.random() * map_w), math.floor(math.random() * map_h) )
	mSalas[#mSalas+1] = s;
	map:setTile(s.x,s.y,4,1)
	if coolDebug then sleep(drawTime) end
end

--Liga salas
for i = 1, #mSalas-1 do
	--Origen
	n_x = mSalas[i].x
	n_y = mSalas[i].y
	--Destino
	d_x = mSalas[i+1].x
	d_y = mSalas[i+1].y
	--Cria caminho em X	
	while n_x ~= d_x do
		if n_x < d_x then
			n_x = n_x+1
		elseif n_x > d_x then
			n_x = n_x-1
		end
		if n_x < 0 or n_x >= map_w then
			break
		end
		map:setTile(n_x, n_y, 5, Tile.PASS)
	end
	if coolDebug then sleep(drawTime) end
	--Cria caminho em Y
	while n_y ~= d_y do
		if n_y < d_y then
			n_y = n_y+1
		elseif n_y > d_y then
			n_y = n_y-1
		end
		if n_y < 0 or n_y >= map_h then
			break
		end
		map:setTile(n_x, n_y, 5, Tile.PASS)	
		if isChance(20.0) then
			goldCreated = math.floor(math.random() * 100)
			res:addGold(n_x, n_y, goldCreated)
		end
	end
	if coolDebug then sleep(drawTime) end
end

--Enlarga salas
for i = 1, #mSalas do
	left_w = min_width + math.floor(math.random() * max_width)
	right_w = min_width + math.floor(math.random() * max_width)
	up_w = min_width + math.floor(math.random() * max_width)
	bottom_w = min_width + math.floor(math.random() * max_width)
	
	startX = mSalas[i].x - left_w
	for ix = 0, left_w+right_w do 
		if startX + ix < 0 then
			--Continue
		elseif startX + ix >= map_w then
			break;
		else
			startY = mSalas[i].y-up_w
			for iy = 0, up_w+bottom_w do
				if startY+iy < 0 then
					--Continue
				elseif startY+iy >= map_h then
					break
				end
				map:setTile(startX+ix, startY+iy, TILE_FLOOR, Tile.PASS)
			end
		end
	end
		if coolDebug then sleep(drawTime) end	
end

for i = 2, #mSalas-1 do
	map:setTile(mSalas[i].x, mSalas[i].y,TILE_CENTER, Tile.PASS)
	if coolDebug then sleep(drawTime) end	
end
map:setTile(mSalas[1].x, mSalas[1].y,TILE_START, Tile.START)
if coolDebug then sleep(drawTime) end	
map:setTile(mSalas[#mSalas].x, mSalas[#mSalas].y,TILE_END, Tile.END)
if coolDebug then sleep(drawTime) end	

