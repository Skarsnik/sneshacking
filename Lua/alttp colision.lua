local camera_x
local camera_y
local link_x
local link_y


-- $0114
-- $6C[0x01] - Indicates whether you are standing in a doorway


--$66[0x01] - Indicates the last direction Link moved towards.
  --	          Valuewise: 0 - Up, 1 - Down, 2 - Left, 3 - Right

--$67[0x01] - Indicates which direction Link is walking (even if not going anywhere)
            --bitwise: 0000abcd. a - Up, b - Down, c - Left, d - Right
-- $2F[0x01] - (Player)   0 - up   2 - down   4 - left   6 - right.

-----------------------------------------------
-- Bizhawk compatibility layer by Nethraz

if not event then
  -- detect snes9x by absence of 'event'
  is_snes9x = true
  memory.usememorydomain = function()
    -- snes9x always uses "System Bus" domain, which cannot be switched
  end
  mainmemory = {plop = 0}
  console = {plop = 0}
  console.writeline = print
  memory.read_u8 = memory.readbyte
  memory.read_s8 = memory.readbytesigned
  memory.read_u16_le = memory.readword
  memory.read_s16_le = memory.readwordsigned
  memory.read_u32_le = memory.readdword
  memory.read_s32_le = memory.readdwordsigned
  memory.read_u16_be = function(addr) return bit.bswap(memory.read_u16_le(addr)) >> 16 end
  mainmemory.read_u8 = function(addr) return memory.readbyte( 0x7E0000 | addr) end
  mainmemory.read_s8 = function(addr) return memory.readbytesigned(0x7E0000 | addr) end
  mainmemory.read_u16_le = function(addr) return memory.readword(0x7E0000 | addr) end
  mainmemory.read_s16_le = function(addr) return memory.readwordsigned(0x7E0000 | addr) end
  mainmemory.read_u32_le = function(addr) return memory.readdword(0x7E0000 | addr) end
  mainmemory.read_s32_le = function(addr) return memory.readdwordsigned(0x7E0000 | addr) end
  local color_b2s = function(bizhawk_color)
    if bizhawk_color == nil then return nil end
    return bit.rol(bizhawk_color,8)
  end
  gui.drawText = function(x,y,text,color)
    gui.text(x,y,text,color_b2s(color))
  end
  gui.drawPixel = function(x, y, color)
    gui.pixel(x, y, color_b2s(color))
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
    local transformed = client.transformPoint(text_x, text_y)
    gui.text(transformed["x"], transformed["y"], str, color)
  end
end

-- End of Bizhawk compatibility layer
-----------------------------------------------			
			
local log = function(fmt, ...)
    console.writeline(string.format(fmt, ...))
end

local hex4 = function(number)
  return string.format("%04X", number)
end

-- Taken from Bank6 #6300
local table_size_hitbox1 = {
  12,   1,  16,  20,  20,   8,   4,  32,
  48,  24,  32,  32,  32,  48,  12,  12,    
  60, 124,  12,  32,   4,  12,  48,  32,
  40,   8,  24,  24,   5,  80,   4,   8
}

local table_size_hitbox2 = {
  14,   1,  16,  21,  24,   4,   8,  40,
  20,  24,  40,  29,  36,  48,  60, 124,     
  12,  12,  17,  28,   4,   2,  28,  20,
  10,   4,  24,  16,   5,  48,   8,  12
}

local table_x_offsets_low = {
  2,   3,   0,  -3,  -6,   0,   2,  -8,
  0,  -4,  -8,   0,  -8, -16,   2,   2,       
  2,   2,   2,  -8,   2,   2, -16,  -8,
  -12,   4,  -4, -12,   5, -32,  -2,   4
}

local table_x_offsets_high = {
  0,  0,  0, -1, -1,  0,  0, -1,
  0, -1, -1,  0, -1, -1,  0,  0,    
  0,  0,  0, -1,  0,  0, -1, -1,
  -1,  0, -1, -1,  0, -1, -1,  0
}

