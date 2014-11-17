'''
Created on Nov 16, 2014

@author: lnunno
'''
def main():
    reg_str = '''\
RegisterElement(Empty-108348428608201411126152204,a)
RegisterElement(Wall-118348428608201411126152204,b)
RegisterElement(Res-118348428608201411126152204,c)
RegisterElement(Dreg-118348428608201411126152204,d)
 SetElementParameter(d,res,200)
 SetElementParameter(d,del,100)
 SetElementParameter(d,dreg,500)
 SetElementParameter(d,ddreg,50)
RegisterElement(Sorter-118348428608201411126152204,e)
RegisterElement(Data-118348428608201411126152204,f)
RegisterElement(Emitter-118348428608201411126152204,g)
RegisterElement(Consumer-118348428608201411126152204,h)
RegisterElement(Block-118348428608201411126152204,i)
RegisterElement(BombRed-118348428608201411126152204,j)
 SetElementParameter(j,range,2)
RegisterElement(BombBlue-118348428608201411126152204,k)
 SetElementParameter(k,range,2)
RegisterElement(BombYellow-118348428608201411126152204,l)
 SetElementParameter(l,count,10)
RegisterElement(AntiFork-118348428608201411126152204,m)
RegisterElement(MQBar-148348428608201411126152204,n)
RegisterElement(Mover-118348428608201411126152204,o)
RegisterElement(Indexed-118348428608201411126152204,p)
RegisterElement(Fish-118348428608201411126152204,q)
 SetElementParameter(q,age,20)
 SetElementParameter(q,evo,false)
RegisterElement(Shark-118348428608201411126152204,r)
 SetElementParameter(r,age,15)
 SetElementParameter(r,energy,8)
RegisterElement(XtalSq1-118348428608201411126152204,s)
RegisterElement(XtalL12-118348428608201411126152204,t)
RegisterElement(XtalR12-118348428608201411126152204,u)
RegisterElement(XtalGen-118348428608201411126152204,v)
 SetElementParameter(v,neighborhood,[])
RegisterElement(Creg-118348428608201411126152204,w)
 SetElementParameter(w,density,3)
RegisterElement(Dmover-128348428608201411126152204,mine_x)
 SetElementParameter(mine_x,radius,1)
RegisterElement(CheckerForkBlue-118348428608201411126152204,y)
RegisterElement(CheckerForkRed-118348428608201411126152204,z)
RegisterElement(WandererCyan-118348428608201411126152204,ba)
 SetElementParameter(ba,cyanWanderDistance,1)
RegisterElement(WandererMagenta-118348428608201411126152204,bb)
 SetElementParameter(bb,magentaWanderDistance,1)
RegisterElement(Mine-118348428608201411126152204,bc)
 SetElementParameter(bc,spawn,5)
 SetElementParameter(bc,exhaust,100)
RegisterElement(Base-118348428608201411126152204,bd)
 SetElementParameter(bd,tribe,2)
 SetElementParameter(bd,goldPRes,5)
 SetElementParameter(bd,baseGoldCost,5)
 SetElementParameter(bd,baseCreate,5)
 SetElementParameter(bd,infGoldCost,1)
 SetElementParameter(bd,infCreate,3)
RegisterElement(Infantry-118348428608201411126152204,be)
 SetElementParameter(be,tribe,1)
 SetElementParameter(be,movChng,10)
 SetElementParameter(be,killOdds,4)
'''
    
    # String for a mine.
    mine_str = 'GA(bc,%d,%d,CD00FC000000000000000020)'
    red_base_str = 'GA(bd,%d,%d,3D80FF800000000000000002)'
    blue_base_str = 'GA(bd,%d,%d,3D80FF800000000000000004)'
    
    start_x = 0
    end_x = 159
    grid_width = 160
    x_offset = 10
        
    mine_x = grid_width / 2
    red_base_x = start_x + x_offset
    blue_base_x = end_x - x_offset 
    for i in range(0, 95, 2):
        y = i
        print(mine_str % (mine_x, y))
        print(red_base_str % (red_base_x, y))
        print(blue_base_str % (blue_base_x, y))
    
if __name__ == '__main__':
    main()
