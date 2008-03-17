/*
 * PDFedit - free program for PDF document manipulation.
 * Copyright (C) 2006, 2007, 2008  PDFedit team: Michal Hocko,
 *                                              Miroslav Jahoda,
 *                                              Jozef Misutka,
 *                                              Martin Petricek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in doc/LICENSE.GPL); if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA  02111-1307  USA
 *
 * Project is hosted on http://sourceforge.net/projects/pdfedit
 */
// vim:tabstop=4:shiftwidth=4:noexpandtab:textwidth=80

#ifndef _CPAGEMODULE_H
#define _CPAGEMODULE_H

#include "static.h"

//=====================================================================================
namespace pdfobjects {
//=====================================================================================

/**
 * Base class for all page modules.
 */
class ICPageModule : public noncopyable
{
	// 
	// Interface methods
	//
public:
	void init () {};
	void reset () {};
	// virutal dtor
	virtual ~ICPageModule () {};

}; // class ICPageModule


//=====================================================================================
} // namespace pdfobjects
//=====================================================================================


#endif // _CPAGEMODULE_H