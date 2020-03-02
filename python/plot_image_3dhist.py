import argparse
import os

import matplotlib.pyplot as plt
import numpy as np
import cv2

from matplotlib import cm

# This import allows 3d projection
# https://matplotlib.org/mpl_toolkits/mplot3d/tutorial.html
from mpl_toolkits.mplot3d import Axes3D  

def parse_coords(arg):
  coords = tuple(int(x) for x in arg.split(','))
  if len(coords) != 2:
    raise RuntimeError('Invalid coords input: {}'.format(arg))
  return coords

def plot_image(filename, image, x0=0, y0=0):
  
  
  data_lab = cv2.cvtColor(image, cv2.COLOR_BGR2LAB).reshape(-1, 3).astype(np.float32)
  data_bgr = image.reshape(-1, 3).astype(np.float32)
  
  colors = data_bgr.astype(np.float32) / 255.0
  colors = np.flip(colors, -1)
  
  xs = (data_lab[:, 1] - 128) / 128.0
  ys = (data_lab[:, 2] - 128) / 128.0
  zs = data_lab[:, 0] / 255.0
  xmin = np.amin(xs)
  ymin = np.amin(ys)
  zmin = np.amin(zs)
  xmax = np.amax(xs)
  ymax = np.amax(ys)
  zmax = np.amax(zs)
  
  fig = plt.figure(figsize=(19.2, 9.6), tight_layout=True)
  for i in range(6):
    azim = np.linspace(0, 180, 6)[i]
    ax = fig.add_subplot(2, 3, i + 1, projection='3d')
    ax.scatter(xs, ys, zs, color=colors)
    ax.set_xlabel('a')
    ax.set_ylabel('b')
    ax.set_zlabel('L')
    
    zs_xz = ymin if azim >= 0 and azim <= 180 else ymax
    zs_yz = xmin if azim >= -90 and azim <= 90 else xmax
    zs_xy = zmin
    
    alpha = 0.05
    ax.plot(xs, zs, 'r+', zdir='y', zs=zs_xz, alpha=alpha)
    ax.plot(ys, zs, 'g+', zdir='x', zs=zs_yz, alpha=alpha)
    ax.plot(xs, ys, 'k+', zdir='z', zs=zs_xy, alpha=alpha)
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)
    ax.set_zlim(zmin, zmax)
    ax.view_init(elev=30., azim=azim)
  fig.savefig(filename, dpi='figure')
  plt.show()

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
  print("Shrink iamge size to {}".format((new_w, new_h)))
  return cv2.resize(image, (new_w, new_h))  

if __name__ == '__main__':
  
  parser = argparse.ArgumentParser(description='')

  parser.add_argument('image', 
      help='Image to plot.')

  parser.add_argument('--maxsize', type=int, default=50,
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
  out_filename = os.path.basename(filename) + ".plt.png"
  plot_image(out_filename, image, x0=x0, y0=y0)
