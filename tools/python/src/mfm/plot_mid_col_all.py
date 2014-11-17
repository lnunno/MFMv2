'''
Created on Nov 12, 2014

@author: lnunno
'''
import pandas as pd
import matplotlib.pyplot as plt
import os

if __name__ == '__main__':
    plt.figure()
    file_path = '/home/lnunno/Dropbox/UNM/Fall14/Robust_Artificial_Life/project/runs/symmetric/mid_col_counts_3kaeps.csv'
    save_path = '/home/lnunno/Dropbox/UNM/Fall14/Robust_Artificial_Life/project/paper/mid_col_3kaeps_infantry'
    df = pd.DataFrame.from_csv(file_path)
    cols = ['Epoch AEPS', 'Res','Red Infantry', 'Blue Infantry']
    df = df[cols]
    df.plot(x='Epoch AEPS', style=['y-','r-.','b--'], logx=True, logy=True)
    plt.ylabel('Number of Atoms')
    plt.title('Growth of tribes with a central column of mines')
    plt.savefig(save_path)    
    print('Saved to: %s' % (os.path.abspath(save_path)))
    plt.close()
    
    plt.figure()
    file_path = '/home/lnunno/Dropbox/UNM/Fall14/Robust_Artificial_Life/project/runs/symmetric/mid_col_counts_3kaeps.csv'
    save_path = '/home/lnunno/Dropbox/UNM/Fall14/Robust_Artificial_Life/project/paper/mid_col_3kaeps_base'
    df = pd.DataFrame.from_csv(file_path)
    cols = ['Epoch AEPS', 'Res','Red Base', 'Blue Base']
    df = df[cols]
    df.plot(x='Epoch AEPS', style=['y-','r-.','b--'], logx=True, logy=True)
    plt.ylabel('Number of Atoms')
    plt.title('Growth of tribes with a central column of mines')
    plt.savefig(save_path)    
    print('Saved to: %s' % (os.path.abspath(save_path)))
    plt.close()