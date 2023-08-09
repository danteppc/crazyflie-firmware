import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import os
for filename in os.listdir(os.getcwd()):
    if filename.endswith('.csv') and filename.startswith('posesample-Apr-17-2023_1211.csv'):
        df = pd.read_csv(filename)
        fig, ax = plt.subplots()
        x_margin = (df['pose.x'].max() - df['pose.x'].min())*0.1
        y_margin = (df['pose.y'].max() - df['pose.y'].min())*0.1
        ax.set_xlim(df['pose.x'].min()-x_margin, df['pose.x'].max()+x_margin)
        ax.set_ylim(df['pose.y'].min()-y_margin, df['pose.y'].max()+y_margin)
        #plt.axis("equal")
        #x_range = df['pose.x'].max() - df['pose.x'].min()
        #y_range = df['pose.y'].max() - df['pose.y'].min()
        #aspect_ratio = x_range / y_range
        #ax.set_aspect(aspect_ratio)
        x = []
        y = []
        scat1 = ax.scatter(x, y,s=50,color='green', marker='.')
        #scat2 = ax.scatter(x, y,s=5,color='black', marker='.')
        
        def update(frame):
            x_new = df['pose.x'][frame]
            y_new = df['pose.y'][frame]
            t = df['timestamp'][frame]
            
            x.append(x_new)
            y.append(y_new)
            
            scat1.set_offsets(list(zip(x, y)))
            #if t>92000:
            #    scat1.set_offsets(list(zip(x, y)))
            #else:
            #    scat2.set_offsets(list(zip(x, y)))

            return scat1,#scat2,

                       
        ax.set_aspect('equal', 'box')
        ani = FuncAnimation(fig, update, interval=1, frames=len(df), blit=True)
        mng = plt.get_current_fig_manager()
        mng.full_screen_toggle()
        plt.show()
        ani.save(filename+'.mp4', writer='ffmpeg', fps=60)

#ani.save('animation.mp4', writer='ffmpeg', fps=60)