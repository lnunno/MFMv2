#include "main.h"
#include "Element_Infantry.h"
#include "Element_Base.h"
#include "Element_Res.h"
#include "AbstractElement_Tribal.h"

namespace MFM
{
  /////
  // Standard model

  typedef ParamConfig<96, 4, 8, 40> OurParamConfigStd;
  typedef P3Atom<OurParamConfigStd> OurAtomStd;
  typedef CoreConfig<OurAtomStd, OurParamConfigStd> OurCoreConfigStd;
  typedef GridConfig<OurCoreConfigStd, 5, 3> OurGridConfigStd;

  /////
  // Tiny model

  typedef ParamConfig<96, 4, 8, 32> OurParamConfigTiny;
  typedef P3Atom<OurParamConfigTiny> OurAtomTiny;
  typedef CoreConfig<OurAtomTiny, OurParamConfigTiny> OurCoreConfigTiny;
  typedef GridConfig<OurCoreConfigTiny, 2, 2> OurGridConfigTiny;

  /////
  // Larger model

  typedef ParamConfig<96, 4, 8, 48> OurParamConfigBig;
  typedef P3Atom<OurParamConfigBig> OurAtomBig;
  typedef CoreConfig<OurAtomBig, OurParamConfigBig> OurCoreConfigBig;
  typedef GridConfig<OurCoreConfigBig, 8, 5> OurGridConfigBig;

  template<class GC>
  struct MFMCDriver: public AbstractDualDriver<GC>
  {
    private:

      const char* GetSimDirPathTemporary(const char* format, ...) const
      {
        static OverflowableCharBufferByteSink<500> buf;
        buf.Reset();
        buf.Printf("%s", this->GetSimulationBasePath());
        va_list ap;
        va_start(ap, format);
        buf.Vprintf(format, ap);
        if (buf.HasOverflowed())
        {
          FAIL(OUT_OF_ROOM);
        }
        return buf.GetZString();
      }

      typedef AbstractDualDriver<GC> Super;
      typedef typename Super::CC CC;
      typedef typename CC::PARAM_CONFIG P;
      typedef typename CC::ATOM_TYPE T;

      virtual void DefineNeededElements()
      {
        this->NeedElement(&Element_Empty<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Wall<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Res<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Dreg<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Sorter<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Data<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Emitter<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Consumer<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Block<CC>::THE_INSTANCE);
        this->NeedElement(&Element_ForkBomb1<CC>::THE_INSTANCE);
        this->NeedElement(&Element_ForkBomb2<CC>::THE_INSTANCE);
        this->NeedElement(&Element_ForkBomb3<CC>::THE_INSTANCE);
        this->NeedElement(&Element_AntiForkBomb<CC>::THE_INSTANCE);
        this->NeedElement(&Element_MQBar<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Mover<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Indexed<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Fish<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Shark<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Xtal_Sq1<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Xtal_L12<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Xtal_R12<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Xtal_General<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Creg<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Dmover<CC>::THE_INSTANCE);
        this->NeedElement(&Element_CheckerForkBlue<CC>::THE_INSTANCE);
        this->NeedElement(&Element_CheckerForkRed<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Wanderer_Cyan<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Wanderer_Magenta<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Mine<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Base<CC>::THE_INSTANCE);
        this->NeedElement(&Element_Infantry<CC>::THE_INSTANCE);
      }

    public:

      virtual void DoEpochEvents(Grid<GC>& grid, u32 epochs, u32 epochAEPS)
      {
        u32 H = Grid<GC>::GetHeight();
        u32 W = Grid<GC>::GetWidth();

        u32 numRes = grid.GetAtomCount(Element_Res<CC>::TYPE());

        u32 redBases = 0;
        u32 redInf = 0;

        u32 blueBases = 0;
        u32 blueInf = 0;

        Element_Base<CC> base = Element_Base<CC>::THE_INSTANCE;
        Element_Infantry<CC> infantry = Element_Infantry<CC>::THE_INSTANCE;

        for (u32 y = 0; y < H; ++y)
        {
          for (u32 x = 0; x < W; ++x)
          {
            Tile<CC> & tile = grid.GetTile(SPoint(x, y));
            for (u32 x = 0; x < P::TILE_WIDTH; ++x)
            {
              for (u32 y = 0; y < P::TILE_WIDTH; ++y)
              {
                const SPoint pt(x, y);
                if (Tile<CC>::IsInCache(pt))
                {
                  continue;
                }

                const T * atom = tile.GetAtom(x, y);
                u32 atomType = atom->GetType();
                const Element<CC> * elt = tile.GetElement(atomType);

                if (dynamic_cast<const AbstractElement_Tribal<CC>*>(elt))
                {
                  // This is a tribal atom.
                  u32 tribe = base.GetTribe(*atom);
                  if (atomType == base.TYPE())
                  {
                    if (tribe == AbstractElement_Tribal<CC>::RED)
                    {
                      redBases++;
                    }
                    else if (tribe == AbstractElement_Tribal<CC>::BLUE)
                    {
                      blueBases++;
                    }
                  }
                  else if (atomType == infantry.TYPE())
                  {
                    if (tribe == AbstractElement_Tribal<CC>::RED)
                    {
                      redInf++;
                    }
                    else if (tribe == AbstractElement_Tribal<CC>::BLUE)
                    {
                      blueInf++;
                    }
                  }
                }
              }
            }
          }
        }

        const char* path = GetSimDirPathTemporary("tbd/tribe_counts.csv");
        FILE* fp = fopen(path, "a");

        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", epochs, epochAEPS, numRes,
            redBases, redInf, redBases + redInf, blueBases, blueInf,
            blueBases + blueInf);
        fclose(fp);
        // Remember to let the parent run too!
        Super::DoEpochEvents(grid, epochs, epochAEPS);
      }

      virtual void AddDriverArguments()
      {
        Super::AddDriverArguments();
      }

      virtual void OnceOnly(VArguments& args)
      {
        Super::OnceOnly(args);
      }

      virtual void ReinitEden()
      {
      }
  };

  static int RunSmall(int argc, const char** argv)
  {
    MFMCDriver<OurGridConfigTiny> sim;
    sim.Init(argc, argv);
    sim.Reinit();
    sim.Run();
    return 0;
  }

  static int RunMedium(int argc, const char** argv)
  {
    MFMCDriver<OurGridConfigStd> sim;
    sim.Init(argc, argv);
    sim.Reinit();
    sim.Run();
    return 0;
  }

  static int RunBig(int argc, const char** argv)
  {
    MFMCDriver<OurGridConfigBig> sim;
    sim.Init(argc, argv);
    sim.Reinit();
    sim.Run();
    return 0;
  }
}

static bool EndsWith(const char *string, const char* suffix)
{
  MFM::u32 slen = strlen(string);
  MFM::u32 xlen = strlen(suffix);
  return xlen <= slen && !strcmp(suffix, &string[slen - xlen]);
}

int main(int argc, const char** argv)
{
  MFM::DateTimeStamp stamper;
  MFM::LOG.SetTimeStamper(&stamper);
  MFM::LOG.SetByteSink(MFM::STDERR);
  MFM::LOG.SetLevel(MFM::LOG.MESSAGE);

  if (EndsWith(argv[0], "_s"))
  {
    return MFM::RunSmall(argc, argv);
  }

  if (EndsWith(argv[0], "_l"))
  {
    return MFM::RunBig(argc, argv);
  }

  return MFM::RunMedium(argc, argv);
}
