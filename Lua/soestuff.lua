-- configuration --
show_tiles = false
show_mapdata = true
<<<<<<< HEAD
show_sprite_data = true
=======
show_sprite_data = false
show_lag = true
show_lag_details = true
show_actual_market_time = false
FPS = 60.098475521 -- 50.0069789082 for PAL
>>>>>>> 2973fc5215ccc99c7e159421ca1e542eb3f3a9ec
pos_fmt = "%d,%d"
--pos_fmt = "%3X,%3X"
lag_fmt = "Lag: %.1fs" -- change to .3f to get milliseconds
det_lag_fmt = "Lag: %d=%3.1fs" -- this shows frames and seconds
ext_lag_fmt = "Lag: %d+%d=%3.1fs" -- this includes missed VSync interrupts in arg2
-- end of configuration --


-- implement required bitops for mesen from
-- https://github.com/AlberTajuelo/bitop-lua/
if not bit then
  bit = {}
  local floor = math.floor
  local function memoize(f)
    local mt = {}
    local t = setmetatable({}, mt)
    function mt:__index(k)
      local v = f(k)
      t[k] = v
      return v
    end
    return t
  end

  local function make_bitop_uncached(t, m)
    local function bitop(a, b)
      local res,p = 0,1
      while a ~= 0 and b ~= 0 do
        local am, bm = a%m, b%m
        res = res + t[am][bm]*p
        a = (a - am) / m
        b = (b - bm) / m
        p = p*m
      end
      res = res + (a+b) * p
      return res
    end
    return bitop
  end
  
  local function make_bitop(t)
    local op1 = make_bitop_uncached(t, 2^1)
    local op2 = memoize(function(a)
      return memoize(function(b)
        return op1(a, b)
      end)
    end)
    return make_bitop_uncached(op2, 2^(t.n or 1))
  end
  bit.bxor = make_bitop {[0]={[0]=0,[1]=1},[1]={[0]=1,[1]=0}, n=4}
  bit.band = function(a,b) return ((a+b) - bit.bxor(a,b))/2 end
  bit.rshift = function(a,disp)
    if disp < 0 then return bit.lshift(a,-disp) end
    return floor(a % 2^32 / 2^disp)
  end
  bit.lshift = function(a,disp)
    if disp < 0 then return rshift(a,-disp) end
    return (a * 2^disp) % 2^32
  end
  bit.bswap = function(x)
    local a = band(x, 0xff); x = rshift(x, 8)
    local b = band(x, 0xff); x = rshift(x, 8)
    local c = band(x, 0xff); x = rshift(x, 8)
    local d = band(x, 0xff)
    return lshift(lshift(lshift(a, 8) + b, 8) + c, 8) + d
  end
  bit.rrotate = function(x, disp)
    disp = disp % 32
    local low = bit.band(x, 2^disp-1)
    return bit.rshift(x, disp) + bit.lshift(low, 32-disp)
  end
  bit.lrotate = function(x, disp)
    return bit.rrotate(x, -disp)
  end
  bit.rol = bit.lrotate
  bit.ror = bit.rrotate
end

-- default fontsize
local fontw = 1
local fonth = 1

