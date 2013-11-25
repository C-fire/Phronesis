#!/usr/bin/python
import sys
import pstats
import os
if len(sys.argv) > 1:
  arg = sys.argv[1]
else:
  arg = ""
os.system("python -m cProfile -o profile.stat phronesisCompiler.py %s"%arg)
p = pstats.Stats('profile.stat')
p.strip_dirs().sort_stats(-1).print_stats()
