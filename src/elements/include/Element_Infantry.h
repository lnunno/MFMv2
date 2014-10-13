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
#include "Dirs.h"

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
        MOVE_DIR_LENGTH = (Dirs::DIR_COUNT >> 1) - 1,
        MOVE_DIR_POSITION = AbstractElement_Tribal<CC>::TRIBAL_FIRST_FREE_POSITION
            - MOVE_DIR_LENGTH

      };

      typedef BitField<BitVector<BITS>, MOVE_DIR_LENGTH, MOVE_DIR_POSITION> MoveDirectionField;

      Dir GetMovementDirection(const T& us) const
      {
        return (Dir) MoveDirectionField::Read(this->GetBits(us));
      }

      void SetMovementDirection(T& us, Dir moveDirection) const
      {
        MoveDirectionField::Write(this->GetBits(us), moveDirection);
      }

    private:
      ElementParameterS32<CC> m_moveDirectionChangeOdds;
      ElementParameterS32<CC> m_killOdds;
    public:

      /* <<TEMPLATE>> Replace class name with yours. Don't forget the '<CC>'. */
      static Element_Infantry<CC> THE_INSTANCE;

      Element_Infantry() :
              AbstractElement_Tribal<CC>(
                  MFM_UUID_FOR("Infantry", INFANTRY_VERSION)),
              m_moveDirectionChangeOdds(this, "movChng", "Dir Change Odds",
                  "The probability that this unit will change its movement direction.",
                  1, 10, 100, 1),
              m_killOdds(this, "killOdds", "Kill Odds",
                  "The probability that this unit will kill an enemy unit.", 1,
                  5, 1000, 5)
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
        this->SetMovementDirection(defaultAtom, Dirs::EAST);
        return defaultAtom;
      }

      /*
       This method is executed every time an atom of your
       element is chosen for an event. See the tutorial in
       the wiki for further information.
       */
      virtual void Behavior(EventWindow<CC>& window) const
      {
        // Where this atom is going to move. Initialized to the center (its current position).
        SPoint centerPt = SPoint(0, 0);
        SPoint movePt = SPoint(0, 0);
        T self = window.GetCenterAtom();
        u32 ourTribe = this->GetTribe(self);
        T empty = Element_Empty<CC>::THE_INSTANCE.GetDefaultAtom();
        const MDist<R> md = MDist<R>::get();
        Random & rand = window.GetRandom();

        for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(1); ++idx)
        {
          const SPoint rel = md.GetPoint(idx);
          if (!window.IsLiveSite(rel))
          {
            continue;
          }
          T other = window.GetRelativeAtom(rel);
          const u32 neighborType = other.GetType();
          const Element<CC> * elt = window.GetTile().GetElement(neighborType);
          if (dynamic_cast<const AbstractElement_Tribal<CC>*>(elt)
              && this->GetTribe(other) != ourTribe)
          {
            // We see an enemy unit! Enter kill mode!
            if (rand.OneIn(m_killOdds.GetValue()))
            {
              // Kill roll is successful. Kill that mofo!
              window.SetRelativeAtom(rel, empty);
            }
          }
        }

        if (rand.OneIn(m_moveDirectionChangeOdds.GetValue()))
        {
          // Change direction.
          Dir newDirection = (Dir) rand.Create(Dirs::DIR_COUNT);
          this->SetMovementDirection(self, newDirection);
        }

        Dir movementDirection = this->GetMovementDirection(self);

        Dirs::FillDir(movePt, movementDirection);

        if (window.GetRelativeAtom(movePt).GetType()
            == Element_Empty<CC>::THE_INSTANCE.GetType()
            && window.IsLiveSite(movePt))
        {
          cout << "Moving to empty" << endl;
          // Move to this location.
          window.SwapAtoms(movePt, centerPt);
        }
        else
        {
          movePt = centerPt;
        }
        window.SetRelativeAtom(movePt, self); // Update self.
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
