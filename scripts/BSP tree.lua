-- Baseado no artigo:
-- http://gamedevelopment.tutsplus.com/tutorials/how-to-use-bsp-trees-to-generate-game-maps--gamedev-12268
MIN_LEAF_SIZE = 6
MAX_LEAF_SIZE = 20

drawTime = 10
--Parametros possiveis
--		Chance de dividir salas horizontal 
-- 		Tamanho das salas internas


coolDebug = false

Leaf = {}
Leaf.__index = Leaf

function Leaf.create(x, y, width, height)
   local leaf = {}             -- our new object
   setmetatable(leaf,Leaf)  -- make Account handle lookup
   
	 leaf.x = x
	 leaf.y = y
	 leaf.width = width
	 leaf.height = height
   return leaf
end

function Leaf:split()
	-- Split leaf into 2 children
  if (self.leftChild ~= null or self.rightChild ~= null) then
		return false -- Already split!
	end
	
	--Determine direction of split
	--If width >25% larger than height, vertical split
	--If height >25% larger than width, horizontal
	local splitH = (math.random() > 0.5)
	if( self.width > self.height and (self.height / self.width) >= 0.05) then
		splitH = false
	elseif( self.height > self.width and (self.width / self.height) >= 0.05) then
	  splitH = true
  end
	
	local max
	if splitH then
		max = self.height
	else
		max = self.width
	end
	
	--Menor divis?o de leaf: 6
	max = max-MIN_LEAF_SIZE
	
	--Area muito pequena para dividir mais uma vez
	if(max <= MIN_LEAF_SIZE) then
		return false
	end
	
	local split = math.random(MIN_LEAF_SIZE, max)
	if(splitH) then
		self.leftChild = Leaf.create(self.x, self.y, self.width, split)
		self.rightChild = Leaf.create(self.x, self.y + split, self.width, self.height - split)
	else
		self.leftChild = Leaf.create(self.x, self.y, split, self.height)
		self.rightChild = Leaf.create(self.x + split, self.y, self.width - split, self.height)
	end
	
	return true --Splitted
end

function Leaf:draw(tileGfx, tileType)
	if (tileGfx == null) then tileGfx = 1 end
	if (tileType == null) then tileType = 1 end
	for i=0, self.width-1 do
		for j=0, self.height-1 do
			map:setTile(self.x+i,self.y+j,tileGfx,tileType)
		end
	end
	
	sleep(drawTime)
end
function Leaf:drawLines(tileGfx, tileType)
	if (tileGfx == null) then tileGfx = 1 end
	if (tileType == null) then tileType = 1 end
	for i=0, self.width-1 do
		for j=0, self.height-1 do
			if i == 0 or j == 0 or i == self.width-1 or j == self.height-1 then 
				map:setTile(self.x+i,self.y+j,tileGfx,tileType)
			end
		end
	end
	sleep(100)
end

function Leaf.x()
	return self.x
end

function Leaf.y()
	return self.y
end

function Leaf.width()
	return self.width
end

function Leaf.height()
	return self.height
end

function Leaf:createRooms()
	--Gera salas e corredores
	if( self.leftChild ~= null or self.rightChild ~= null) then
		--Foi cortada, vai para filhas
		if(self.leftChild ~= null) then
			self.leftChild:createRooms()
		end
		if(self.rightChild ~= null) then
			self.rightChild:createRooms()
		end
		
		if self.leftChild ~= null and self.rightChild ~= null then
			self:createHall(self.leftChild:getRoom(), self.rightChild:getRoom())
		end
	else
		--Pronta para fazer a sala
		local sizeW;
		local sizeH;
		local posX;
		local posY;
		
		--Sala pode ser do tamanho 3x3 at? o tamanho da left-2
		local sizeW = math.random(3, self.width-2);
		local sizeH = math.random(3, self.height-2);
		
		--Coloca a sela dentro da leaf mas n?o encostado, para n?o mergir salas
		local posX  = math.random(1, self.width - sizeW-1);
		local posY  = math.random(1, self.height - sizeH-1);
		
		if coolDebug == true then
			self:drawLines(4)
		end
		self.room = Leaf.create(self.x+posX, self.y+posY, sizeW, sizeH)
		self.room:draw()
		
	end
end

