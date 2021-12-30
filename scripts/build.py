import argparse
import os
import platform
import subprocess


def main():
  parser = argparse.ArgumentParser(description='Build Hades.')
  parser.add_argument('mode', metavar='mode', type=str,
                      help='cmake build mode')
  parser.add_argument('-p', dest='program', default=None,
                      help='the program to run')
  parser.add_argument('-j', dest='cores', default=4,
                      help='value for -j to make')

  args = parser.parse_args()

  if args.mode.lower() == 'debug':
    build_dir = '.build_debug'
    cmake_args = '-DCMAKE_BUILD_TYPE=Debug'
  elif args.mode.lower() == 'release':
    build_dir = '.build_release'
    cmake_args = '-DCMAKE_BUILD_TYPE=Release'
  
  if platform.system() == 'Windows':
    cmake_args += '-G "MinGW Makefiles"'

  os.makedirs(build_dir, exist_ok=True)
  os.chdir(build_dir)
  subprocess.call(f"cmake {cmake_args} ..", shell=True)
  subprocess.call(f"cmake --build . -j {args.cores}", shell=True)
  print('\x1b[0m', end='')  # color reset b/c mingw32-make on Windows is bad
  os.chdir('..')

  if args.program is not None:
    subprocess.call(f"./{build_dir}/{args.program}", shell=True)


if __name__ == '__main__':
  main()
