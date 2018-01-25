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
  memory.read_u24_le = function(addr)  end
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



memory.usememorydomain("System Bus")

local camera_x
local camera_y


				  
local function gameDrawBox(x1, y1, x2, y2, color1, color2)
  --console.writeline("x1 : "..x1.." : y1 : "..y1)
  gui.drawBox(x1 - camera_x, y1 - camera_y, x2 - camera_x, y2 - camera_y, color1, color2)
end

local function gameDrawText(x, y, text)
   DrawNiceText(x - camera_x, y - camera_y, text, 0xFF00FF00)
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
                  -- x06-x07 = Partial pointer to a ROM location with Monster/NPC data
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
	   index = idx,
	   anim_ptr1 = memory.read_u24_le(0x7E0000 + idx),
	   anim_ptr2 = memory.read_u24_le(0x7E0000 + idx + 3),
	   rom_ptr = memory.read_u16_le(0x7E0000 + idx + 6),
	   unknown1 = memory.read_u16_le(0x7E0000 + idx + 0x8),
	   unknown2 = memory.read_u16_le(0x7E0000 + idx + 0xA),
	   unknown3 = memory.read_u16_le(0x7E0000 + idx + 0xC),
	   unknown4 = memory.read_u16_le(0x7E0000 + idx + 0xE),
	   pos_x = memory.read_u16_le(0x7E0000 + idx + 0x1A),
	   pos_y = memory.read_u16_le(0x7E0000 + idx + 0x1C),
	   unknown5 = memory.read_u16_le(0x7E0000 + idx + 0x1E),
	   z_pos = memory.read_u16_le(0x7E0000 + idx + 0x1E),
	   unknown6 = memory.read_u16_le(0x7E0000 + idx + 0x20),
	   direction = memory.read_u16_le(0x7E0000 + idx + 0x22),
	   target = memory.read_u16_le(0x7E0000 + idx + 0x24),
	   unknown7 = memory.read_u16_le(0x7E0000 + idx + 0x26),
	   unknown8 = memory.read_u16_le(0x7E0000 + idx + 0x28),
	   hp = memory.read_u16_le(0x7E0000 + idx + 0x2A),
	   unknown9 = memory.read_u16_le(0x7E0000 + idx + 0x2C),
	   charge_lvl = memory.read_u16_le(0x7E0000 + idx + 0x2E),
	   ptr_next = memory.read_u16_le(0x7E0000 + idx + 0x5E),
	   stype = memory.read_u16_le(0x7E0000 + idx + 0x60),
	   x_tile = memory.read_u8(0x7E0000 + idx + 0x62),
	   y_tile = memory.read_u8(0x7E0000 + idx + 0x63),
	   diagev = memory.read_u16_le(0x7E0000 + idx + 0x66),
	   diagev_flag = memory.read_u16_le(0x7E0000 + idx + 0x68),
	   dmg_taken = memory.read_u16_le(0x7E0000 + idx + 0x76)
	}
	return sprite
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



local draw_boy_pos = function()
    local boy_x = memory.read_s16_le(0x7E4EA3)
	local boy_y = memory.read_s16_le(0x7E4EA5)
	gameDrawBox(boy_x - 8, boy_y - 8, boy_x + 8, boy_y + 8, 0xFFFFFFFF, 0x7777FFFF)
end

local	function draw_sprites()
	sprites = load_all_sprites()
	gui.text(0, 0, "number of sprites : "..table.getn(sprites))
	for i, sprite in pairs(sprites) do
	    gui.text(0, 50 + i * 12, string.format("%d|%X - stype : %04X - pos[% 4d, % 4d] - HP: %02d | U1:%04X U2:%04X U3:%04X U4:%04X U5:%04X U6:%04X U7:%04X U8:%04X U9:%04X", i, 
		               sprite['index'], sprite['stype'], sprite['pos_x'], sprite['pos_y'], sprite['hp'], sprite['unknown1'], sprite['unknown2'], sprite['unknown3'], sprite['unknown4'],
					   sprite['unknown5'], sprite['unknown6'], sprite['unknown7'], sprite['unknown8'], sprite['unknown9'] ))
        gameDrawBox(sprite['pos_x'] - 8 , sprite['pos_y'] - 8, sprite['pos_x'] + 8 , sprite['pos_y'] + 8, 0xFFFFFFFF, 0x7777FFFF)
		gameDrawText(sprite['pos_x'] + 8, sprite['pos_y'] - 8, string.format("%04X", sprite['stype']))
		gameDrawText(sprite['pos_x'] + 8, sprite['pos_y'] - 4, string.format("%d,%d", sprite['pos_x'], sprite['pos_y']))
		gameDrawText(sprite['pos_x'] - 8, sprite['pos_y'] - 8, string.format("%04X", sprite['index']))
    end
end

local my_draw = function()
   camera_x = memory.read_s16_le(0x7E0112)
   camera_y = memory.read_s16_le(0x7E0110)
   draw_boy_pos()
   draw_sprites()
end

if is_snes9x then
  event.onframeend(my_draw)
else
  while true do
    my_draw()
    emu.frameadvance()
  end
end