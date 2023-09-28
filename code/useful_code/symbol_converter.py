"""
converts symbols from and to uC presentation
"""

import os
import re
import math
import matplotlib.pyplot as plt
import numpy as np
import cv2
from glob import glob
from pathlib import Path


def plot_image(img):
  """
  plot image
  """

  # setup figure
  fig = plt.figure()

  # create axis
  ax = plt.axes()

  # plot selected mfcc
  im = ax.imshow(img)
  plt.show()


def create_folder(dirs):
  """
  create folders in paths
  """

  # get all folder path to create
  for k, p in dirs.items():

    # path exists check
    if not os.path.isdir(p): os.makedirs(p)


def extract_symbols_to_dict(file_data):
  """
  extract symbols to a nice dict
  """

  # get sym data
  symbol_vars = re.findall(r'unsigned char[\w\s\[\]]*\=\s*\{[\w\s, /]*\}\s*;', file_data)

  # symbold dictionary
  symbols_dict = {}

  # go through each var
  for symbol_var in symbol_vars:

    # extract symbol name
    symbol_name = re.match(r'(?P<var_name>unsigned char) (?P<vn>\w+)', symbol_var).group('vn')

    # get symbol data
    symbol_data = re.match(r'([\w\s\[\]]*\=\s*\{)([\w\s, /]*)(\};)', symbol_var).group(2)

    # remove comments
    symbol_data = re.sub(r'/+\s\w+', '', symbol_data)

    # remove whitespaces
    symbol_data = re.sub(r'\s', '', symbol_data)

    # get symbols digits
    symbol_data = re.findall(r'\d+', symbol_data)

    # make an array
    symbol_data = [int(i) for i in symbol_data]

    # get dimensions
    symbol_dict = {'dim': symbol_data[0:2], 'data': symbol_data[2:]}

    # dict update
    symbols_dict.update({str(symbol_name): symbol_dict})

  return symbols_dict


def convert_vertical_binary_to_bmp(symbol_dict):
  """
  vertical binary to bitmap
  """

  # separate binary imgs
  bin_imgs = np.array(symbol_dict['data']).reshape((-1, math.ceil(symbol_dict['dim'][1] / 8), symbol_dict['dim'][0]))
  print(bin_imgs)
  print(bin_imgs.shape)

  # create bitmap
  bitmap_imgs = np.zeros((len(bin_imgs), symbol_dict['dim'][1], symbol_dict['dim'][0]), dtype='int')

  # for each separate symbol
  for bin_img_idx, bin_img in enumerate(bin_imgs):

    # ravel again
    bin_img = bin_img.ravel()

    # y is partitioned
    for y_part in range(math.ceil(symbol_dict['dim'][1] / 8)):

      # x dimension is the same
      for x in range(symbol_dict['dim'][0]):

        # for each part
        for j, y in enumerate(range(y_part * 8, y_part * 8 + 8)):

          # safety
          if y >= symbol_dict['dim'][1]: continue

          # set bit
          bitmap_imgs[bin_img_idx, y, x] = int(bool(bin_img[y_part * symbol_dict['dim'][0] + x] & (1 << j)))
          #print("x: {}, y: {}, data: {}".format(x, y, bitmap_img[y, x]))

  return bitmap_imgs


def convert_bmp_to_vertical_binary(img):
  """
  image conversion to vertical binary
  """

  # bitmap
  img = np.array(np.array(img, dtype=bool), dtype=int)

  # pad in x dim
  p = 8 - img.shape[0] % 8

  # pad image and reshape
  img_t = np.pad(img, ((0, p), (0, 0))).reshape((-1, 8, img.shape[1]))

  # convert to binary vertical
  conv_bin_vector = np.array([2**i for i in range(8)])

  # matrix vector mult
  bin_img = np.einsum('ijk,j', img_t, conv_bin_vector)

  return bin_img


