/*                                              -*- mode:C++ -*-
 Element_Mine.h Tutorial template for a new Element
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

/**
 \file   Element_Mine.h Mine element, emits Res.
 \author Lucas L. Nunno
 \date (C) 2014 All rights reserved.
 \lgpl
 */
#ifndef ELEMENT_MINE_H
#define ELEMENT_MINE_H

#include "Element.h"
#include "EventWindow.h"
#include "ElementTable.h"
#include "Element_Base.h"
#include "itype.h"
#include <iostream>

using namespace std;

namespace MFM
{

#define MINE_VERSION 1

  template<class CC>
  class Element_Mine: public Element<CC>
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
        EXHAUST_COUNTER_LENGTH = 20,
        EXHAUST_COUNTER_POSITION = AbstractElement_Tribal<CC>::TRIBAL_FIRST_FREE_POSITION
            - EXHAUST_COUNTER_LENGTH

      };

      typedef BitField<BitVector<BITS>, VD::U32, EXHAUST_COUNTER_LENGTH,
          EXHAUST_COUNTER_POSITION> ExhaustField;

      u32 GetExhaustCount(const T& us) const
      {
        return ExhaustField::Read(this->GetBits(us));
      }

      void SetExhaustCount(T& us, u32 newCount) const
      {
        ExhaustField::Write(this->GetBits(us), newCount);
      }

    private:

      ElementParameterS32<CC> m_resSpawnOdds;
      ElementParameterS32<CC> m_exhaustionRate;

    public:

      static Element_Mine<CC> THE_INSTANCE;

      Element_Mine() :
              Element<CC>(MFM_UUID_FOR("Mine", MINE_VERSION)),
              m_resSpawnOdds(this, "spawn", "Res Spawn Odds",
                  "The probability that this mine will spawn a Res.", 1, 5, 50),
              m_exhaustionRate(this, "exhaust", "Exhaustion Odds",
                  "The rate at which a mine gets exhausted.", 1, 100, 500)
      {
        Element<CC>::SetAtomicSymbol("Mn");
        Element<CC>::SetName("Mine");
      }

      /*
       <<TEMPLATE>> Set how likely your element is to be moved by another element. See
       Element.h for details.
       */
      virtual u32 PercentMovable(const T& you, const T& me,
          const SPoint& offset) const
      {
        return 0; // Mines are not movable.
      }

      virtual u32 Diffusability(EventWindow<CC> & ew, SPoint nowAt,
          SPoint maybeAt) const
      {
        return this->NoDiffusability(ew, nowAt, maybeAt);
      }

      /* <<TEMPLATE>> This color will be the default rendering color for your element. */
      virtual u32 DefaultPhysicsColor() const
      {
        return 0x99663300;
      }

      /*
       <<TEMPLATE>> This color will be the color rendered when your
       element is rendered in lowlighting.
       */
      virtual u32 DefaultLowlightColor() const
      {
        return 0x99663300;
      }

      /*
       <<TEMPLATE>> This is a short description of your element.
       */
      virtual const char* GetDescription() const
      {
        return "A producer of Res. Stationary.";
      }

      static const u32 TYPE()
      {
        return THE_INSTANCE.GetType();
      }

      virtual const T& GetDefaultAtom() const
      {
        static T defaultAtom(TYPE(), 0, 0, 0);
        this->SetExhaustCount(defaultAtom, 1);
        return defaultAtom;
      }

      /*
       <<TEMPLATE>> This method is executed every time an atom of your
       element is chosen for an event. See the tutorial in
       the wiki for further information.

       This code is similar to the behavior of DReg, but even simpler!
       Does not do any deletions.
       */
      virtual void Behavior(EventWindow<CC>& window) const
      {
        SPoint centerPt = SPoint(0, 0);
        T self = window.GetCenterAtom();
        u32 exhaustCounter = this->GetExhaustCount(self);

        const MDist<R>& md = MDist<R>::get();
        Random& random = window.GetRandom();

        SPoint dir;
        md.FillRandomSingleDir(dir, random);

        T atom = window.GetRelativeAtom(dir);
        u32 atomType = atom.GetType();
        u32 spawnChance = m_resSpawnOdds.GetValue() * exhaustCounter;
        bool shouldSpawn = random.OneIn(spawnChance);
        if (Element_Empty<CC>::THE_INSTANCE.IsType(atomType))
        {

          if (shouldSpawn)
          {
            // Create the Res here.
            atom = Element_Res<CC>::THE_INSTANCE.GetDefaultAtom();
            window.SetRelativeAtom(dir, atom);

            u32 exhaustChance = m_exhaustionRate.GetValue();
            if (random.OneIn(exhaustChance))
            {
              u32 a = exhaustCounter / 10;
              u32 incr = (a > 1) ? a : 1;

              // Increment exhaustion counter.
              this->SetExhaustCount(self, exhaustCounter + incr);
            }
          }
          // Else we didn't get lucky, don't do anything.
        }
        else if (atomType == Element_Base<CC>::TYPE())
        {
          // TODO: How should we handle this? We can't access the base's internals directly.

          // This is a base.
          if (shouldSpawn)
          {
            // Give this base the res.
            Element_Base<CC>::THE_INSTANCE.IncrementGoldCount(atom);

            // Update the base in the event window.
            window.SetRelativeAtom(dir,atom);
          }

        }

        window.SetCenterAtom(self); // Update myself.
      }
  };

  template<class CC>
  Element_Mine<CC> Element_Mine<CC>::THE_INSTANCE;

}

#endif /* ELEMENT_MINE_H */
