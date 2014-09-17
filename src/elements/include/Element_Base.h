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
#include "itype.h"
#include "AbstractElement_Tribal.h"

namespace MFM
{

  #define BASE_VERSION 1

  template <class CC>
  class Element_Base : public AbstractElement_Tribal<CC> /* <<TEMPLATE>> Replace class name with yours */
  {
    // Extract short names for parameter types
    typedef typename CC::ATOM_TYPE T;
    typedef typename CC::PARAM_CONFIG P;
    enum { R = P::EVENT_WINDOW_RADIUS };

  private:

    ElementParameterS32<CC> m_goldPerRes;

    /* <<TEMPLATE>> Add any configurable parameters here. */

  public:

    /* <<TEMPLATE>> Replace class name with yours. Don't forget the '<CC>'. */
    static Element_Base<CC> THE_INSTANCE;

    Element_Base()
      : AbstractElement_Tribal<CC>(MFM_UUID_FOR("Base", BASE_VERSION)),
        /* <<TEMPLATE>> Initialize all configurable parameters here. */
        m_goldPerRes(this, "goldPRes", "Gold Per Res",
                  "The amount of gold produced for each Res collected.", 1, 10, 100, 1)
    {
      /* <<TEMPLATE>> Set atomic symbol and name for your element. */
      Element<CC>::SetAtomicSymbol("Bs");
      Element<CC>::SetName("Base");
    }

    /*
       <<TEMPLATE>> Set how likely your element is to be moved by another element. See
                    Element.h for details.
    */
    virtual u32 PercentMovable(const T& you,
                               const T& me, const SPoint& offset) const
    {
      return 0;
    }

    /* <<TEMPLATE>> This color will be the default rendering color for your element. */
    virtual u32 DefaultPhysicsColor() const
    {
      return 0xFF0000FF;
    }

    /*
       <<TEMPLATE>> This color will be the color rendered when your
                    element is rendered in lowlighting.
     */
    virtual u32 DefaultLowlightColor() const
    {
      return 0xFF0000FF;
    }

    /*
      <<TEMPLATE>> This is a short description of your element.
     */
    virtual const char* GetDescription() const
    {
      return "This is a base that creates other units and collects Res.";
    }

    /*
      <<TEMPLATE>> This method is executed every time an atom of your
                   element is chosen for an event. See the tutorial in
                   the wiki for further information.
     */
    virtual void Behavior(EventWindow<CC>& window) const
    {
    }
  };

  /*
     <<TEMPLATE>> Rename the class names here to the class name of your element.
  */
  template <class CC>
  Element_Base<CC> Element_Base<CC>::THE_INSTANCE;

}

#endif /* Element_Base_H */ /* <<TEMPLATE>> Rename the header guard to your element */
