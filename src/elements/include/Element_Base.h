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
 \file   Element_Base.h Base element for creating units.
 \author Lucas L. Nunno
 \date (C) 2014 All rights reserved.
 \lgpl
 */
#ifndef Element_Base_H /* <<TEMPLATE>> Replace header guard with your element name.*/
#define Element_Base_H

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

#define BASE_VERSION 1

  template<class CC>
  class Element_Base: public AbstractElement_Tribal<CC> /* <<TEMPLATE>> Replace class name with yours */
  {
      // Extract short names for parameter types
      typedef typename CC::ATOM_TYPE T;
      typedef typename CC::PARAM_CONFIG P;

      enum
      {
        R = P::EVENT_WINDOW_RADIUS,
        BITS = P::BITS_PER_ATOM,

        //////
        // Element state fields.

        GOLD_LENGTH = 8,
        GOLD_POSITION = AbstractElement_Tribal<CC>::TRIBAL_FIRST_FREE_POSITION
            - GOLD_LENGTH
      };

      typedef BitField<BitVector<BITS>, GOLD_LENGTH, GOLD_POSITION> GoldBitField;

    private:

      ElementParameterS32<CC> m_goldPerRes;
      ElementParameterS32<CC> m_baseGoldCost;
      ElementParameterS32<CC> m_baseCreateOdds;

    protected:
      // We don't want the gold field to be accessible to the world.
      u32 GetGoldCount(const T& us) const
      {
        return GoldBitField::Read(this->GetBits(us));
      }

      void SetGoldCount(T& us, const u32 goldCount) const
      {
        cout << "Setting gold count to " << goldCount << endl;
        GoldBitField::Write(this->GetBits(us), goldCount & 0xff);
        cout << "Current? " << GetGoldCount(us) << endl;
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
      static Element_Base<CC> THE_INSTANCE;

      Element_Base() :
          AbstractElement_Tribal<CC>(MFM_UUID_FOR("Base", BASE_VERSION)),
          /* <<TEMPLATE>> Initialize all configurable parameters here. */
          m_goldPerRes(this, "goldPRes", "Gold Per Res",
              "The amount of gold produced for each Res collected.", 1, 10, 100,
              1), m_baseGoldCost(this, "baseGoldCost", "Base Gold Cost",
              "The cost (in gold) of producing a base.", 1, 5, 100, 1), m_baseCreateOdds(
              this, "baseCreate", "Base Create Chance",
              "The chance that a base will be attempted to be created.", 1, 5,
              100, 1)
      {
        /* <<TEMPLATE>> Set atomic symbol and name for your element. */
        Element<CC>::SetAtomicSymbol("Bs");
        Element<CC>::SetName("Base");
        AbstractElement_Tribal<CC>::SetElementGradient(0x00110000);
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
        return 0xff0000ff;
      }

      /*
       <<TEMPLATE>> This color will be the color rendered when your
       element is rendered in lowlighting.
       */
      virtual u32 DefaultLowlightColor() const
      {
        return 0xff0000ff;
      }

      /*
       <<TEMPLATE>> This is a short description of your element.
       */
      virtual const char* GetDescription() const
      {
        return "This is a base that creates other units and collects Res.";
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

       This method is inspired by the Element_Shark Behavior function.

       The loop:

       for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(1); ++idx)

       Looks in the immediate vicinity of this atom (all 8 locations).

       The behavior is as follows:

       - First look around the current window with a radius one for any available Res.
       - Consume any Res found within the search range and convert them into the appropriate gold amount.
       - If there are no available empty spaces within the search range, return since we do
       not have space to create anything new.
       - For each possible element, if we have enough gold to create an atom of that type,
       try to create an atom of each type based on the odds for creating that individual type.
       */
      virtual void Behavior(EventWindow<CC>& window) const
      {
        T self = window.GetCenterAtom();
        u32 ourTribe = this->GetTribe(self);
        Random & random = window.GetRandom();
        u32 goldCount = GetGoldCount(self);
        cout << "Current gold count = " << goldCount << endl;
        T empty = Element_Empty<CC>::THE_INSTANCE.GetDefaultAtom();
        const MDist<R> md = MDist<R>::get();
        u32 goldCollected = 0;

        // Consume Res loop.
        for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(1); ++idx)
        {
          const SPoint rel = md.GetPoint(idx);
          if (!window.IsLiveSite(rel))
          {
            continue;
          }
          T other = window.GetRelativeAtom(rel);
          u32 neighborType = other.GetType();
          if (neighborType == Element_Res<CC>::THE_INSTANCE.GetType())
          {
            // Consume the res. Set that point as empty.
            window.SetRelativeAtom(rel, empty);
            goldCollected += m_goldPerRes.GetValue();
          }
        }
        goldCount += goldCollected;
        SetGoldCount(self, goldCount);

        // Creation loop
        u32 emptyIndex = this->GetFirstEmptySpace(window);
        if (emptyIndex < 0)
        {
          // No space, don't do anything.
        }
        else
        {
          // First, checks to see if we have enough gold and then rolls the dice.

          // Base creation.
          if ((goldCount >= m_baseGoldCost.GetValue())
              && random.OneIn(m_baseCreateOdds.GetValue()))
          {
            // Checks passed, create a base.
            T base = Element_Base<CC>::THE_INSTANCE.GetDefaultAtom();
            this->SetTribe(base, ourTribe); // Change the tribe to our own.
            const SPoint& emptySpot = md.GetPoint(emptyIndex);
            window.SetRelativeAtom(emptySpot, base);
          }
        }

        // These guys are happy to move around randomly.
        this->Diffuse(window);
      }

  }
  ;

  /*
   <<TEMPLATE>> Rename the class names here to the class name of your element.
   */
  template<class CC>
  Element_Base<CC> Element_Base<CC>::THE_INSTANCE;

}

#endif /* Element_Base_H */ /* <<TEMPLATE>> Rename the header guard to your element */