-----------------------------------------------
-- snes9x Bizhawk compatibility layer by Nethraz
-- + mesen compatibility layer by black_sliver
if emu and bizstring == nil then
  -- detect mesen by existance of 'emu' and absence of bizstring
  fontw = 2 -- font is bigger in mesen
  fonth = 2 -- font is bigger in mesen
  is_mesen = true
  memory = {}
  gui = {}
  event = {}
  local decode_addr = function(addr)
    return addr, emu.memType.cpu
  end
  memory.usememorydomain = function()
    -- mesen works differently
  end
  memory.read_u8 = function(addr)
    local addr,t = decode_addr(addr)
    return emu.read(addr, t, false)
  end
  memory.read_s8 = function(addr)
    local addr,t = decode_addr(addr)
    return emu.read(addr, t, true)
  end
  memory.read_u16_le = function(addr)
    local addr,t = decode_addr(addr)
    return emu.read(addr, t, false) + 0x100*emu.read(addr+1, t, false)
  end
  memory.read_s16_le = function(addr)
    local addr,t = decode_addr(addr)
    return emu.readWord(addr, t, true)
  end
  memory.read_u24_le = function(addr)
    local addr,t = decode_addr(addr)
    return (emu.read(addr, t, false) + 0x100*emu.read(addr+1, t, false)
           + 0x10000*emu.read(addr+2, t, false))
  end
  memory.read_s24_le = function(addr)
    local val = memory.read_u24_le(addr)
    if (val > 0x7fffff) then val = val - 0x800000 - 0x800000 end
    return val
  end
  memory.read_u32_le = function(addr)
    local addr,t = decode_addr(addr)
    return (emu.read(addr, t, false) + 0x100*emu.read(addr+1, t, false)
           + 0x10000*emu.read(addr+2, t, false)
           + 0x1000000*emu.read(addr+3, t, false))
  end
  memory.read_s32_le = function(addr)
    local val = memory.read_u32_le(addr)
    if (val > 0x7fffffff) then val = val - 0x80000000 - 0x80000000 end
    return val
  end
  memory.read_u16_be = function(addr) return bit.rshift(bit.bswap(memory.read_u16_le(addr)),16) end
  memory.readbyterange = function(addr,len)
    res = {}
    local addr,t = decode_addr(addr)
    for i=0,len-1 do
      res[i] = emu.read(addr+i, t, false)
    end
    return res
  end
  local color_b2m = function(bizhawk_color)
    -- if numeric then same as bizhawk but alpha is inverse
    if bizhawk_color == nil then return nil end
    return bit.band(bizhawk_color,0x00ffffff)+(0xff000000-bit.band(bizhawk_color,0xff000000))
  end
  gui.drawText = function(x,y,text,color)
    emu.drawString(x,y,text,color_b2m(color))
  end
  gui.text = gui.drawText -- ???
  gui.drawLine = function(x1,y1,x2,y2,color)
    emu.drawLine(x1,y1,x2,y2,color_b2m(color))
  end
  gui.drawRectangle = function(x,y,w,h,outline_color,fill_color)
    if outline_color == fill_color then
      emu.drawRectangle(x,y,w,h,color_b2m(outline_color),true)
    elseif color_b2m(fill_color) then
      emu.drawRectangle(x,y,w,h,color_b2m(outline_color),false)
      emu.drawRectangle(x+1,y+1,w-2,h-2,color_b2m(fill_color),true)
    else
      emu.drawRectangle(x,y,w,h,color_b2m(outline_color),false)
    end
  end
  gui.drawBox = function(x1,y1,x2,y2,outline_color,fill_color)
    if x2<x1 then local tmp=x1 ; x1=x2 ; x2=tmp end
    if y2<y1 then local tmp=y1 ; y1=y2 ; y2=tmp end
    return gui.drawRectangle(x1,y1,x2-x1+1,y2-y1+1,outline_color,fill_color)
  end
  
  event.onframeend = function(f)
    emu.addEventCallback(f, emu.eventType.endFrame)
  end

elseif not event then
  -- detect snes9x by absence of 'event'
  is_snes9x = true
  memory.usememorydomain = function()
    -- snes9x always uses "System Bus" domain, which cannot be switched
  end
  memory.read_u8 = memory.readbyte
  memory.read_s8 = memory.readbytesigned
  memory.read_u16_le = memory.readword
  memory.read_s16_le = memory.readwordsigned
  memory.read_u32_le = memory.readdword
  memory.read_s32_le = memory.readdwordsigned
  memory.read_u24_le = function(addr) return memory.read_u16_le(addr) + 0x10000*memory.read_u8(addr+2) end
  memory.read_u16_be = function(addr) return bit.rshift(bit.bswap(memory.read_u16_le(addr)),16) end
  local color_b2s = function(bizhawk_color)
    if bizhawk_color == nil then return nil end
    return bit.rol(bizhawk_color,8)
  end
  gui.drawText = function(x,y,text,color)
    gui.text(x,y,text,color_b2s(color))
  end
  gui.drawLine = function(x1,y1,x2,y2,color)
    gui.line(x1,y1,x2,y2,color_b2s(color))
  end
  gui.drawBox = function(x1,y1,x2,y2,outline_color,fill_color)
    gui.box(x1,y1,x2,y2,color_b2s(fill_color),color_b2s(outline_color))
  end
  event = {}
  event.onframeend = function(luaf,name)
    local on_gui_update_old = gui.register()
    local function on_gui_update_new()
      if on_gui_update_old then
        on_gui_update_old()
      end
      luaf()
    end
    gui.register(on_gui_update_new)
  end

else
  is_bizhawk = true
end

