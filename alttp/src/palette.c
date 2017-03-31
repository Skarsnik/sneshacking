/*
Copyright 2016 Sylvain "Skarsnik" Colinet

 This file is part of the sneshackingtools project.

    sneshackingtools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with sneshackingtools.  If not, see <http://www.gnu.org/licenses/>
    */

#include <stdlib.h>
#include "palette.h"

// Inspired/copied from https://wiki.superfamicom.org/snes/show/Palettes

palette* palette_create(const unsigned int size, const unsigned int id)
{
  palette *new_pal = (palette*) malloc(sizeof(palette));
  new_pal->colors = (m_color*) malloc(sizeof(m_color) * size);
  new_pal->id = id;
  new_pal->size = size;
  return new_pal;
}


palette*	extract_palette(const char* data, const unsigned int offset, const unsigned int palette_size)
{
  palette* toret = palette_create(palette_size, 0);
  unsigned colnum = 0;
  for (int i = 0; i < palette_size * 2; i += 2)
  {
    unsigned short snes_color;
    snes_color = data[offset + i + 1] << 8;
    snes_color = snes_color | data[offset + i];
    toret->colors[colnum] = convertcolor_snes_to_rgb(snes_color);
    colnum++;
  }
  return toret;
}

m_color convertcolor_snes_to_rgb(const unsigned short snes_color)
{
  m_color toret;
  
  toret.red = ((snes_color       ) % 32) * 8;
  toret.green = ((snes_color /   32) % 32) * 8;
  toret.blue = ((snes_color / 1024) % 32) * 8;

  toret.red = toret.red + toret.red / 32;
  toret.green = toret.green + toret.green / 32;
  toret.blue = toret.blue + toret.blue / 32;
  return toret;
}

unsigned short 	convertcolor_rgp_to_snes(const m_color color)
{
  return convertcolor_rgp_to_snes2(color.red, color.green, color.blue);
}

unsigned short 	convertcolor_rgp_to_snes2(const uchar red, const uchar green, const uchar blue)
{
  uchar R = red / 8;
  uchar G = green / 8;
  uchar B = blue / 8;

  return B * 1024 + G * 32 + R;
}