local table_y_offsets_low = {
  0,   3,   4,  -4,  -8,   2,   0, -16,
  12,  -4,  -8,   0, -10, -16,   2,   2,
  2,   2,  -3, -12,   2,  10,   0, -12,
  16,   4,  -4, -12,   3, -16,  -8,  10
}

local table_y_offsets_high = {
  0,  0,  0, -1, -1,  0,  0, -1,
  0, -1, -1,  0, -1, -1,  0,  0,      
  0,  0, -1, -1,  0,  0,  0, -1,
  0,  0, -1, -1,  0, -1, -1,  0,
}

local type_to_name = {
    -- 0x00 
    "Raven",
    "Vulture",
    "Flying Stalfos Head",
    "Unused",
    "Good Switch being pulled",
    "Some other sort of switch being pulled, but from above?",
    "Bad Switch",
    "switch again (facing up)",
    "Octorock",
    "Giant Moldorm",
    "Four Shooter Octorock",
    "Chicken / Chicken Transformed into Lady",
    "Octorock",
    "Normal Buzzblob / Morphed Buzzblob (tra la la... look for Sahashrala)",
    "Plants with big mouths",
    "Octoballoon (The thing that explodes into 10 others)",

    -- 0x10 
    "Octobaby (Baby Octorocks from the Otobaloon)",
    "Hinox (Bomb-chucking one-eyed giant)",
    "Moblin",
    "Helmasaur (small variety)",
    "Thieves' Town (AKA Gargoyle's Domain) Grate",
    "Bubble (AKA Fire Faerie)",
    "Sahashrala / Aginah, sage of the desert",
    "Rupee Crab under bush / rock",
    "Moldorm",
    "Poe",
    "Dwarf, Mallet, and the shrapnel from it hitting",
    "Arrow shot by solder / stuck in wall? Spear thrown by Moblin?",
    "Moveable Statue",
    "Weathervane",
    "Crystal Switch",
    "Sick Kid with Bug Catching Net",

    -- 0x20 
    "Sluggula",
    "Push Switch (like in Swamp Palace)",
    "DW Popo",
    "Red Bari / Small Red Bari",
    "Blue Bari",
    "Tree you can talk to?",
    "Hardhat Beetle (Charging Octopus looking thing)",
    "Dead Rock (Some might see them as Gorons, but bleh)",
    "Shrub Guy who talks about Triforce / Other storytellers",
    "Blind Hideout Guy / Thief Hideout Guy / Flute Boy's Father",
    "Sweeping Lady",
    "Bum under the bridge + smoke and other effects like the fire",
    "Lumberjack Bros.",
    "Telepathic stones? Looks like a prototype for a telepathic interface using sprites instead of tiles. However, this one only says one thing.",
    "Flute Boy and his musical notes",
    "Maze Game Lady",

    -- 0x30 
    "Maze Game Guy",
    "Fortune Teller / Dwarf swordsmith",
    "Quarreling brothers",
    "Pull For Rupees",
    "Young Snitch Lady",
    "Innkeeper",
    "Witch",
    "Waterfall",
    "Arrow Target (e.g. Statue with big eye in Dark Palace)",
    "Middle Aged Guy in the desert",
    "Magic Powder Bat /The Lightning Bolt the bat hurls at you.",
    "Dash Item / such as Book of Mudora, keys",
    "Kid in village near the trough",
    "Older Snitch Lady (Signs?) (Chicken lady also showed up)",
    "Rock Rupee Crabs",
    "Tutorial Soldiers from beginning of game",

    -- 0x40 
    "Hyrule Castle Barrier to Agahnim's Tower",
    "Soldier",
    "Blue Soldier",
    "Red Sp.Soldier",
    "Crazy Blue Killer Soldiers",
    "Red Sp.Soldier",
    "Blue Archer Soldiers",
    "Green Archer Soldiers (in the bushes)",
    "Red Javelin Trooper",
    "Red Javelin Soldiers (in the bushes)",
    "Red Bomb Soldier",
    "Green Recruit",
    "Sand Monster",
    "Bunny",
    "Snake basket",
    "Blob",

    -- 0x50 
    "Metal Balls",
    "Armos",
    "Zora King",
    "Armos Knight",
    "Lanmolas",
    "Fireball",
    "Walking Zora",
    "Desert Palace Barriers",
    "Crab",
    "Lost Woods Bird",
    "Lost Woods Squirrel",
    "Spark (clockwise on convex)",
    "Spark (counterclockwise on convex)",
    "Roller (vertical moving)",
    "Roller (vertical moving)",
    "Roller (???)",

    -- 0x60 
    "Roller (horizontal moving)",
    "Statue Sentry",
    "Master Sword plus pendants and beams of light",
    "Sand Lion Pit",
    "Sand Lion",
    "Shooting Gallery guy",
    "Moving cannon ball shooters",
    "Moving cannon ball shooters",
    "Moving cannon ball shooters",
    "Moving cannon ball shooters",
    "Ball N' Chain Trooper",
    "Cannon Ball Shooting Soldier (unused in original = WTF?)",
    "Warp Vortex created by Magic Mirror",
    "Rat / Bazu",
    "Rope / Skullrope (aka Sukarurope?)",
    "Bats / Also one eyed bats",

    -- 0x70 
    "Splitting Fireballs from Helmasaur King",
    "Leever",
    "Activator for the ponds (where you throw in items)",
    "Link's Uncle / Sage / Barrier that opens in the sanctuary",
    "Red Hat Boy who runs from you",
    "Bottle Vendor",
    "Princess Zelda",
    "Also Fire Faeries (seems like a different variety)",
    "Elder's Wife (Sahasrahlah's Wife, supposedly)",
    "Good bee / normal bee",
    "Agahnim",
    "Agahnim energy blasts (not the duds)",
    "Green Stalfos",
    "32*32 Pixel Yellow Spike Traps",
    "Swinging Fireball Chains",
    "Swinging Fireball Chains",

    -- 0x80 
    "Wandering Fireball Chains",
    "Waterhoppers",
    "Swirling Fire Faeries (Eastern Palace)",
    "Greene Eyegore",
    "Red Eyegore",
    "Yellow Stalfos (drops to the ground, dislodges head)",
    "Kodondo",
    "Flames",
    "Mothula",
    "Mothula's beam",
    "Spike",
    "Gibdo",
    "Arghuss",
    "Arghuss spawn",
    "Chair Turtles you kill with hammers",
    "Blobs / Crazy Blobs via Magic powder or Quake Medallion",

    -- 0x90 
    "Grabber things?",
    "Stalfos Knight",
    "Helmasaur King",
    "Bungie / Red Orb? (according to HM)",
    "Pirogusu (aka Swimmer) / Flying Tiles",
    "Eye laser",
    "Eye laser",
    "Eye laser",
    "Eye laser",
    "Pengator",
    "Kyameron",
    "Wizzrobe",
    "Black sperm looking things",
    "Black sperm looking things",
    "Ostrich seen with Flute Boy",
    "Rabbit seen with Flute Boy",

    -- 0xA0 
    "Birds seen with Flute Boy",
    "Freezor",
    "Kholdstare",
    "Another part of Kholdstare",
    "Ice balls from above",
    "Blue Zazak / Fire Phlegm (Fireballs of Red Zazaks and other sprites)",
    "Red Zazak",
    "Red Stalfos Skeleton",
    "Bomber Flying Creatures from Darkworld",
    "Bomber Flying Creatures from Darkworld",
    "Like Like (O_o yikes)",
    "Maiden (as in, the maidens in the crystals after you beat a boss)",
    "Apples",
    "Old Man on the Mountain",
    "Down Pipe",
    "Up Pipe",

    -- 0xB0 
    "Right Pipe",
    "Left Pipe",
    "Good bee again? (Perhaps the good bee is different after being released.... It would make sense, actually)",
    "Hylian Inscription (near Desert Palace). Also near Master Sword",
    "Thief's chest (not the one that follows you, the one that you grab from the DW smithy house)",
    "Bomb Salesman (elephant looking guy)",
    "Kiki the monkey?",
    "Maiden that ends up following you in Thieves Town",
    "Monologue Testing Sprite (Debug Artifact)",
    "Feuding Friends on Death Mountain",
    "Whirlpool",
    "Salesman / chestgame guy / 300 rupee giver guy / Chest game thief",
    "Drunk in the inn",
    "Vitreous (the large eyeball)",
    "Vitreous' smaller eyeballs",
    "Aghanim / Vitreous' lightning blast",

    -- 0xC0 
    "Monster in Lake of Ill Omen / Quake Medallion",
    "Agahnim teleporting Zelda to dark world",
    "Boulders / Rocks from Lanmolas erupting from the ground",
    "Gibo (vulnerable part)",
    "Thief",
    "Evil Fireball Spitters (THE FACES!!!)",
    "Four Way Fireball Spitters (spit when you use your sword)",
    "Hokbok (HM calls it Fuzzy Stack, I think?)",
    "Big Healing Faeries / Faerie Dust",
    "Firebat",
    "Chain Chomp",
    "Trinexx",
    "Another Part of Trinexx",
    "Another Part of Trinexx (again)",
    "Blind the Thief",
    "Swamola (swamp worms from Swamp of Evil)",

    -- 0xD0 
    "Lynel (centaur like creature)",
    "Rabbit Beam aka Transform aka Yellow Hunter",
    "Flopping Fish",
    "Stal",
    "Landmine",
    "DigGameProprietor",
    "Ganon",
    "Ganon Copy",
    "Heart Refill",
    "Green Rupee",
    "Blue Rupee",
    "Red Rupee",
    "Bomb Refill (1)",
    "Bomb Refill (4)",
    "Bomb Refill (8)",
    "Small Magic Refill",

    -- 0xE0 
    "Full Magic Refill",
    "Arrow Refill (5)",
    "Arrow Refill (10)",
    "Faerie",
    "Key",
    "Big Key",
    "Shield Pickup (Fighter or Red Shield after being dropped by a Pikit)",
    "Mushroom",
    "Fake Master Sword",
    "Magic Shop dude / His items, including the magic powder",
    "Full Heart Container",
    "Quarter Heart Container",
    "Bushes",
    "Cane of Somaria Platform",
    "Movable Mantle (in Hyrule Castle)",
    "Cane of Somaria Platform (same as 0xED but this index is not used)",

    -- 0xF0
    "Cane of Somaria Platform (same as 0xED but this index is not used)",
    "Cane of Somaria Platform (same as 0xED but this index is not used)",
    "Medallion Tablet",
}


