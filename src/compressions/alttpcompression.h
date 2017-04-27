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

#ifndef H_DECOMPRESS_H
#define H_DECOMPRESS_H

#ifdef __cplusplus
extern "C" {
#endif


char*	decompress(const char *c_data, const unsigned int start, unsigned int* uncompressed_data_size, unsigned int* compressed_lenght);
char*	compress(const char* u_data, const size_t start, const unsigned int lenght, unsigned int* compressed_size);


#ifdef __cplusplus
}
#endif


#endif