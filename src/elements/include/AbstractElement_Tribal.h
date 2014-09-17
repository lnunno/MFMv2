/*                                              -*- mode:C++ -*-
  AbstractElement_Tribal.h Tutorial template for a new Element
  Copyright (C) 2014 The Regents of the University of New Mexico.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
  USA
*/

/* <<TEMPLATE>> Replace doxygen and license information with your element information. */
/**
  \file   AbstractElement_Tribal.h Tribal base class.
  \author Lucas L. Nunno
  \date (C) 2014 All rights reserved.
  \lgpl
 */
#ifndef ELEMENT_TRIBAL_H /* <<TEMPLATE>> Replace header guard with your element name.*/
#define ELEMENT_TRIBAL_H

#include "Element.h"
#include "EventWindow.h"
#include "ElementTable.h"
#include "itype.h"
#include "BitField.h"

namespace MFM
{

  template <class CC>
  class AbstractElement_Tribal : public Element<CC>
  {
    // Extract short names for parameter types
    typedef typename CC::ATOM_TYPE T;
    typedef typename CC::PARAM_CONFIG P;
    enum { R = P::EVENT_WINDOW_RADIUS };

    // BEGIN ATOMIC PARAMETERS
    // From the atomic parameters tutorial:
    enum
    {
        BITS = P::BITS_PER_ATOM,

        BYTE_BITS_LENGTH = 8,
        BYTE_BITS_POSITION = BITS - BYTE_BITS_LENGTH - 1
    };

    typedef BitField<BitVector<BITS>, BYTE_BITS_LENGTH, BYTE_BITS_POSITION> AFByteBits;
    // END ATOMIC PARAMETERS


    /*
     * Tribe enum. This defines which tribes we can actually have.
     */
    enum Tribe
    {
    	RED=1,
    	BLUE,
    	GREEN,
    	TRIBE_COUNT
    };

  private:

    ElementParameterS32<CC> m_tribe;

    /* <<TEMPLATE>> Add any configurable parameters here. */

  public:

    u32 GetTribeBits(const T& us) const
	{
		return AFByteBits::Read(this->GetBits(us));
	}

	void SetTribeBits(const T& us, u32 bits) const
	{
		AFByteBits::Write(this->GetBits(us), bits & 0xff);
	}

    /*
     * This supplies the tribe's color.
     */
    virtual u32 LocalPhysicsColor(const T& atom, u32 selector)
    {
    	u32 tribeNumber = GetTribeBits(atom);
    	switch(tribeNumber)
    	{
    	case RED:
    		return 0xFFFF0000;
    		break;
    	case BLUE:
    		return 0xFF00FF00;
    		break;
    	case GREEN:
    		return 0xFF0000FF;
    		break;
    	default:
    		// TODO: Throw an exception.
    		break;
    	}
    	return 0xFFFF0000;
    }

    u32 GetSelectedTribe()
    {
    	return (u32) m_tribe.GetValue();
    }

    /**
     * Test the other tribal element to see if it is in the same tribe as this element.
     *
     * TODO: Implement
     */
    bool IsInSameTribe(const AbstractElement_Tribal& otherGuy)
    {
    	return false;
    }

    AbstractElement_Tribal(const UUID & uuid) : Element<CC>(uuid),
    		m_tribe(this,"tribe","Tribe","This is the tribe that this element has.",RED,RED,TRIBE_COUNT-1,1)
    {
    }

  };

}

#endif /* ELEMENT_TRIBAL_H */
