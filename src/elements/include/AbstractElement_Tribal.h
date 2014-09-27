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
#include "Fail.h"
#include <iostream>

using namespace std;

namespace MFM
{

  template<class CC>
  class AbstractElement_Tribal: public Element<CC>
  {
      // Extract short names for parameter types
      typedef typename CC::ATOM_TYPE T;
      typedef typename CC::PARAM_CONFIG P;

    public:
      // BEGIN ATOMIC PARAMETERS
      // From the atomic parameters tutorial:
      enum
      {
        R = P::EVENT_WINDOW_RADIUS,
        BITS = P::BITS_PER_ATOM,

        TRIBE_LENGTH = 4,
        TRIBE_POSITION = BITS - TRIBE_LENGTH - 1,

        TRIBAL_FIRST_FREE_POSITION = BITS - TRIBE_LENGTH
      };

      typedef BitField<BitVector<BITS>, TRIBE_LENGTH, TRIBE_POSITION> TribeBitField;
      // END ATOMIC PARAMETERS

      /*
       * Tribe enum. This defines which tribes we can actually have.
       */
      enum Tribe
      {
        RED = 1,
        BLUE,
        GREEN,
        YELLOW,
        TRIBE_COUNT
      };

    protected:

      ElementParameterS32<CC> m_tribe;

      /**
       * This is the shading of an element from its local tribe color
       * in order to differentiate elements that are on the same tribe.
       */
      s32 m_elementGradient;

      /**
       * Get the amount that each element should be shaded by.
       *
       * It works like so:
       *
       * FINAL_COLOR = TRIBE_COLOR + ELEMENT_GRADIENT
       */
      s32 GetElementGradient() const
      {
        return m_elementGradient;
      }

      /**
       * Set the element gradient.
       */
      void SetElementGradient(s32 gradientValue)
      {
        m_elementGradient = gradientValue;
      }

    public:

      u32 GetTribe(const T& us) const
      {
        return TribeBitField::Read(this->GetBits(us));
      }

      void SetTribe(T& us, const u32 tribe) const
      {
        TribeBitField::Write(this->GetBits(us), tribe & 0xf);
      }

      /*
       * This supplies the tribe's color.
       */
      virtual u32 LocalPhysicsColor(const T& atom, u32 selector) const
      {
        u32 tribeNumber = GetTribe(atom);
        switch (tribeNumber)
        {
          case RED:
            return 0xFF880000 + GetElementGradient();
          case BLUE:
            return 0xFF000088 + GetElementGradient();
          case GREEN:
            return 0xFF008800 + GetElementGradient();
          case YELLOW:
            return 0xFF888800 + GetElementGradient();
          default:
            // Shouldn't get here, fail if we do.
            FAIL(ILLEGAL_STATE);
            break;
        }
        // Shouldn't get here, fail if we do.
        FAIL(ILLEGAL_STATE);
        return 0x0;
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
      bool IsInSameTribe(const AbstractElement_Tribal& us,
          const AbstractElement_Tribal& otherGuy)
      {
        return false;
      }

      AbstractElement_Tribal(const UUID & uuid) :
          Element<CC>(uuid), m_tribe(this, "tribe", "Tribe",
              "This is the tribe that this element has.", RED, RED,
              TRIBE_COUNT - 1, 1), m_elementGradient(0)
      {
      }

  };

}

#endif /* ELEMENT_TRIBAL_H */