local table_dir_value = {
  [0x00] = "up",
  [0x01] = "down",
  [0x02] = "left",
  [0x03] = "right",
}

local function dir_to_text(value)
  if value < 0x04 then
    return table_dir_value[value]
  else
    return value;
  end
end

state_to_verbose = {
    [0x00] = "dead",
    [0x01] = "falling pit",
    [0x02] = "puff of smoke",
    [0x03] = "falling water",
    [0x04] = "dying boss",
    [0x05] = "falling pit special",
    [0x06] = "dying monster",
    [0x08] = "spawning",
    [0x09] = "normal",
    [0x0A] = "carried",
    [0x0B] = "frozen",
}

local function to_hex(num)
    return string.format('%X', num)
end

local function gameDrawBox(x1, y1, x2, y2, color1, color2)
  local lx1 = x1 - camera_x & 0xFF
  local ly1 = y1 - camera_y & 0xFF
  local lx2 = x2 - camera_x & 0xFF
  local ly2 = y2 - camera_y & 0xFF
  gui.drawBox(lx1, ly1, lx2, ly2, color1, color2)
end

local function load_sprite(i)
    memory.usememorydomain("System Bus")
    -- memory.writebyte(0x7E0DE0 + 0, 4)
    local sprite = {
        index = i,
        state = memory.readbyte(0x7E0DD0 + i),
        hp = memory.readbyte(0x7E0E50 + i),
        type = memory.readbyte(0x7E0E20 + i),
        x = memory.readbyte(0x7E0D10 + i) | (memory.readbyte(0x7E0D30 + i) << 8),
        y = memory.readbyte(0x7E0D00 + i) | (memory.readbyte(0x7E0D20 + i) << 8),
		x_low = memory.readbyte(0x7E0D10 + i),
		x_high = memory.readbyte(0x7E0D30 + i),
		y_low = memory.readbyte(0x7E0D00 + i),
		y_high = memory.readbyte(0x7E0D20 + i),
        x_vel = memory.readbyte(0x7E0D50 + i),
        y_vel = memory.readbyte(0x7E0D40 + i),
        ai = memory.readbyte(0x7E0D80 + i),
        timer = memory.readbyte(0x7E0DF0 + i ),
        aux_timer1 = memory.readbyte(0x7E0E00 + i ),
        aux_timer2 = memory.readbyte(0x7E0E10 + i ),
        dir = memory.readbyte(0x7E0DE0 + i ),
		hitbox = memory.readbyte(0x7E0F60 + i)
    }

    sprite['active'] = sprite['state'] > 0
    sprite['name'] = type_to_name[sprite['type'] + 1]
    sprite['state_verbose'] = state_to_verbose[sprite['state']]

    return sprite
