import csv
  
op = open("posesample-Jan-10-2023_1252.csv", "r")
dt = csv.DictReader(op)
print(dt)
up_dt = []
counter = 0
for r in dt:
    if counter%2 == 0:
      up_dt.append(r)
    counter = counter + 1
op.close()
op = open("posesample-Jan-10-2023_1252.csv", "w", newline='')
headers = ['timestamp', 'pose.x', 'pose.y']
data = csv.DictWriter(op, delimiter=',', fieldnames=headers)
data.writerow(dict((heads, heads) for heads in headers))
data.writerows(up_dt)
  
op.close()