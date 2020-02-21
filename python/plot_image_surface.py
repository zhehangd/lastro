import argparse
import os

import matplotlib.pyplot as plt
import numpy as np
import cv2

# This import allows 3d projection
# https://matplotlib.org/mpl_toolkits/mplot3d/tutorial.html
from mpl_toolkits.mplot3d import Axes3D  

def parse_coords(arg):
  coords = tuple(int(x) for x in arg.split(','))
  if len(coords) != 2:
    raise RuntimeError('Invalid coords input: {}'.format(arg))
  return coords

def plot_image(filename, image, x0=0, y0=0):
  x = np.arange(x0, x0+image.shape[1])
  y = np.arange(y0, y0+image.shape[0])
  x2, y2 = np.meshgrid(x, y)
  
  fig = plt.figure(figsize=(19.2, 9.6), tight_layout=True)
  for r in range(2):
    for c in range(3):
      ax = fig.add_subplot(2, 3, r * 3 + c + 1, projection='3d')
      ax.plot_surface(x2, y2, image[:, :, 2 - c], color='rgb'[c])
      ax.set_xlabel('col')
      ax.set_ylabel('row')
      ax.invert_yaxis()
      azim = 45 + (0 if r == 0 else 90)
      ax.view_init(elev=30., azim=azim)
  fig.savefig(filename, dpi='figure')

def crop_window(image, center, wsize):
  h, w = image.shape[:2]
  rs = center[1] - wsize // 2
  re = rs + wsize
  cs = center[0] - wsize // 2
  ce = cs + wsize
  rs = max(0, rs)
  cs = max(0, cs)
  re = min(h, re)
  ce = min(w, ce)
  print(rs,re,cs,ce)
  if (re - rs <= 0 or ce - cs <= 0):
    msg = 'Window centered at {} of size {} is out of the image of size {}x{}'\
      .format(center, wsize, w, h)
    raise RuntimeError(msg)
  return image[rs:re,cs:ce], cs, rs

def fit_image_size(image, maxsize):
  h, w = image.shape[:2]
  if max(w, h) < maxsize:
    return image
  if w > h:
    new_h = (maxsize * h + w - 1) // w
    new_w = maxsize
  else:
    new_w = (maxsize * w + h - 1) // h
    new_h = maxsize
  return cv2.resize(image, (new_w, new_h))  

if __name__ == '__main__':
  
  parser = argparse.ArgumentParser(description='')

  parser.add_argument('image', 
      help='Image to plot.')

  parser.add_argument('--maxsize', type=int, default=600,
      help='Shrink large image so the longer side has this size')

  parser.add_argument('--wsize', type=int, default=32,
      help='Size of the sampled window')

  parser.add_argument('--at', type=parse_coords,
      help='Size of the sampled window')

  args = parser.parse_args()
  
  filename = args.image
  image = cv2.imread(filename, cv2.IMREAD_UNCHANGED)
  if args.at is None:
    image = fit_image_size(image, args.maxsize)
    x0, y0 = (0, 0)
  else:
    image, x0, y0 = crop_window(image, args.at, args.wsize)
  print(image.shape)
  out_filename = os.path.basename(filename) + ".plt.png"
  plot_image(out_filename, image, x0=x0, y0=y0)