function Leaf:getRoom()
	--Itera todos as leafs para achar a sala
	if self.room ~= null then
		return self.room
	else
		local lRoom
		local rRoom
		if self.leftChild ~= null then
			lRoom = self.leftChild:getRoom()
		end
		if self.rightChild ~= null then
			rRoom = self.rightChild:getRoom()
		end
		
		--Retorna o leaf com a sala , caso ambos tenham sala, retorna aleat?rio um deles
		if lRoom == null and rRoom == null then
			return null
		elseif rRoom == null then
			return lRoom
		elseif lRoom == null then
			return rRoom
		elseif math.random() > 0.5 then
			return lRoom
		else
			return rRoom
		end
		
	end
end

function Leaf:createHall(l, r)
	--Conecta as duas salas
	local point1X =  math.random(l.x + 1, l.x+l.width-2)
	local point1Y =  math.random(l.y + 1, l.y+l.height-2)
	local point2X =  math.random(r.x + 1, r.x+r.width-2)
	local point2Y =  math.random(r.y + 1, r.y+r.height-2)
	
	local w       = point2X - point1X
	local h	      = point2Y - point1Y
	
	local halls = {}
	
	if w < 0 then
		if h < 0 then
			--Alterna caminhos , ex: esquerda cima,  cima esquerda
			if math.random() > 0.5 then
				halls[#halls+1] = Leaf.create(point2X, point1Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point2X, point2Y, 1, math.abs(h))
			else
				halls[#halls+1] = Leaf.create(point2X, point2Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point1X, point2Y, 1, math.abs(h))
			end
		elseif h > 0 then
			--Alterna caminhos , ex: esquerda cima,  cima esquerda
			if math.random() > 0.5 then
				halls[#halls+1] = Leaf.create(point2X, point1Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point2X, point1Y, 1, math.abs(h))
			else
				halls[#halls+1] = Leaf.create(point2X, point2Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point1X, point1Y, 1, math.abs(h))
			end
		else -- if h == 0
				halls[#halls+1] = Leaf.create(point2X, point2Y, math.abs(w), 1)
		end
	elseif w > 0 then
		if h < 0 then
			--Alterna caminhos , ex: esquerda cima,  cima esquerda
			if math.random() > 0.5 then
				halls[#halls+1] = Leaf.create(point1X, point2Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point1X, point2Y, 1, math.abs(h))
			else
				halls[#halls+1] = Leaf.create(point1X, point1Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point2X, point2Y, 1, math.abs(h))
			end
		elseif h > 0 then
			--Alterna caminhos , ex: esquerda cima,  cima esquerda
			if math.random() > 0.5 then
				halls[#halls+1] = Leaf.create(point1X, point1Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point2X, point1Y, 1, math.abs(h))
			else
				halls[#halls+1] = Leaf.create(point1X, point2Y, math.abs(w),1)
				halls[#halls+1] = Leaf.create(point1X, point1Y, 1, math.abs(h))
			end
		else -- if h == 0
				halls[#halls+1] = Leaf.create(point1X, point1Y, math.abs(w), 1)
		end
	else -- if w == 0 
		if h < 0 then
			halls[#halls+1] = Leaf.create(point2X, point2Y, 1, math.abs(h))
		elseif h > 0 then
			halls[#halls+1] = Leaf.create(point1X, point1Y, 1, math.abs(h))
		end
	end
	
	for i, hall in ipairs(halls) do
		hall:draw()
	end
	
end

--Create map
map:createMap(30,30, Tile.BLOCK, 30)

-- Create leafs
local _leafs = {}
local root = Leaf.create(0,0,map.w,map.h)
_leafs[1] = root



local did_split = true
while did_split do
	did_split = false
	
	--map:setTile(0,1,1,1)
	--map:setTile(0+i,0,1,1)
	for i, l in ipairs(_leafs) do
		if(l.leftChild == null and l.rightChild == null) then
			--Se leaf muito grande ou 75% chance
			if(l.width > MAX_LEAF_SIZE or l.height > MAX_LEAF_SIZE or math.random() > 0.25) then
				if(l:split()) then
					_leafs[#_leafs+1] = l.leftChild
					_leafs[#_leafs+1] = l.rightChild
					did_split = true
				end
			end
		end
	end	
end

root:createRooms()


--root:draw()
-- create and use an Leaf
--leaf = Leaf.create(0,0,13,13)

--if leaf:split() then
--	map:createMap(10,10);
--else 
--	map:createMap(1,1);
--end
