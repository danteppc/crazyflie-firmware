import cfusdlog
import numpy as np
logData = cfusdlog.decode("/Volumes/NO NAME/pose00")

print(len(logData["fixedFrequency"]["tdoaEngine.anchorIdA"]))