end

local function load_all_sprites()
    local sprites = {}
    for i=0, 15 do
        table.insert(sprites, load_sprite(i))
    end
    return sprites
end

local function show_sprite_info(pos, sprite)
  local x = pos % 4
   local y = math.floor(pos / 4) * (7 * 10)
    DrawNiceText(x * 64, y + 7 * 0, '#' .. to_hex(sprite['index']))
    DrawNiceText(x * 64, y + 7 * 1, sprite['name'])
    DrawNiceText(x * 64, y + 7 * 2, 'State: ' .. sprite['state_verbose'])
    DrawNiceText(x * 64, y + 7 * 3, 'HP: ' .. sprite['hp'])
    DrawNiceText(x * 64, y + 7 * 4, 'XYcor: ' .. to_hex(sprite['x']) .. "," .. to_hex(sprite['y']))
    DrawNiceText(x * 64, y + 7 * 5, 'XYvel: ' .. to_hex(sprite['x_vel']) .. "," .. to_hex(sprite['y_vel']))
	--console.writeline(sprite['dir'])
    DrawNiceText(x * 64, y + 7 * 6, 'Dir: ' .. to_hex(sprite['dir']).."/"..dir_to_text(sprite['dir']))
    DrawNiceText(x * 64, y + 7 * 7, 'AI: ' .. to_hex(sprite['ai']))
    DrawNiceText(x * 64, y + 7 * 8, 'Timers: ' .. to_hex(sprite['timer']) .. " " .. to_hex(sprite['aux_timer1']) .. " " .. to_hex(sprite['aux_timer2']))
