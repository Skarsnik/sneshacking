local donald_x
local donald_y
local old_donald_x = 0
local old_donald_y = 0

local camera_x
local camera_y

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
    local calc_pos = client.transformPoint(text_x, text_y)
    gui.text(calc_pos['x'], calc_pos['y'], str, color)
  end
end

-- End of Bizhawk compatibility layer
-----------------------------------------------

function function_sprite()
-- 0x0F85 tab seems to start here, then the game check for every $20 pos for a new thing
-- 0 is used to read C5A9A1-A4 
-- if 1 is 05 it probably skip the thing. 10E5 is the end?
-- type 5 dmg donald
-- type 6 can be dmg
-- type 2 taken dmg, invul?
-- 2 is invul timer ?
-- 3 is one byte
-- 5 is 2 bytes
-- 6 is HP
-- B is Y
-- E is X (2 byte)

-- 16 hp initial?

-- hit box info
--C5A94F $B9 $00 $00     LDA $0000,Y [000F85] = $0582    A:0082 X:0D35 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizC V:29  H:251 FC:38587
--C5A952 $29 $FF $00     AND #$00FF                      A:0582 X:0D35 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizC V:29  H:261 FC:38587
--C5A955 $0A             ASL                             A:0082 X:0D35 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizC V:29  H:266 FC:38587
--C5A956 $0A             ASL                             A:0104 X:0D35 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizc V:29  H:269 FC:38587
--C5A957 $AA             TAX                             A:0208 X:0D35 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizc V:29  H:272 FC:38587
--C5A958 $E2 $20         SEP #$20                        A:0208 X:0208 Y:0F85 S:0185 D:0000 DB:00 P:nvmxdizc V:29  H:275 FC:38587
--C5A95A $BF $A1 $A9 $C5 LDA $C5A9A1,X [C5ABA9] = $50   

  memory.usememorydomain("System Bus")
  for i = 0, 10 do
    local offset = i * 0x20 + 0xF85
    local type = memory.read_u8(offset + 1)
    local timer1 = memory.read_u8(offset + 2)
    local x = memory.read_s16_le(offset + 0xE)
    local y = memory.read_s16_le(offset + 0xB)
    local hp = memory.read_u8(offset + 6)
    local b0 = memory.read_u8(offset)
    local w5 = memory.read_s16_le(offset + 5)
    local timer2 = memory.read_u8(offset + 0x14)
    local offsetx = memory.read_u16_le(0xC5A9A1 + b0)
    local offsety = memory.read_u16_le(0xC5A9A3 + b0)


    b0 = bit.lshift(b0, 2)
    if type > 0 then
        gui.drawBox(x - 16 - camera_x, y - camera_y, x + 16 - camera_x, y - 32 - camera_y, 0xFFFFFFFF, 0x7777FFFF)
        DrawNiceText(x + 16 - camera_x + 2, y - 32 - camera_y, string.format("Type:%02X", type))
        DrawNiceText(x + 16 - camera_x + 2, y - 32 - camera_y + 8, string.format("HP:%02d", hp))
        DrawNiceText(x + 16 - camera_x + 2, y - 32 - camera_y + 16, string.format("%02d - %02d - %02d - %02d", memory.read_s8(0xC5A9A1 + b0), memory.read_s8(0xC5A9A2 + b0), memory.read_s8(0xC5A9A3 + b0), memory.read_s8(0xC5A9A4 + b0)))
        DrawNiceText(x + 16 - camera_x + 2, y - 32 - camera_y + 24, string.format("T:%d,%d", timer1, timer2))
        --console.write(string.format("%X - %X ; %d - %d\n", offsetx, offsety, offsetx, offsety))
    end
  end

  --for j = 0, 4 do
  --  for i = 0,0x20 do1
  --    DrawNiceText(20 + j * 30, 20 + 5 * i, string.format("%02X : %02X", i, memory.read_u8(0x0F85 + j * 0x20 + i)))
  --  end
  --end
end


function my_draw()
    donald_x = memory.read_s16_le(0x0B2E)
    donald_y = memory.read_s16_le(0x0B2B)

    camera_x = memory.read_s16_le(0x0091)
    camera_y = memory.read_s16_le(0x0094)
    local speed = memory.read_u8(0x0E0E)
    -- local speeds = memroy.read_u8(0x0E0D)

    local hitbox_x1 = donald_x - 16 - camera_x
    local hitbox_x2 = donald_x + 16 - camera_x
    local hitbox_y1 = donald_y - camera_y
    local hitbox_y2 = donald_y - 32 - camera_y
    gui.drawBox(hitbox_x1, hitbox_y1, hitbox_x2, hitbox_y2, 0xFFFFFFFF, 0x7777FFFF)
    DrawNiceText(hitbox_x2 + 2, hitbox_y2, string.format("x:%04d,y:%04d", donald_x, donald_y))
    DrawNiceText(hitbox_x2 + 2, hitbox_y2 + 8, string.format("HSpeed: %01d", speed))
    DrawNiceText(hitbox_x2 + 2, hitbox_y2 + 16, string.format("CHSpeed: %01d", donald_x - old_donald_x))
    DrawNiceText(hitbox_x2 + 2, hitbox_y2 + 24, string.format("CVpeed: %01d", donald_y - old_donald_y))
    old_donald_x = donald_x
    old_donald_y = donald_y

    DrawNiceText(200, 0, string.format("Lag count : %d", emu.lagcount()))


    -- 3 is likely invu timer


    for i = 0,4 do
      DrawNiceText(150 + i * 20, 200, string.format("%d , ", memory.read_u8(0x0E0D + i)))
    end
    --local enemy_x = memory.read_s16_le(0x1005 + 0xE)
    --local enemy_y = memory.read_s16_le(0x1005 + 0xB)
    --DrawNiceText(200, 20, string.format("X : %04d", enemy_x))
    --DrawNiceText(200, 30, string.format("Y : %04d", enemy_y))
    --DrawNiceText(200, 40, string.format("HP : %02d", memory.read_u8(0x1005 + 6)))
    --gui.drawBox(enemy_x - 16 - camera_x, enemy_y - camera_y, enemy_x + 16 - camera_x, enemy_y - 32 - camera_y, 0xFFFFFFFF, 0x7777FFFF)
    function_sprite()

end

while true do
    my_draw()
    emu.frameadvance()
  end