	local g = golly()

	function string:split(delimiter)
	  local result = { }
	  local from  = 1
	  local delim_from, delim_to = string.find( self, delimiter, from  )
	  while delim_from do
		table.insert( result, string.sub( self, from , delim_from-1 ) )
		from  = delim_to + 1
		delim_from, delim_to = string.find( self, delimiter, from  )
	  end
	  table.insert( result, string.sub( self, from  ) )
	  return result
	end


	glds = {}

	glds[0] = g.parse("3o$o$bo!")
	glds[1] = g.evolve(g.parse("3o$o$bo!"), 2)
	--blck = g.parse("6bo$5bobo$5bobo$6bo2$b2o7b2o$o2bo5bo2bo$b2o7b2o2$6bo$5bobo$5bobo$6bo!", -7, -8)
	blck = g.parse("2o$2o!", -4, 0)

	function place_glider(i, dx)
		if i%2 == 0 then
			g.putcells(glds[0], 15 * math.floor(i/2) + dx, 15 * math.floor(i/2))
		else
			g.putcells(glds[1], 8 + 15 * math.floor(i/2) + dx, 8 + 15 * math.floor(i/2))
		end 
	end 

	function validate_recipe(res)
		g.new("")
		g.putcells(blck)
		
		for i = 1, #res do 
			if res[i] == '1' then
				place_glider(i, 0)
			end
		end 
		
		--place_glider(#res + 2, 0)
		--place_glider(#res + 11, 0)
		
		g.setbase(8)
		g.setstep(5)
		g.step()
		
		rect = g.getrect()
		
		if #rect ~= 0 then 
			
			local x = rect[1]
			local y = rect[2]
			
			if x - y == -4 then 
				return y
			--if tonumber(g.getpop()) < 18 then 
			--	return false
			end 
		end 
		
		return 10000 
		
	end

	g.new("")
	file = 'C:\\Users\\SimSim314\\Documents\\GitHub\\Glue\\Glue++\\P30_results_375.txt'
	lines = {}

	resipes = {}
	idx = 0 

	for line in io.lines(file) do 
		res = line:split(',')
		
		x = validate_recipe(res)
		if x ~= 10000 then
			if resipes[x] == nil then 
				resipes[x] = res
			end 
		end 
		
		g.show(idx)
		idx = idx + 1

	end

	g.new("")

	dx = 0
	for i = 1, 200 do
		res = resipes[i - 100]
		
		if res ~= nil then 
			g.putcells(blck, dx)
			
			for i = 1, #res do 
				if res[i] == '1' then
					place_glider(i, dx)
				end
			end 
		end 
		
		dx = dx + 100
		
	end 
