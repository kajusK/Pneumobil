#!/usr/bin/python
# -*- coding: utf-8 -*-

#    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.
#
#    This file is part of BUT pneumobil.
#
#    BUT pneumobil is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    BUT pneumobil is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
Generate table of commands from protocols.ods file
"""
import argparse
import json
import re
from pyexcel_odsr import get_data

SOURCE = "../../doc/specs/protocols.ods"
TARGET = "../../fw/sources/modules/comm/comm.h"
SHEET = "Commands"
REGEX = '\/\*.*$\ntypedef enum {[ \t\w\d\/\*,_=\n\r]*} comm_cmd_id_t;'
COL_ID = 0
COL_NAME = 1
COL_DESC = 2

desc = "Generate comm_cmd_id_t table from %s to %s" % (SOURCE, TARGET)
parser = argparse.ArgumentParser(description=desc)
parser.add_argument('-d, --dry-run', dest="dry", action='store_true',
        help="Don't modify the file, just print to console")
args = parser.parse_args()

data = get_data(SOURCE);
enum = "/** Command IDs - autogenerated by sw/generators/comm_cmd.py */\n"
enum += "typedef enum {\n"
block_space = False

for line in data[SHEET]:
    if len(line) == 1 or (len(line) >= 3 and
            len(line[COL_ID]) and len(line[COL_NAME]) == 0
            and len(line[COL_DESC]) == 0):
        if block_space:
            enum += "\n"
        enum += "/* "+line[COL_ID]+"*/\n"
        block_space = True
        continue

    if len(line) < 3:
        continue

    if len(line[COL_ID]) != 4 or len(line[COL_NAME]) < 2:
        continue

    name = "COMM_CMD_"+line[COL_NAME].upper()
    enum += "    "+name+" = "+line[COL_ID]+","
    if len(line[COL_DESC]):
        enum += "    /* "+line[COL_DESC]+" */\n"
    else:
        enum += "\n"
enum += '} comm_cmd_id_t;'

with open(TARGET) as f:
    data = f.read()

search_pattern = re.compile(REGEX, flags=re.MULTILINE)
new_data = search_pattern.sub(enum, data)

if args.dry:
    print(new_data)
    exit(0)
else:
    with open(TARGET, "w") as f:
        print("Writing to", TARGET)
        f.write(new_data)
