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
#include "Element_Infantry.h"
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

    public:
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

      // Bit field operations.
      u32 GetGoldCount(const T& us) const
      {
        return GoldBitField::Read(this->GetBits(us));
      }

      void SetGoldCount(T& us, u32 newGoldCount) const
      {
        GoldBitField::Write(this->GetBits(us), newGoldCount);
      }

    private:

      ElementParameterS32<CC> m_goldPerRes;

      ElementParameterS32<CC> m_baseGoldCost;
      ElementParameterS32<CC> m_baseCreateOdds;

      ElementParameterS32<CC> m_infantryGoldCost;
      ElementParameterS32<CC> m_infantryCreateOdds;

      /**
       * This is the factor in which the Base is happy to stay where it
       * is if other Bases of the same tribe surround it.
       */
      static const u32 CONSTRUCT_STABILITY = 10;

      /**
       * This is the radius which the atom looks in its event window
       * for CONSTRUCT_STABILITY.
       */
      static const u32 STABILITY_RANGE = 2; // This must be <= R.

    private:

      // Some utility functions.
      s32 GetFirstEmptySpace(EventWindow<CC>& window) const
      {
        const MDist<R> md = MDist<R>::get();
        for (u32 idx = md.GetFirstIndex(1); idx <= md.GetLastIndex(1); ++idx)
        {
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
                  "The amount of gold produced for each Res collected.", 1, 1,
                  100, 1),
              m_baseGoldCost(this, "baseGoldCost", "Base Gold Cost",
                  "The cost (in gold) of producing a base.", 1, 5, 100, 1),
              m_baseCreateOdds(this, "baseCreate", "Base Create Chance",
                  "The chance that a base will be attempted to be created.", 1,
                  5, 100, 1),
              m_infantryGoldCost(this, "infGoldCost", "Infantry Gold Cost",
                  "The cost (in gold) of producing an infantry unit.", 1, 1,
                  100, 1),
              m_infantryCreateOdds(this, "infCreate", "Infantry Create Chance",
                  "The chance that an infantry unit will be attempted to be created.",
                  1, 3, 100, 1)

      {
        /* <<TEMPLATE>> Set atomic symbol and name for your element. */
        Element<CC>::SetAtomicSymbol("Bs");
        Element<CC>::SetName("Base");
        AbstractElement_Tribal<CC>::SetElementGradient(0x00110000);
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
        this->SetGoldCount(defaultAtom, 0);
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
       - For each possible Tribal unit element, if we have enough gold to create an atom of that type,
       try to create an atom of each type based on the odds for creating that individual type.
       */
      virtual void Behavior(EventWindow<CC>& window) const
      {
        SPoint centerPt = SPoint(0, 0);
        T self = window.GetCenterAtom();
        u32 goldCount = GetGoldCount(self);
        u32 ourTribe = this->GetTribe(self);
        u32 baseType = this->THE_INSTANCE.GetType();

        Random & random = window.GetRandom();

        T empty = Element_Empty<CC>::THE_INSTANCE.GetDefaultAtom();
        const MDist<R> md = MDist<R>::get();

        u32 friendlyBaseCount = this->GetTribalElementCount(window, STABILITY_RANGE, self,
            baseType); // # of bases that we are next to.

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
          else if (neighborType == baseType
              && this->GetTribe(other) == ourTribe)
          {
            friendlyBaseCount++;
          }
        }
        goldCount += goldCollected;
        SetGoldCount(self, goldCount);

        // TODO: Abstract this stuff into a method.
        // Creation loop

        s32 emptyIndex = this->GetFirstEmptySpace(window);
        if (emptyIndex < 0)
        {
          // No space, don't do anything.
        }
        else
        {
          const SPoint& emptySpot = md.GetPoint(emptyIndex);

          // First, checks to see if we have enough gold and then rolls the dice.

          // Base creation.
          if ((goldCount >= m_baseGoldCost.GetValue())
              && random.OneIn(m_baseCreateOdds.GetValue()))
          {
            // Checks passed, create a base.
            T base = Element_Base<CC>::THE_INSTANCE.GetDefaultAtom();
            this->SetTribe(base, ourTribe); // Change the tribe to our own.
            window.SetRelativeAtom(emptySpot, base);
            goldCount -= m_baseGoldCost.GetValue(); // Decrement the cost.
          }
        }

        emptyIndex = this->GetFirstEmptySpace(window);
        if (emptyIndex < 0)
        {
          // No space, don't do anything.
        }
        else
        {
          const SPoint& emptySpot = md.GetPoint(emptyIndex);

          // First, checks to see if we have enough gold and then rolls the dice.

          // Infantry creation.
          if ((goldCount >= m_infantryGoldCost.GetValue())
              && random.OneIn(m_infantryCreateOdds.GetValue()))
          {
            // Checks passed, create the unit.
            T newUnit = Element_Infantry<CC>::THE_INSTANCE.GetDefaultAtom();
            this->SetTribe(newUnit, ourTribe); // Change the tribe to our own.
            window.SetRelativeAtom(emptySpot, newUnit);
            goldCount -= m_infantryGoldCost.GetValue(); // Decrement the cost.
          }
        }

        // Update gold count.
        SetGoldCount(self, goldCount);

        //// Base movement.
        SPoint movePt = centerPt;
        md.FillRandomSingleDir(movePt, random);

        if (window.GetRelativeAtom(movePt).GetType()
            == Element_Empty<CC>::THE_INSTANCE.GetType()
            && window.IsLiveSite(movePt))
        {
          bool shouldSwap = true; // Whether the base should move.
          if (friendlyBaseCount >= 1)
          {
            u32 swapOdds = CONSTRUCT_STABILITY * friendlyBaseCount;
            shouldSwap = random.OneIn(swapOdds);
          }

          if (shouldSwap)
          {
            // Move to this location.
            window.SwapAtoms(movePt, centerPt);
          }
          else
          {
            movePt = centerPt;
          }

        }
        else
        {
          movePt = centerPt;
        }
        // Update any changes made to myself to the window.
        window.SetRelativeAtom(movePt, self);

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
