import os
for filename in os.listdir(os.getcwd()):
    if filename.endswith('.csv') and filename.startswith('posesample'):
        print(filename)