'''
Created on Nov 12, 2014

@author: lnunno
'''
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('file_path')
    parser.add_argument('save_path')
    args = parser.parse_args()
    df = pd.DataFrame.from_csv(args.file_path)
    cols = ['Epoch AEPS', 'Res','Red Base']
    df = df[cols]
    df.plot(x='Epoch AEPS' ,color=['y','r'], logx=True)
    plt.ylabel('Number of Atoms')
    plt.title('Base self-healing in a DReg saturated environment')
    plt.savefig(args.save_path)    
    print('Saved to: %s' % (os.path.abspath(args.save_path)))