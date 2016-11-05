-- Color are AARRGGB format
local Mrnut_hitbox_color = 0x7777FFFF
local Mrnut_hitbox_color_outline = 0xFFFFFFFF
local Enemy_hitbox_color = 0x77FF7777
local Enemy_hitbox_color_outline = 0xFFFFFFFF
local Nuts_hitbox_color = 0x00FF00
local Coins_hitbox_color = 0xFFFF00
local Line_to_nuts_color = 0x77FFFFFF
local Line_to_exit_color = 0xFFFF5522
local Line_to_enemy_color = 0xFF000000
local show_enemy_direction = 1
local show_nuts_direction = 1

local prev_input
local saved_pos_x = 0
local saved_pos_y = 0


-----------------------------------------------
-- Bizhawk compatibility layer by Nethraz

if not event then
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
end

function DrawNiceText(text_x, text_y, str, color)
  --local sh = client.screenheight
  --local sw = client.screenwidth
  if is_snes9x then 
    gui.text(text_x, text_y, str, color)
  else
    local calc_x = client.transformPointX(text_x)
    local calc_y = client.transformPointY(text_y)
    gui.text(calc_x, calc_y, str, color)
  end
end

-- End of Bizhawk compatibility layer
-----------------------------------------------

function showmap()
  local camx = memory.read_s16_le(0x0CE)
  local camy = memory.read_s16_le(0x0D0)
  
  local startx = -(camx % 16)
  local starty = -(camy % 16)
  
  local px = memory.read_s16_le(0x0037)
  local py = memory.read_s16_le(0x0039)
  local player_x1 = memory.read_s16_le(0x18A8)
  local player_y1 = memory.read_s16_le(0x18AA)
  local player_x2 = memory.read_s16_le(0x18AC)
  local player_y2 = memory.read_s16_le(0x18AE)
  local v_direction = memory.read_s8(0x006D)
  
  local hitbox_x1 = px + 20 - camx
  local hitbox_x2 = px + 36 - camx
  local hitbox_y1 = py - 36 - camy
  local hitbox_y2 = py - 4 - camy
  
  gui.drawBox(hitbox_x1, hitbox_y1, hitbox_x2, hitbox_y2, Mrnut_hitbox_color_outline, Mrnut_hitbox_color )
  
  local COLLISION_NONE = 0
  local COLLISION_FULL = 2
  local COLLISION_UP = 1
  local COLLISION_45_SLOPE_RIGHT = 0x10
  local COLLISION_45_SLOPE_LEFT = 0x12
  
  
  local box_x1 = startx
  local box_y1 = starty
  --DrawNiceText(150, 150, camx.." "..camx % 16 .."| "..startx)
  while box_x1 < 260 do
    box_y1 = starty
    while box_y1 < 230 do
      --gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 16, 0xCCFF0000, 0x11FF0000)
	  --gui.drawBox(box_x1, box_y1 + 1, box_x1+16, box_y1 + 16 + 1, 0xCC0000FF, 0x11FF0000)
	  local stuff = 2*bit.rshift(camx + box_x1 + 1, 4) + mainmemory.read_u16_le(0x0829 + 2*bit.rshift(camy + box_y1 + 1, 4))
	  local stuffand = bit.band(mainmemory.read_u16_le(0x2000+stuff), 0x03FF)
	  local weird = bit.band(mainmemory.read_u16_le(0x2000+stuff), 0xFC00)
	  local incollision = mainmemory.read_u16_le(0xF400 + stuffand);
	  local collision_base_type = bit.band(incollision, 0x00FF)
	  local colcolor = 0xFF0000
	  if collision_base_type ~= 0 then
	     local know_type = false
	     if weird == 0x1800 then
	       colcolor = 0x00FF00
		   know_type = true
	     end
	     if collision_base_type == COLLISION_FULL then
		   gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 16, 0xCC000000 + colcolor, 0x55000000 + colcolor)
		   know_type = true
		 end
		 if collision_base_type == COLLISION_UP then
		   gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 16, 0x88000000 + colcolor, 0x55000000 + colcolor)
		   gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 8, 0xCC000000 + colcolor, 0x55000000 + colcolor)
		   know_type = true
		 end
		 if collision_base_type == COLLISION_45_SLOPE_RIGHT then
		   local points = {{box_x1, box_y1 + 16}, {box_x1 + 16, box_y1}, {box_x1 + 16, box_y1 + 16}, {box_x1, box_y1 + 16}}
		   gui.drawPolygon(points, 0xCC000000 + colcolor, 0x55000000 + colcolor)
		   know_type = true
		   --gui.drawLine(box_x1, box_y1 + 16, box_x1 + 16, box_y1, 0xCCFF0000)
		   --gui.drawLine(box_x1, box_y1 + 16, box_x1 + 16, box_y1 + 16, 0xCCFF0000)
		   --gui.drawLine(box_x1 + 16, box_y1 + 16, box_x1 + 16, box_y1, 0xCCFF0000)
		   
		 end
		 if collision_base_type == COLLISION_45_SLOPE_LEFT then
		   local points = {{box_x1, box_y1}, {box_x1 + 16, box_y1 + 16}, {box_x1, box_y1 + 16}, {box_x1, box_y1}}
		   gui.drawPolygon(points, 0xCC000000 + colcolor, 0x55000000 + colcolor)
		   know_type = true
		 end
		 if know_type == false then
		   gui.drawBox(box_x1, box_y1, box_x1+16, box_y1 + 16, 0xCC000000 + colcolor, 0x55000000 + colcolor)
		   DrawNiceText(box_x1 + 5, box_y1 + 10, string.format("%x", bit.band(incollision, 0x00FF)), 0xFF0000FF)
		 end
	  end
	  box_y1 = box_y1 + 16
	end
	box_x1 = box_x1 + 16
  end
  if v_direction == 0 then
    gui.drawLine(hitbox_x1, hitbox_y2, hitbox_x2, hitbox_y2, 0xFF00FF00) 
  end