end

local function draw_sprite(pos, sprite)
    show_sprite_info(pos, sprite)
    --console.writeline(string.format("Name : %s (S:0x%02X) - HitboxB:0x%02X - map position : %d,%d - screen position : %d,%d   --- low/hight %d, %d / %d, %d", sprite['name'], sprite['state'], sprite['hitbox'], 
	--sprite['x'], sprite['y'],  sprite['x'] - camera_x, sprite['y'] - camera_y, sprite['x_low'], sprite['x_high'], sprite['y_low'], sprite['y_high']))
	local s_color = 0x000000
	if sprite['state'] == 0x09 then
	  s_color = 0x0000FF
	end
	if sprite['state'] == 0x0A then
	  s_color = 0x555500
	end
	if sprite['state'] == 0x00 then
	  s_color = 0x8F8F8F
	end
	if sprite['state'] == 0x08 then
	  s_color = 0x00FF00
	end
	local hitbox_index = (sprite['hitbox'] & 0x1F) + 1
	--console.writeline(hitbox_index..table_size_hitbox1[hitbox_index])
	local size_w = table_size_hitbox1[hitbox_index]
	local size_h = table_size_hitbox2[hitbox_index]
	local x_low = sprite['x_low'] + table_x_offsets_low[hitbox_index]
	local x_high = sprite['x_high']-- + table_x_offsets_high[hitbox_index]
	local y_low = sprite['y_low'] + table_y_offsets_low[hitbox_index] - memory.readbyte(0x0F70 + sprite['index'])
	local y_high = sprite['y_high'] -- + table_y_offsets_high[hitbox_index]
	local modified_x = x_low | (x_high << 8)
	local modified_y = y_low | (y_high << 8)
	
	--console.writeline(string.format("-- Weird stuff %d, %d | %d, %d", x_low, x_high, y_low, y_high))
	--console.writeline(string.format("--Weird pos : %d, %d -- shadow %d", modified_x, modified_y, memory.readbyte(0x0F70 + sprite['index'])))
	
	gameDrawBox(modified_x, modified_y, modified_x + size_w, modified_y + size_h, 0xFFFFFFFF, 0xBBFFFFFF)
	gui.drawPixel(sprite['x'] - camera_x, sprite['y'] - camera_y, 0xFF00FF00)
	gui.drawBox(sprite['x'] - camera_x, sprite['y'] - camera_y, sprite['x'] - camera_x + size_w, sprite['y'] - camera_y + size_h, 0xFF000000 |  s_color, 0x55000000 | s_color)
	DrawNiceText(sprite['x'] - camera_x + 16, sprite['y'] - camera_y + 5, sprite['name'], 0xFF00FF00)
	DrawNiceText(sprite['x'] - camera_x + 16, sprite['y'] - camera_y, "#"..to_hex(sprite['index']), 0xFF00FF00)
