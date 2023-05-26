import re


if __name__ == '__main__':
  """
  main
  """

  import yaml
  
  # yaml config file
  cfg = yaml.safe_load(open("./config.yaml"))["symbol_converter"]

  print("config: ", cfg)


  # read input file
  with open(cfg["input_file"]) as f:
    file_data = f.read()

  # get sym data
  symbol_vars = re.findall(r'unsigned char[\w\s\=\[\]]*\{[\w\s, /]*\}', file_data)

  # go through each var
  for symbol_var in symbol_vars:

    # remove comments
    symbol_var = re.sub(r'/+\s\w+', '', symbol_var)

    # remove whitespaces
    symbol_var = re.sub(r'\s', '', symbol_var)

    # get symbols
    symbol_var = re.findall(r'\d+', symbol_var)

    print(symbol_var)  