function DrawNiceText(text_x, text_y, str, color)
  --local sh = client.screenheight
  --local sw = client.screenwidth
  if is_snes9x or is_mesen then 
    gui.text(text_x, text_y, str, color)
  else
    local calc_x = client.transformPointX(text_x)
    local calc_y = client.transformPointY(text_y)
    gui.text(calc_x, calc_y, str, color)
  end
end

-- End of Bizhawk compatibility layer
-----------------------------------------------



memory.usememorydomain("System Bus")

local map_id
local camera_x
local camera_y
local trig_off_x
local trig_off_y
local frames0
local timer0
local emuframes = 0
local market_running = false


local function gameDrawBox(x1, y1, x2, y2, color1, color2)
  --console.writeline("x1 : "..x1.." : y1 : "..y1)
  gui.drawBox(x1 - camera_x, y1 - camera_y, x2 - camera_x, y2 - camera_y, color1, color2)
end

local function gameDrawText(x, y, text, color)
   if color == nil then -- default to freen
      if mesen then color=0x7700ff00 else color=0xff00ff00 end
   end
   DrawNiceText(x - camera_x, y - camera_y, text, color)
end


local stype_to_name = {
  [0x0A70] = "Le_Dog",
  [0x0A26] = "Le_Boy",
  [0xD4D2] = "Bone Snake",
  [0xD6D8] = "Maggot",
  [0xD722] = "Moskito"
}


-- 7E3DE5 to 7E4E88 = Monster/NPC data for the current room. Each Monster/NPC gets x8E bytes of data.
                  -- x00-x02 = Sprite/Animation script pointer
                  -- x03-x05 = Sprite/Animation script pointer
                  -- x06-x07 = Partial pointer to a ROM location with Monster/NPC data / likely gfx index
                  -- x1A-x1B = X position on map
                  -- x1C-x1D = Y position on map
                  -- x22-x23 = Used to determine the direction creature is facing
                  -- x24-x25 = Pointer to ram address of the target of the monster's attacks
                  -- x2A-x2B = Hit Points
                  -- x2E-x2F = Charge level for attack.
                  -- x5E-x5F = Pointer to data structure of next valid entity.  0 if end of list.
                  -- x60-x61 = Used to identify the type of monster/npc
                  -- x62-x63 = X,Y position on the map in terms of tiles
                  -- x66-x67 = Dialog/Event indicator
                  -- x68-x69 = Flags that determine when Event specified in x66-x67 is triggered
                            -- (0040 = occurs when player presses talk/activate in proximity to NPC)
                            -- (0100 = occurs when NPC takes damage)
                            -- (0200 = occurs when NPC is killed)
                  -- x76-x77 = damage taken from last attack

				  
