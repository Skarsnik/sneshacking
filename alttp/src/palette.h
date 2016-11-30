#ifndef H_PALETTE_H
#define H_PALETTE_H

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} m_color;

typedef struct {
  unsigned int	id;
  unsigned int	size;
  m_color*	colors;
} palette;


palette *palette_create(const unsigned int size, const unsigned int id);

#endif