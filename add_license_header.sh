#!/bin/bash

if [ $# -ne 1 ];
then 
  echo "Usage : $0 <fileName>";
  exit 1
fi

set -o noglob

license='/*\nPhronesis, a diagnosis and recovery tool for system administrators.\nCopyright (C) 2013  Christophe Haen\n\nThis file is part of Phronesis.\n\nPhronesis is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 2 of the License, or\nany later version.\n\nPhronesis is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with Phronesis.  If not, see <http://www.gnu.org/licenses/>.\n*/\n\n'

grep -q "Copyright " $1
grepRc=$?

if [ $grepRc -eq 0 ];
then
  echo "The file $1 has already a license";
  exit 2;
fi

(echo -e $license ; cat $1 ) > "$1".new
mv "$1"{.new,}