local function load_sprite(idx)
	local sprite = {
     name = "none",
	   index = idx,
	   anim_ptr1 = memory.read_u24_le(0x7E0000 + idx),
	   anim_ptr2 = memory.read_u24_le(0x7E0000 + idx + 3),
	   rom_ptr = memory.read_u16_le(0x7E0000 + idx + 6),
	   unknow = {},
	   --unknown[1] = memory.read_u16_le(0x7E0000 + idx + 0x8), -- 00 
	   --unknown[2] = memory.read_u16_le(0x7E0000 + idx + 0xA), -- 00 when moving
	   --unknown[3] = memory.read_u16_le(0x7E0000 + idx + 0xC),
	   --unknown[4] = memory.read_u16_le(0x7E0000 + idx + 0xE), -- counter
	   pos_x = memory.read_u16_le(0x7E0000 + idx + 0x1A),
	   pos_y = memory.read_u16_le(0x7E0000 + idx + 0x1C),
	   --unknown[5] = memory.read_u16_le(0x7E0000 + idx + 0x1E),
	   z_pos = memory.read_u16_le(0x7E0000 + idx + 0x1E),
	   --unknown[6] = memory.read_u16_le(0x7E0000 + idx + 0x20),
	   direction = memory.read_u16_le(0x7E0000 + idx + 0x22),
	   target = memory.read_u16_le(0x7E0000 + idx + 0x24),
	   --unknown[7] = memory.read_u16_le(0x7E0000 + idx + 0x26),
	   --unknown[8] = memory.read_u16_le(0x7E0000 + idx + 0x28),
	   hp = memory.read_u16_le(0x7E0000 + idx + 0x2A),
	   --unknown[9] = memory.read_u16_le(0x7E0000 + idx + 0x2C),
	   charge_lvl = memory.read_u16_le(0x7E0000 + idx + 0x2E),
	   --unknown[10] = memory.read_u16_le(0x7E0000 + idx + 0x30),
	   --unknown[11] = memory.read_u16_le(0x7E0000 + idx + 0x32),
	   --unknown[12] = memory.read_u16_le(0x7E0000 + idx + 0x34),
	   --unknown[13] = memory.read_u16_le(0x7E0000 + idx + 0x36),
	   ptr_next = memory.read_u16_le(0x7E0000 + idx + 0x5E),
	   stype = memory.read_u16_le(0x7E0000 + idx + 0x60),
	   x_tile = memory.read_u8(0x7E0000 + idx + 0x62),
	   y_tile = memory.read_u8(0x7E0000 + idx + 0x63),
	   diagev = memory.read_u16_le(0x7E0000 + idx + 0x66),
	   diagev_flag = memory.read_u16_le(0x7E0000 + idx + 0x68),
	   dmg_taken = memory.read_u16_le(0x7E0000 + idx + 0x76)
  }
  local piko = memory.read_u24_le(0xCE0000 + sprite['stype'])
  --console.writeline(string.format("%04X %06X", sprite['stype'] + 0xCE0000 + 0xB678 - 0xA26, piko))
  local bytesname = memory.readbyterange(piko, 32)
  local strname = ''
  for i=0, (32 - 1) do
     --console.write(string.format("%d :  %d", bytesname[i], tonumber(bytesname[i], 16)))
     --console.write('-')
     if bytesname[i] == 0 then
        break
     end
     strname = strname .. string.char(bytesname[i])
  end
  --console.writeline(strname)
  --console.writeline(memory.read_u16_le(0xCE0000 + sprite['stype'] + 0xD))
  sprite['name'] = strname
	sprite['unknow'][1] = memory.read_u16_le(0x7E0000 + idx + 0x8) -- 00
	sprite['unknow'][2] = memory.read_u16_le(0x7E0000 + idx + 0xA)
	sprite['unknow'][3] = memory.read_u16_le(0x7E0000 + idx + 0xC)
	sprite['unknow'][4] = memory.read_u16_le(0x7E0000 + idx + 0xE) -- counter
	sprite['unknow'][5] = memory.read_u16_le(0x7E0000 + idx + 0x1E)
	sprite['unknow'][6] = memory.read_u16_le(0x7E0000 + idx + 0x20)
	sprite['unknow'][7] = memory.read_u16_le(0x7E0000 + idx + 0x26)
	sprite['unknow'][8] = memory.read_u16_le(0x7E0000 + idx + 0x28)
	sprite['unknow'][9] = memory.read_u16_le(0x7E0000 + idx + 0x2C)
	for i = 0, 23 do
	   sprite['unknow'][i + 10] = memory.read_u16_le(0x7E0000 + idx + 0x30 + i * 2)
	end
	sprite['unknow'][34] = memory.read_u16_le(0x7E0000 + idx + 0x6A)
	sprite['unknow'][35] = memory.read_u16_le(0x7E0000 + idx + 0x6C)
	sprite['unknow'][36] = memory.read_u16_le(0x7E0000 + idx + 0x6E)
	sprite['unknow'][37] = memory.read_u16_le(0x7E0000 + idx + 0x70)
	sprite['unknow'][38] = memory.read_u16_le(0x7E0000 + idx + 0x72)
	sprite['unknow'][39] = memory.read_u16_le(0x7E0000 + idx + 0x74)
	sprite['unknow'][40] = memory.read_u16_le(0x7E0000 + idx + 0x78)
	sprite['unknow'][41] = memory.read_u16_le(0x7E0000 + idx + 0x7A)
	sprite['unknow'][42] = memory.read_u16_le(0x7E0000 + idx + 0x7C)
	sprite['unknow'][43] = memory.read_u16_le(0x7E0000 + idx + 0x7E)
	sprite['unknow'][44] = memory.read_u16_le(0x7E0000 + idx + 0x80)
	sprite['unknow'][45] = memory.read_u16_le(0x7E0000 + idx + 0x82)
	sprite['unknow'][46] = memory.read_u16_le(0x7E0000 + idx + 0x84)
	sprite['unknow'][47] = memory.read_u16_le(0x7E0000 + idx + 0x86)
	sprite['unknow'][48] = memory.read_u16_le(0x7E0000 + idx + 0x88)
	sprite['unknow'][49] = memory.read_u16_le(0x7E0000 + idx + 0x8A)
	sprite['unknow'][50] = memory.read_u16_le(0x7E0000 + idx + 0x8C)
	sprite['unknow'][51] = memory.read_u16_le(0x7E0000 + idx + 0x8E)
	
	
	-- print_sprite_info(idx)
	return sprite
