
from itertools import product
import hashlib

def flatten_dict(d, prefix=None):
    if isinstance(d, dict):
        entries = []
        for key, value in d.items():
            new_prefix = key if prefix is None else '.'.join([prefix, key])
            entries += flatten_dict(value, prefix=new_prefix)
        return entries
    else:
        return [(prefix, d)]

def get_configs(settings):
    settings = dict(flatten_dict(settings))
    keys = list(settings.keys())
    values = list(settings.values())
    configs = [{key: value for key, value in zip(keys, setting)} for setting in product(*values)]
    return configs

def get_config_code_and_summary (config):
    '''
    Construct a sha256 hexadecimal code using the config dictionary.
    '''
    summary = ' '.join(k+'('+str(v)+')' for k, v in config.items())
    return hashlib.sha256(bytes(summary, encoding='utf8')).hexdigest(), summary

def printProgressBar (progress, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        length      - Optional  : character length of bar (Int)
        fill        - Optional  : bar fill character (Str)
        printEnd    - Optional  : end character (e.g. "\r", "\r\n") (Str)
    """
    percent = ("{0:." + str(decimals) + "f}").format(100 * (progress))
    filledLength = int(length * progress)
    bar = fill * filledLength + '-' * (length - filledLength)
    # print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = "\n")
    print(f'\r{prefix} {percent}% {suffix}', end = "\n")