end

local function iterate_sprites()
    local pos = 0
    local sprites = load_all_sprites()
    for i, sprite in pairs(sprites) do
        if sprite['active'] then
            draw_sprite(pos, sprite)
            pos = pos + 1
        end
    end
end


     -- object A and object B colision
     --!pos1_low  = $00  - XA1/YA1
     -- !pos1_size = $02 - WA/HA
     -- !pos2_low  = $04 - XB1/YB1
     --!pos2_size = $06 - WB/HB
     --!pos1_high = $08 - XA2/YA2
     --!pos2_high = $0A - YB2/YB2

	 

-- Bank6 #6065
function draw_dash_hitbox()
  local index = bit.rshift(mainmemory.read_s8(0x2F))
  local left_x = link_x + mainmemory.read_s16_le(0xF588 + index) 
  local right_x = link_x + mainmemory.read_s16_le(0xF58C + index)
  local top_y = link_y - 0x0A
  local bot_y = top_y + 0x2D
end


function draw_link_hitbox()  
    gameDrawBox(link_x, link_y, link_x + 16, link_y + 23, 0x8800FF00, 0x4400FF00) 
end


local function draw_action_hitbox()
    -- from compendium, on $0301:
    --     When non zero, Link has something in his hand, poised to strike. (Ice Rod, Hammer) Bit 6 being set indicates that magic powder is being sprinkled
    local _0301 = memory.read_u8(0x0301)
    -- x box, y box (starting corner of the box)
    local xb = 0
    local yb = 0

    local nobox = false
    local donebox = false
    -- $2F is link direction - 0:U, 2:D, 4:L, 6:R
    local direction  = 0
    local tmp = 0
    local Y  = 0
    local X  = 0
    local hh = 0
    local ww = 0

    -- Z3C: Flag indicating whether Link will bounce off if he touches a wall.
    --   perhaps a proxy for 'is dashing' ?
    local _0372 = memory.read_u8(0x0372)
    
    if _0372 ~= 0 then -- dashing (buggy)
        -- this does not draw the hitbox in the correct place :\
        -- i can't figure it out, sorry i'm giving up
	    -- _0372 == 1 appears to match dashing-with-sword-out state.
        direction = memory.read_u8(0x2F)
        index = direction >> 1
        -- Y = (direction-(direction%2))/2
        --xb = link_x + mainmemory.read_s16_le(0xF58E + index)
        --yb = link_y + mainmemory.read_s16_le(0xF596 + index) - 0X0A

        x_offset_l = memory.read_u8(0x06F58E + Y, "System Bus")
        x_offset_h = memory.read_u8(0x06F592 + Y, "System Bus")
        y_offset_l = memory.read_u8(0x06F596 + Y, "System Bus")
        y_offset_h = memory.read_u8(0x06F58C + Y, "System Bus")

        x_offset = (x_offset_h << 8) + x_offset_l
        y_offset = (y_offset_h << 8) + y_offset_l
        xb = link_x + x_offset
        yb = link_y + y_offset

        hh = 0x10
        ww = 0x10
    else
        -- most likely relevant piece of Z3C on 0x3C: 
        --   Lower Nibble: How many frames the B button has been held, approximately.
        --   Upper nibble set to 9 on spin attack release.
        -- so Y is, roughly, how long we've been holding sword out.
        --     NOTE: Y <= 9 apparently forever
        Y = memory.read_u8(0x3C)
        -- N.B. about 0x037a:
        --     ??? related to picking up bombs (see Bank 07)
        --         (bank 07 search does not reveal anything useful)

        -- What are we checking $0301 against here?
        --     0x02 - hammer, it looks like (Z3C: AND #$0002 ; check if hammer is used )
        --     next branch says:
        --         AND #$0027 ; check if sword is used 
        --     0027 == 0b100111
        --
        --     0x08 == 0b001000
        --     0x0A == 0b001010
        --     0x10 == 0b010000
        --  $0x037A is related to bombs or shovel (?)
        if _0301 == 0x02 or _0301 == 0x08 or _0301 == 0x0A or memory.read_u8(0x037A) == 0x10 then
	        -- if any of this (hammer, or... other stuff), we don't care how long we've been holding B
            Y = 0x00
        elseif Y > 127 then -- spin hitbox active
            -- 127 == 0x7F
            xb = link_x-0xE
            yb = link_y-0XA
            hh = 0x2C
            ww = 0x2C
            donebox = true
        elseif Y < 128 then
            -- not a special case (hammer or ice rod or whatever....?), not spinning
            -- i.e. slashing or not doing anything [or maybe other states too?]
            tmp = memory.read_u8(0x06F577 + Y, "System Bus")
            if tmp ~= 0 then
                -- this occurs when we're standing still,
                -- and while we're slashing but without a hitbox
                nobox = true
            else
                -- sword hitbox active (this branch is reached during active slash frames & during
                -- poke hitbox frames)
                direction = memory.read_u8(0x2F)

                -- This % should be useless; direction is <= 8
                -- can it get a weird/bad value somehow I'm not aware of?
                direction = (direction*8) % 256
                X = direction + Y + 1
            end
        end
        if not nobox and not donebox then -- slashing or poking
            -- N.B. does this catch anything other than slash/poke?
            --
            local sword_offset_x = memory.read_u8(0x45)
            sword_offset_x = sword_offset_x + memory.read_u8(0x06F473 + X, "System Bus")
            sword_offset_x = sword_offset_x % 256

            xb = link_x + sword_offset_x

            local sword_offset_y = memory.read_u8(0x44)
            sword_offset_y = sword_offset_y + memory.read_u8(0x06F4F5 + X, "System Bus")
            sword_offset_y = sword_offset_y % 256

            yb = link_y + sword_offset_y

            ww = memory.read_u8(0x06F4B4 + X, "System Bus")
            hh = memory.read_u8(0x06F536 + X, "System Bus")
        end
    end

    gameDrawBox(xb, yb, xb+ww, yb+hh, 0xCCFFCCCC, 0x77FFCCCC)
end



function draw_bombs()
	BOMB_ID = 0x07

	for ancilla_idx=0,9 do
		local id = mainmemory.read_u8(0x0C4A + ancilla_idx)
		if id == BOMB_ID then
			-- these don't seem to be that relevant?
			-- local timer = mainmemory.read_u8(0xEF0 + ancilla_idx)
			-- local ignore_all = mainmemory.read_u8(0x0BA0 + ancilla_idx)
			local something_counting_up = mainmemory.read_u8(0x0C5E + ancilla_idx)
			if something_counting_up > 0 and something_counting_up < 0x0B then
				-- empirically, this rolls from 1 to 0x0B during the bombs active hitbox frames. not sure exactly what it's counting.

				local x_low = mainmemory.read_u8(0x0C04 + ancilla_idx)
				local x_high = mainmemory.read_u8(0x0C18 + ancilla_idx)
				local y_low = mainmemory.read_u8(0x0BFA + ancilla_idx)
				local y_high = mainmemory.read_u8(0x0C0E + ancilla_idx)
				local altitude = mainmemory.read_u8(0x029E + ancilla_idx)
				-- this probably has overflow issues that i don't fully understand
				y_low = y_low - 0x18 - altitude
				x_low = x_low - 0x18
				local x = x_low | (x_high << 8)
				local y = y_low | (y_high << 8)
				gameDrawBox(x, y, x + 0x30, y + 0x30, 0x88FF66FF, 0x44FF66FF)
			end

		end

	end

end


memory.usememorydomain("System Bus")

oldrng = 0
lastrngchange = 0

function my_draw()
 -- $0618[0x02] -   Y coordinate of the scrolling camera. Probably the lower bound for scrolling.
 -- $061A[0x02] -   Y coordinate of the upper bounds of scrolling.
 -- $061C[0x02] -   X coordinate of the lower bounds of scrolling.
 -- $061E[0x02] -   X coordinate of the upper bounds of scrolling.
 
 link_x = mainmemory.read_s16_le(0x22)
 link_y = mainmemory.read_s16_le(0x20)
 --previous value for camera are bogus/weird
 camera_x = mainmemory.read_s16_le(0x11E)
 camera_y = mainmemory.read_s16_le(0x122)
 xspeed = memory.read_s8(0x31)
 yspeed = memory.read_s8(0x30)
 rng = memory.read_u8(0x7E0FA1)

 if rng ~= oldrng then
   oldrng = rng
   lastrngchange = emu.framecount()
 end
 --rng2137 = memory.read_u8(0x2137)
 rng213c = memory.read_u8(0x213c)
 rng1a = memory.read_u8(0x1a)
 
 -- draw a pixel at links coords:
 --   gui.drawPixel(link_x - camera_x, link_y - camera_y, 0xFF00FF00)
 --
 -- DrawNiceText(180, 150, "X : "..link_x)
 -- DrawNiceText(180, 160, "Y : "..link_y)
 -- DrawNiceText(180, 180, "XSpeed : "..xspeed)
 -- DrawNiceText(180, 190, "YSpeed : "..yspeed)
 -- DrawNiceText(80, 190, string.format("OLDRNG  %d  : %d", lastrngchange, oldrng))
 -- DrawNiceText(80, 195, string.format(" RNG : %d,  -  213c: %x,  1a : %x", rng, rng213c, rng1a))
 -- DrawNiceText(100, 200, string.format("1: %x - 11: %x - 111: %x", rng & 1, rng & 3, rng & 7))

 iterate_sprites()
 draw_link_hitbox()
 draw_action_hitbox()
 draw_bombs()
end

if is_snes9x then
  event.onframeend(my_draw)
else
  while true do
    my_draw()
    emu.frameadvance()
  end
end
