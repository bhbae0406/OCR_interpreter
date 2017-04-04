# -*- coding: utf-8 -*-

# usage: $ python <input dir> <output dir> <image num>
# e.g. python ./ ./ 0036

import numpy as np
import networkx as nx
import cv2
import json
import sys

# paremeters
scale = 0.2
ThresholdDelta = 4
ap_ratio_threshold = 0.1     # filter those too big
area_low_threshold = 0.75   # quantile
ratio_to_whole = 0.6  # the ratio between height(width) to whole height(width)
height_width = 4      # the ratio between height and width
low_int = 0.1
high_int = 0.9


# scripts

## inputs
in_dir = sys.argv[1]
out_dir = sys.argv[2]
jpgname = sys.argv[3]

## read image
roi = cv2.imread(in_dir+jpgname+'.jpg')
I = cv2.resize(roi, (0,0), fx=scale, fy=scale)
I = cv2.cvtColor(I, cv2.COLOR_BGR2GRAY)
colorImage = I;

## Detect MSER regions
mser = cv2.MSER_create(_delta=ThresholdDelta, _min_area=200, _max_area=8000)
try:
	mserRegions, _ = mser.detectRegions(I)
except:
	mserRegions = mser.detectRegions(I, None)

bboxes = [cv2.boundingRect(p) for p in mserRegions]
bboxes = list(set(bboxes))
          
## filter
filterIdx = np.array([False]*len(bboxes))

### area
areas = np.array([b[2]*b[3] for b in bboxes])
low_th = np.percentile(areas, area_low_threshold*100)
filterIdx = np.logical_or(filterIdx, areas<low_th)

### height and width
heights = np.array([b[3] for b in bboxes])
widths  = np.array([b[2] for b in bboxes])
whole_height = I.shape[0]
whole_width  = I.shape[1]
filterIdx = np.logical_or(filterIdx, heights>ratio_to_whole*whole_height)
filterIdx = np.logical_or(filterIdx, widths>ratio_to_whole*whole_width)
filterIdx = np.logical_or(filterIdx, widths>heights*height_width)
filterIdx = np.logical_or(filterIdx, heights>widths*height_width)

### filtering
new_bboxes = []
for i in range(0, len(filterIdx)):
    if filterIdx[i]==False:
        new_bboxes.append(bboxes[i])
bboxes = new_bboxes

## generating blocks
n = len(bboxes)
adj_mat = np.zeros(shape=(n,n))
def is_overlap(a, b):
    return (abs(a[0]-b[0]) < (a[2]+b[2])/2) & (abs(a[1]-b[1]) < (a[3]+b[3])/2);

for i in range(0,n):
    for j in range(0,n):
        adj_mat[i,j] = is_overlap(bboxes[i], bboxes[j])
    adj_mat[i,i] = 0

G=nx.from_numpy_matrix(adj_mat)
concmp = nx.connected_components(G)

blocks = []
for indexes in concmp:
    x_arr = np.array([])
    y_arr = np.array([])
    for idx in indexes:
        x_arr=np.append(x_arr, bboxes[idx][0])
        y_arr=np.append(y_arr, bboxes[idx][1])
        x_arr=np.append(x_arr, bboxes[idx][0]+bboxes[idx][2])
        y_arr=np.append(y_arr, bboxes[idx][1]+bboxes[idx][3])
    x = x_arr.min()
    y = y_arr.min()
    width = x_arr.max()-x_arr.min()
    height = y_arr.max()-y_arr.min()
    blocks.append([x, y, width, height])

# generate json results
anns = []
id = 0
for block in blocks:
    anns.append({"class": "graphic",
                          "height": block[3]/scale,
                          "id": str(id),
                          "type": "rect",
                          "width": block[2]/scale,
                          "x": block[0]/scale,
                          "y": block[1]/scale})
    seg = {
            "annotations": anns,
          }
          
with open(out_dir+jpgname+'.json','w') as f:
    json.dump(seg, f, indent=4)
