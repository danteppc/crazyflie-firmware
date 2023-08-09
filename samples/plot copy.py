#!/usr/bin/env python3
import numpy as np
import holoviews as hv
from holoviews.operation import histogram
import pandas as pd
from itertools import combinations
from holoviews.operation import timeseries

hv.extension('matplotlib')
df = pd.read_csv('posesample-Apr-17-2023_1201.csv')

attribs = ['pose.x', 'pose.y', 'pose.z']
colors = ['red', 'green', 'blue']
combs = combinations(attribs, 2)
plots = []
for (i,comb) in enumerate(combs):
	points = hv.Points(df, kdims=list(comb))
	plot = hv.Bivariate(points).opts(colorbar=True, cmap='inferno', filled=True)
	plots.append(plot)

for (i, attrib) in enumerate(attribs):
	curve = hv.Curve(df, "timestamp", attrib)
	curve.opts(color=colors[i])
	hv.save(timeseries.rolling(curve, rolling_window=200).opts(aspect=4, fig_inches=10, fig_bounds=(0, 0, 1, 1)), f"curve{i}.pdf", fmt='pdf')
	
	
layout = hv.Layout(plots)



	