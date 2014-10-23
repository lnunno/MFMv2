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
#include "Element_Wall.h"
#include <iostream>
#include <set>

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

        TRIBAL_FIRST_FREE_POSITION = BITS - TRIBE_LENGTH - 1
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

      void SetTribe(T& us, u32 tribe) const
      {
        TribeBitField::Write(this->GetBits(us), tribe & 0xf);
      }

      /**
       * Get a count of elements in the event window that share the same
       * tribal type as the passed in atom.
       *
       * range: The width of the event window to look for atoms.
       * self:  An atom of the appropriate tribe.
       * elementType: The element type to count.
       */
      u32 GetTribalElementCount(EventWindow<CC>& window, u32 range, T self,
          u32 elementType) const
      {
        const MDist<R> md = MDist<R>::get();
        u32 ourTribe = this->GetTribe(self);
        u32 elementCount = 0;

        // Loop that examines the entire event window.
        for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(range);
            ++idx)
        {
          const SPoint rel = md.GetPoint(idx);
          if (!window.IsLiveSite(rel))
          {
            continue;
          }
          T other = window.GetRelativeAtom(rel);
          u32 neighborType = other.GetType();
          const Element<CC> * element = window.GetTile().GetElement(
              neighborType);
          if (dynamic_cast<const AbstractElement_Tribal<CC>*>(element)
              && this->GetTribe(other) == ourTribe
              && neighborType == elementType)
          {
            elementCount++;
          }
        }
        return elementCount;
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
       * Check if an atom is tribal.
       */
      bool IsTribal(const T& atom) const
      {
        // TODO: This is a bad hack and it probably doesn't work right..
        // but we are desperate.
        return this->GetTribe(atom) > 0;
      }

      /**
       * Test the point in the event window to see if it contains a tribal element.
       */
      bool IsTribal(EventWindow<CC>& window, SPoint& rel) const
      {
        T other = window.GetRelativeAtom(rel);
        const u32 neighborType = other.GetType();
        const Element<CC> * elt = window.GetTile().GetElement(neighborType);
        if (dynamic_cast<const AbstractElement_Tribal<CC>*>(elt))
        {
          return true;
        }
        else
        {
          return false;
        }
      }

      bool IsInSameTribe(const T& you, const T& me) const
      {
        if (!this->IsTribal(you) || !this->IsTribal(me))
        {
          return false;
        }
        else
        {
          return this->GetTribe(you) == this->GetTribe(me);
        }
      }

      /**
       * Test the other tribal atom to see if it is in the same tribe as this atom.
       *
       */
      bool IsInSameTribe(T& self, EventWindow<CC>& window, SPoint& rel) const
      {
        if (!this->IsTribal(window, rel))
        {
          return false;
        }
        u32 ourTribe = this->GetTribe(self);
        T other = window.GetRelativeAtom(rel);
        return ourTribe == this->GetTribe(other);
      }

      /**
       * Used for inspecting whether or not this tribal atom is free to move an adjacent atom.
       */
      bool ShouldSwap(const T& you, const T& me) const
      {
        // Elements that should not be swapped by tribal atoms.
        u32 sa[] =
        { Element_Wall<CC>::TYPE() };
        set<u32> nonSwappables(sa, sa + 1);
        u32 yourType = you.GetType();
        if (this->IsTribal(you) && this->IsTribal(me))
        {
          // We're cool with it if we're in the same tribe, otherwise we are vehemently against it.
          return this->IsInSameTribe(you, me) ? true : false;
        }
        else
        {
          return nonSwappables.count(yourType) == 0;
        }
      }

      /*
       <<TEMPLATE>> Set how likely your element is to be moved by another element. See
       Element.h for details.
       */
      virtual u32 PercentMovable(const T& you, const T& me,
          const SPoint& offset) const
      {

        if (this->IsTribal(you) && this->IsTribal(me))
        {
          // We're cool with it if we're in the same tribe, otherwise we are vehemently against it.
          u32 percentMovable = this->IsInSameTribe(you, me) ? 100 : 0;
          return percentMovable;
        }
        // By default we don't care if people move us.
        return 100;
      }

      AbstractElement_Tribal(const UUID & uuid) :
              Element<CC>(uuid),
              m_tribe(this, "tribe", "Tribe",
                  "This is the tribe that this element has.", RED, RED,
                  TRIBE_COUNT - 1, 1),
              m_elementGradient(0)
      {
      }

  };

}

#endif /* ELEMENT_TRIBAL_H */