end

local function print_sprite_info(idx)
    console.writeline(string.format("=====%04X=====", idx))
	for i = 0, 8 do
	   local bytes = memory.readbyterange(0x7E0000 + idx + i * 16, 16)
	   for k,v in pairs(bytes) do
         console.write(string.format("%02X ", v))
       end
	   console.writeline("")
	end
end

local function load_all_sprites()
    local sprites = {}
	local sprite_start = memory.read_u16_le(0x7E3DDF)
    repeat
		local sprite = load_sprite(sprite_start)
        table.insert(sprites, sprite)
		sprite_start = sprite['ptr_next']
    until (sprite['ptr_next'] == 0)
    return sprites
end

local function load_trigger(ptr)
  return {
    pos_y = (memory.read_u8(ptr+0)-trig_off_y)*16,
    pos_x = (memory.read_u8(ptr+1)-trig_off_x)*16,
    h = (memory.read_u8(ptr+2)-memory.read_u8(ptr+0)-1)*16,
    w = (memory.read_u8(ptr+3)-memory.read_u8(ptr+1)-1)*16,
    script = memory.read_u16_le(ptr+4)
  }
end

local function load_all_btriggers()
  triggers = {}
  local dataptr = memory.read_u24_le(0x9ffde7 + map_id*4);
  --local mscriptlistptr = dataptr+0x0d+2;
  local mscriptlistlen = memory.read_u16_le(dataptr+0x0d);
  local bscriptlistptr = dataptr+0x0d+2+mscriptlistlen+2;
  local bscriptlistlen = memory.read_u16_le(dataptr+0x0d+2+mscriptlistlen);
  for ptr=bscriptlistptr,bscriptlistptr+bscriptlistlen-1,6 do
    table.insert(triggers, load_trigger(ptr))
  end
  return triggers
end

local function load_all_steptriggers()
  triggers = {}
  local dataptr = memory.read_u24_le(0x9ffde7 + map_id*4);
  local mscriptlistptr = dataptr+0x0d+2;
  local mscriptlistlen = memory.read_u16_le(dataptr+0x0d);
  for ptr=mscriptlistptr,mscriptlistptr+mscriptlistlen-1,6 do
    table.insert(triggers, load_trigger(ptr))
  end
  return triggers
end

local function draw_map()
  local startx = -(camera_x % 16)
  local starty = -(camera_y % 16)
  local box_x1 = startx
  local box_y1 = starty
  while box_x1 < 260 do
    box_y1 = starty
    while box_y1 < 230 do
      gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 16, 0xCCFF0000, 0x11FF0000)
	  --gui.drawBox(box_x1, box_y1 + 1, box_x1+16, box_y1 + 16 + 1, 0xCC0000FF, 0x11FF0000)
    box_y1 = box_y1 + 16
    end
    box_x1 = box_x1 + 16
  end
end


local draw_boy_pos = function()
  local boy_x = memory.read_s16_le(0x7E4EA3)
  local boy_y = memory.read_s16_le(0x7E4EA5)
  gameDrawBox(boy_x - 8, boy_y - 8, boy_x + 8, boy_y + 8, 0xFFFFFFFF, 0x7777FFFF)
end