end


local hex4 = function(number)
  return string.format("%04X", number)
end

local prev_px = 0
local prev_spx = 0
local prev_py = 0

local draw_cross = function(camx, camy, posx, posy)
  gui.drawLine(posx - 2 - camx, posy - camy, posx + 2 - camx, posy - camy, 0xFF0088FF)
  gui.drawLine(posx - camx , posy - 2 - camy, posx - camx, posy + 2 - camy, 0xFF0088FF)
  gui.drawPixel(posx - camx, posy - camy, 0xFF00FF00)
end

local my_draw = function()

  showmap()
  memory.usememorydomain("System Bus")

  local hspeed = memory.read_s16_le(0x0058)
  local jump_speed = memory.read_s16_le(0x0066)
  local fall_speed = memory.read_s16_le(0x0060)
  local v_direction = memory.read_s8(0x006D)
  local vspeed_text = "0"
  if v_direction > 0 then
    vspeed_text = jump_speed .. " (jump)"
  elseif v_direction < 0 then
    vspeed_text = "-" .. fall_speed .. " (fall)"
  end

  local subposx = mainmemory.read_u32_le(0x005A);
  local posx = mainmemory.read_s16_le(0x0037)
  local posy = mainmemory.read_s16_le(0x0039)
  local nuts = memory.read_u16_le(0x0A29)
  local player_x1 = memory.read_s16_le(0x18A8)
  local player_y1 = memory.read_s16_le(0x18AA)
  local player_x2 = memory.read_s16_le(0x18AC)
  local player_y2 = memory.read_s16_le(0x18AE)
  local skill_points = memory.read_u16_le(0x189E)
  local camx = memory.read_s16_le(0x0CE)
  local camy = memory.read_s16_le(0x0D0)
  local hitbox_x1 = player_x1 - camx
  local hitbox_x2 = player_x2 - camx
  local hitbox_y1 = player_y1 - camy
  local hitbox_y2 = player_y2 - camy

  -- Count all Collectibles
  local collected_nuts_count = 0
  local total_nuts_count = 0
  local sprite_scroll_table = 0x7F8000 + memory.read_u16_le(0x7F8002)
  for i = 0,2047 do
    local sprite_entry_A = sprite_scroll_table + 6*i

    local sprite_x = memory.read_s16_le(sprite_entry_A + 0)
    if sprite_x >= 0x7E00 then break end
    local sprite_flags_A = memory.read_s16_le(sprite_entry_A + 2)
    local sprite_index = memory.read_u16_le(sprite_entry_A + 4)
    local sprite_entry_B = 0x7FA000 + 8*sprite_index
    
    local sprite_enabled = memory.read_u8(sprite_entry_B + 0)
    local sprite_type = memory.read_u8(sprite_entry_B + 1)
    local sprite_x = memory.read_s16_le(sprite_entry_B + 4)
    local sprite_y = memory.read_s16_le(sprite_entry_B + 6)
    
    if sprite_type == 6 then -- Nut
      total_nuts_count = total_nuts_count + 1
      if sprite_enabled == 0xFF then
        collected_nuts_count = collected_nuts_count + 1
      else
	    if show_nuts_direction then
          gui.drawLine((hitbox_x1+hitbox_x2)/2,(hitbox_y1+hitbox_y2)/2,sprite_x+8-camx,sprite_y+8-camy,  Line_to_nuts_color)
		end
      end
    elseif sprite_type == 4 then -- Exit
      local exit_info = 0x7F8000 + sprite_x
      local exit_x1 = memory.read_s16_le(exit_info + 0)
      local exit_y1 = memory.read_s16_le(exit_info + 2)
      local exit_x2 = memory.read_s16_le(exit_info + 4)
      local exit_y2 = memory.read_s16_le(exit_info + 6)
      --local exit_type = memory.read_s16_le(exit_info + 8)
      
      gui.drawLine((hitbox_x1+hitbox_x2)/2,(hitbox_y1+hitbox_y2)/2,exit_x1-camx,exit_y2-camy, Line_to_exit_color)
    end
  end

  -- Draw Enemy hitboxes
  for i=0,31 do
    local enemy_info = 0x05A3 + 0x40*i
    local enemy_bounds = 0x0A77 + 0xC*i
    if (memory.read_u16_le(enemy_bounds) == 0) then break end
    
    local enemy_sprite = memory.read_u16_le(enemy_info+4)
    local enemy_hp = memory.read_s8(enemy_info+0x2D) + 1

    local enemy_x1 = memory.read_s16_le(enemy_bounds+2)
    local enemy_y1 = memory.read_s16_le(enemy_bounds+4)
    local enemy_x2 = memory.read_s16_le(enemy_bounds+6)
    local enemy_y2 = memory.read_s16_le(enemy_bounds+8)
    
    gui.drawBox(enemy_x1-camx, enemy_y1-camy, enemy_x2-camx, enemy_y2-camy, Enemy_hitbox_color_outline, Enemy_hitbox_color)
	--console.writeline(hex4(enemy_sprite))
    DrawNiceText(enemy_x2-camx+3, enemy_y1-camy-16, hex4(enemy_sprite))
    DrawNiceText(enemy_x2-camx+3, enemy_y1-camy- 8, enemy_hp .. " HP")
	if show_enemy_direction then
	  gui.drawLine((hitbox_x1+hitbox_x2)/2,(hitbox_y1+hitbox_y2)/2, (enemy_x1+enemy_x2)/2 - camx,  (enemy_y1+enemy_y2)/2 - camy, Line_to_enemy_color)
	end
  end

  -- Draw Collectible hitbox
  local last_collec_entry = memory.read_u16_le(0x1068)
  for i = 0,127 do
    local collec_entry = 0x106A + 0x14*i
    if collec_entry >= last_collec_entry then break end
    
    local collec_x1 = memory.read_s16_le(collec_entry + 0x06)
    local collec_x2 = collec_x1 + memory.read_s16_le(collec_entry + 0x10)
    local collec_y1 = memory.read_s16_le(collec_entry + 0x08)
    local collec_y2 = collec_y1 + memory.read_s16_le(collec_entry + 0x12)
    local collec_sprite_ptr = memory.read_u16_le(collec_entry + 0x0E)
    local collec_sprite_info_0 = memory.read_u16_be(0x7F0000 + collec_sprite_ptr)
    local collec_type = bit.band(memory.read_u16_le(collec_entry+4),0xF)
    
    if collec_type == 0 or collec_type == 1 then
      local collec_color = 0xFF0000
      if collec_type == 1 then
        collec_color = Nuts_hitbox_color   -- Nut
      elseif collec_type == 0 then
        collec_color = Coins_hitbox_color   -- Coin
      end
      
      gui.drawBox(collec_x1-camx, collec_y1-camy, collec_x2-camx, collec_y2-camy, 0xFF000000 + collec_color, 0x77000000 + collec_color)
      --DrawNiceText(collec_x2-camx, collec_y1-camy, hex4(collec_sprite_ptr)
      --   .. ", x:" .. collec_sprite_load_x .. ", y:" .. collec_sprite_load_y)
      DrawNiceText(collec_x2-camx, collec_y1-camy, hex4(collec_sprite_ptr) .. ": " .. hex4(collec_sprite_info_0))
    end
  end

  -- Draw Exits
  for i=0,32 do
    local exit_entry = 0x18F7 + 0xC*i
    local exit_type = memory.read_u16_le(exit_entry + 0)
    if exit_type == 0 then break end
    local exit_x1 = memory.read_s16_le(exit_entry + 2)
    local exit_y1 = memory.read_s16_le(exit_entry + 4)
    local exit_x2 = memory.read_s16_le(exit_entry + 6)
    local exit_y2 = memory.read_s16_le(exit_entry + 8)
    
    gui.drawBox(exit_x1-camx,exit_y1-camy,exit_x2-camx,exit_y2-camy,0xFF7777FF,0x770000FF)
  end

  -- Draw Mr.Nutz hitbox
  gui.drawBox(hitbox_x1, hitbox_y1, hitbox_x2, hitbox_y2, Mrnut_hitbox_color_outline, Mrnut_hitbox_color )
  
  -- Draw Mr. Nutz x/y "center" or pos? kinda weird
  draw_cross(camx, camy, posx, posy)
  
  DrawNiceText(160, 150, "HSpeed: " .. hspeed)
  DrawNiceText(160, 155, "VSpeed: " .. vspeed_text)
  DrawNiceText(160, 160, "CSpeedx : "..player_x1 - prev_px);
  DrawNiceText(160, 165, "CSSpeedx : "..subposx - prev_spx);
  DrawNiceText(160, 170, "Pos X: " .. posx)
  DrawNiceText(160, 174, "Pos Y: " .. posy)
  DrawNiceText(160, 190, "Nuts: " .. collected_nuts_count .. "/" ..  total_nuts_count)
  DrawNiceText(160, 198, "Sk.Point: " .. hex4(skill_points))
  prev_px = player_x1
  prev_spx = subposx
end

if is_snes9x then
  event.onframeend(my_draw)
else
  while true do
    my_draw()
	prev_input = input.get()
    emu.frameadvance()
  end
end