def symbols_to_bmp(cfg):
  """
  symbols to bitmaps
  """

  # read input file
  with open(cfg["input_file"]) as f:
    file_data = f.read()

  # extract symbols
  symbols_dict = extract_symbols_to_dict(file_data)

  for symbol_name, symbol_dict in symbols_dict.items():
    print("\nnew symbol: [{}] with dim: [{}] ".format(symbol_name, symbol_dict['dim']))
    print(symbol_dict['data'])

    # bmp conversion
    bitmap_imgs = convert_vertical_binary_to_bmp(symbol_dict)

    # write images
    [cv2.imwrite("{}img_{}-{}.pbm".format(cfg["dirs"]["out_pgm"], symbol_name, i), bitmap_img, (cv2.IMWRITE_PXM_BINARY, 0)) for i, bitmap_img in enumerate(bitmap_imgs)]

    # plot image
    #plot_image(bitmap_img)


def bmp_to_symbols(cfg):
  """
  bitmaps to symbols
  """

  # bitmaps to vertical binary format
  bmp_files = sorted(glob(cfg["dirs"]["in_pgm"] + '*.pbm'))

  print(bmp_files)

  # get symbol names
  symbol_names = np.unique([re.split(r'-[0-9]+\.pbm', Path(f).name)[0] for f in bmp_files])

  # symbol dict
  symbols_dict = {s: {'mem_size': 0, 'dim': (0, 0), 'files': [f for f in bmp_files if s == re.split(r'-[0-9]+\.pbm', Path(f).name)[0]]} for s in symbol_names}

  # check and update dim and memory size
  for symbol_name, symbol_dict in symbols_dict.items():

    # check if there are files
    assert len(symbol_dict['files'])

    # init
    dim = (0, 0)
    mem_size = 2

    print("\ncheck files of symbol files: {}".format(symbol_name))
    for symbol_file in symbol_dict['files']:

      # read image
      img = cv2.imread(symbol_file, cv2.IMREAD_GRAYSCALE)

      # check dimensions
      if not all(dim): dim = img.shape
      else: assert dim == img.shape

      # add memory space
      mem_size += (int(img.shape[0] / 8) + (1 if img.shape[0] % 8 else 0)) * img.shape[1]
      print("dim: {} cum. mem size: {}".format(img.shape, mem_size))

    # update meta data
    symbols_dict[symbol_name]['dim'] = dim
    symbols_dict[symbol_name]['mem_size'] = mem_size

  # out text
  out_text = ''

  # go through each file
  for symbol_name, symbol_dict in symbols_dict.items():
    print("\nextract data of: ", symbol_name)

    # mem allocation size
    symbol_text = '\n#define {}_LEN {}'.format(symbol_name, symbol_dict['mem_size'])

    # variable definition
    symbol_text += '\nunsigned char {}[{}_LEN] =\n{{'.format(symbol_name, symbol_name)

    # dimension
    symbol_text += '\n\t// size\n\t{}, {},'.format(symbol_dict['dim'][1], symbol_dict['dim'][0])

    for symbol_file in symbol_dict['files']:
      print("file: ", symbol_file)
      symbol_text += '\n\n\t// {}'.format(Path(symbol_file).name)

      # read image
      img = cv2.imread(symbol_file, cv2.IMREAD_GRAYSCALE)

      # get dimensions
      dim = img.shape

      # get image
      bin_img = convert_bmp_to_vertical_binary(img)

      # symbol data
      for row_data in bin_img:
        symbol_text += '\n\t'
        for d in row_data:
          symbol_text += '{}, '.format(d)

    # add end and add to overall text
    symbol_text += '\n};\n'
    out_text += symbol_text

  # debug
  #print(out_text)

  # write file
  with open(cfg["output_file"], 'w') as f:
    f.write(out_text)

  print("\n--\nfile: {} written, sucess.\n".format(cfg["output_file"]))


if __name__ == '__main__':
  """
  main
  """

  import yaml
  
  # yaml config file
  cfg = yaml.safe_load(open("./config.yaml"))["symbol_converter"]
  print("config: ", cfg)

  create_folder(cfg['dirs'])

  # symbols to bitmaps
  #symbols_to_bmp(cfg)

  # bitmaps to symbols
  bmp_to_symbols(cfg)