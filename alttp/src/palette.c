#include <stdlib.h>
#include "palette.h"

palette* palette_create(const unsigned int size, const unsigned int id)
{
  palette *new_pal = (palette*) malloc(sizeof(palette));
  new_pal->colors = (m_color*) malloc(sizeof(m_color) * size);
  new_pal->id = id;
  new_pal->size = size;
  return new_pal;
}
