'''
Created on Nov 16, 2014

@author: lnunno
'''

def main():
    # String for a mine.
    mine_str = 'GA(bc,%d,%d,CD00FC000000000000000020)'
    
    start_x = 0
    end_x = 159
    start_y = 0
    end_y = 95
    width = 160
    grid_size = 30
        
    mine_x = width / 2
    
    for i in range(end_y//4, (3*end_y)//4, 2):
        y = i
        
        # Sides.
        print(mine_str % (mine_x-(grid_size//2), y))
        print(mine_str % (mine_x+(grid_size//2), y))
        
        # Top and bottom.
        print(mine_str % (mine_x//2+i, (end_y//4)-1)) 
        print(mine_str % (mine_x//2+i, 3*end_y//4+1))
    
if __name__ == '__main__':
    main()