local function draw_sprites()
    sprites = load_all_sprites()
    gui.text(0, 0, string.format("Number of sprites:%2d",#sprites))
    for i, sprite in pairs(sprites) do
        if show_sprite_data then
            gui.text(0, 50 + i * 30, string.format("%d|%X - stype : %04X/%04X - pos[% 4d, % 4d] - HP: %02d", 
                     i, sprite['index'], sprite['stype'], sprite['rom_ptr'], sprite['pos_x'], sprite['pos_y'], sprite['hp']))
                        
            local tmpstr = ""
            for j = 33, 40 do
                tmpstr = tmpstr .. string.format("|%d:%04X", j, sprite['unknow'][j])
            end
            gui.text(0, 50 + (i * 2 + 1) * 15, tmpstr)
        end
        gameDrawBox(sprite['pos_x'] - 8*fontw , sprite['pos_y'] - 8*fonth, sprite['pos_x'] + 8 , sprite['pos_y'] + 8, 0xFFFFFFFF, 0x7777FFFF)
        gameDrawText(sprite['pos_x'] - 8*fontw, sprite['pos_y'] - 4*fonth, sprite['name'])
        gameDrawText(sprite['pos_x'] + 8*fontw, sprite['pos_y'] - 8*fonth, string.format("%04X", sprite['stype']))
        gameDrawText(sprite['pos_x'] + 8*fontw, sprite['pos_y'] - 4*fonth, string.format(pos_fmt, sprite['pos_x'], sprite['pos_y']))
        gameDrawText(sprite['pos_x'] - 8*fontw, sprite['pos_y'] - 8*fonth, string.format("%04X", sprite['index']))
    end
end

local function draw_triggers(triggers, color)
  for i, trigger in pairs(triggers) do
    gameDrawBox(trigger['pos_x'], trigger['pos_y'], trigger['pos_x']+trigger['w']+15, trigger['pos_y']+trigger['h']+15,
                0xFF000000+color, 0x77000000+color)
  end
end

local function draw_btriggers()
  local triggers = load_all_btriggers()
  return draw_triggers(triggers, 0xffff00)
end

local function draw_steptriggers()
  local triggers = load_all_steptriggers()
  return draw_triggers(triggers, 0xff00ff)
end

local function seconds2str(t)
   local mn = math.floor(t/60)
   local sc = math.floor(t-60*mn)
   t = math.floor((t- 60*mn - sc)*10)
   return string.format("%02d:%02d.%d", mn,sc,t)
end

local function draw_timing()
   local market = memory.read_u16_le(0x7E2513);
   local timer  = memory.read_u24_le(0x7E0B19);
   local frames = memory.read_u24_le(0x7E0100);
   local dTimer = (timer-timer0)
   if market>0 then -- market timer started
      if not market_running then
        emuframes = 0
        frames0 = frames
        timer0 = timer
        dTimer = 0
        market_running = true
      end
      if bit.band(memory.read_u8(0x7e225f),0x20)==0 then -- not vigor dead
         local s = "Market: "
         if bit.band(memory.read_u8(0x7e225d),0x08)==0x08 then -- market timer expired
            s = s.."00:00.0"
         else
            s = s..seconds2str((bit.band(0xffff,market-timer+0xc4e0))/FPS)
         end
         if show_actual_market_time then
            s = s.."/"..seconds2str((0xc4e0+(emuframes-dTimer))/FPS)
         end
         if show_lag then gui.text(0,213,s) else gui.text(0,222,s) end
      else
         market_running = false
      end
   else
      market_running = false
   end
   if show_lag then
      local dFrames = (frames-frames0)
      -- NOTE: depending on where we start the script, we may have a diff of 1 in frame count
      if emuframes-dFrames > 1 and show_lag_details then
         gui.text(0, 222, string.format(ext_lag_fmt,
            dFrames-dTimer, emuframes-dFrames, (emuframes-dTimer)/FPS))
      elseif show_lag_details then
         gui.text(0, 222, string.format(det_lag_fmt,
            dFrames-dTimer, (dFrames-dTimer)/FPS))
      else
         gui.text(0, 222, string.format(lag_fmt,
            (emuframes-dTimer)/FPS))
      end
   end
end

local my_draw = function()
   map_id = memory.read_u8(0x7E0ADB);
   camera_x = memory.read_s16_le(0x7E0112)
   camera_y = memory.read_s16_le(0x7E0114)
   trig_off_x = memory.read_s16_le(0x7E0F86)
   trig_off_y = memory.read_s16_le(0x7E0F88)
   if show_tiles then draw_map() end -- draw map first as text is not always-on-top in mesen
   draw_btriggers()
   draw_steptriggers()
   draw_boy_pos()
   draw_sprites()
   draw_timing()
   if show_mapdata then gui.text(0, 231, string.format("Map ID: %02x, Trig Off: %02x %02x", map_id, trig_off_x, trig_off_y)) end
   emuframes = emuframes+1
end

frames0 = memory.read_u24_le(0x7E0100);
timer0  = memory.read_u24_le(0x7E0B19);

if is_snes9x or is_mesen then
  event.onframeend(my_draw)
else
  while true do
    my_draw()
    emu.frameadvance()
  end
end
