/*
	Audio File Library
	Copyright (C) 2001, Silicon Graphics, Inc.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

/*
	msadpcm.h

	This module declares the interface for the Microsoft ADPCM
	compression module.
*/

#ifndef MSADPCM_H
#define MSADPCM_H

#include <audiofile.h>

#include "afinternal.h"

bool _af_ms_adpcm_format_ok (_AudioFormat *f);

_AFmoduleinst _af_ms_adpcm_init_decompress (_Track *track, AFvirtualfile *fh,
	bool seekok, bool headerless, AFframecount *chunkframes);

#endif /* MSADPCM_H */
