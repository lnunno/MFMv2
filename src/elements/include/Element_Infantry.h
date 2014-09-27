/*                                              -*- mode:C++ -*-
 Element_Base.h Tutorial template for a new Element
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
 \file   Element_Infantry.h Infantry element for creating units.
 \author Lucas L. Nunno
 \date (C) 2014 All rights reserved.
 \lgpl
 */
#ifndef Element_Infantry_H /* <<TEMPLATE>> Replace header guard with your element name.*/
#define Element_Infantry_H

#include "Element.h"
#include "EventWindow.h"
#include "ElementTable.h"
#include "Element_Res.h"
#include "itype.h"
#include "AbstractElement_Tribal.h"

#include <iostream>

using namespace std;

namespace MFM
{

#define INFANTRY_VERSION 1

  template<class CC>
  class Element_Infantry: public AbstractElement_Tribal<CC> /* <<TEMPLATE>> Replace class name with yours */
  {
    public:
      // Extract short names for parameter types
      typedef typename CC::ATOM_TYPE T;
      typedef typename CC::PARAM_CONFIG P;

      enum
      {
        R = P::EVENT_WINDOW_RADIUS,
        BITS = P::BITS_PER_ATOM,

        //////
        // Element state fields.

        TIME_ALIVE_LENGTH = 4,
        TIME_ALIVE_POSITION = AbstractElement_Tribal<CC>::TRIBAL_FIRST_FREE_POSITION
            - TIME_ALIVE_LENGTH
      };

      typedef BitField<BitVector<BITS>, TIME_ALIVE_LENGTH, TIME_ALIVE_POSITION> TimeAliveField;

      u32 GetTimeAlive(const T& us) const
      {
        return TimeAliveField::Read(this->GetBits(us));
      }

      void SetTimeAlive(T& us, const u32 timeAlive) const
      {
        cout << "Set time alive to " << timeAlive << endl;
        TimeAliveField::Write(this->GetBits(us), timeAlive & 0xf);
        cout << "Set? " << this->GetTimeAlive(us) << endl;
      }

    private:

      // Some utility functions.
      u32 GetFirstEmptySpace(EventWindow<CC>& window) const
      {
        const MDist<R> md = MDist<R>::get();
        for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(1); ++idx)
        {
          cout << "Index: " << idx << endl;
          const SPoint rel = md.GetPoint(idx);
          if (!window.IsLiveSite(rel))
          {
            continue;
          }
          T other = window.GetRelativeAtom(rel);
          u32 neighborType = other.GetType();
          if (neighborType == Element_Empty<CC>::THE_INSTANCE.GetType())
          {
            // Found an empty space, return it.
            return idx;
          }
        }
        // None found, return NULL.
        return -1;
      }

    public:

      /* <<TEMPLATE>> Replace class name with yours. Don't forget the '<CC>'. */
      static Element_Infantry<CC> THE_INSTANCE;

      Element_Infantry() :
              AbstractElement_Tribal<CC>(
                  MFM_UUID_FOR("Infantry", INFANTRY_VERSION))

      {
        /* <<TEMPLATE>> Set atomic symbol and name for your element. */
        Element<CC>::SetAtomicSymbol("In");
        Element<CC>::SetName("Infantry");
        AbstractElement_Tribal<CC>::SetElementGradient(0x00001100);
      }

      /*
       <<TEMPLATE>> Set how likely your element is to be moved by another element. See
       Element.h for details.
       */
      virtual u32 PercentMovable(const T& you, const T& me,
          const SPoint& offset) const
      {
        return 0;
      }

      /* <<TEMPLATE>> This color will be the default rendering color for your element. */
      virtual u32 DefaultPhysicsColor() const
      {
        return 0xffff0000;
      }

      /*
       <<TEMPLATE>> This color will be the color rendered when your
       element is rendered in lowlighting.
       */
      virtual u32 DefaultLowlightColor() const
      {
        return 0xffff0000;
      }

      /*
       <<TEMPLATE>> This is a short description of your element.
       */
      virtual const char* GetDescription() const
      {
        return "This is an infantry unit.";
      }

      static const u32 TYPE()
      {
        return THE_INSTANCE.GetType();
      }

      virtual const T& GetDefaultAtom() const
      {
        static T defaultAtom(TYPE(), 0, 0, 0);
        u32 tribeValue = this->m_tribe.GetValue();
        this->SetTribe(defaultAtom, tribeValue);
        return defaultAtom;
      }

      /*
       This method is executed every time an atom of your
       element is chosen for an event. See the tutorial in
       the wiki for further information.
       */
      virtual void Behavior(EventWindow<CC>& window) const
      {
        T self = window.GetCenterAtom();
        u32 timeAlive = this->GetTimeAlive(self);
        u32 newVal = timeAlive + 1;
        cout << "Time alive: " << timeAlive << endl;
        this->SetTimeAlive(self,newVal);
        this->Diffuse(window);
      }

  }
  ;

  /*
   <<TEMPLATE>> Rename the class names here to the class name of your element.
   */
  template<class CC>
  Element_Infantry<CC> Element_Infantry<CC>::THE_INSTANCE;

}

#endif /* Element_Infantry_H */
