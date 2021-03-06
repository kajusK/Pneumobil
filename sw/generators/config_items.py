#!/usr/bin/python
# -*- coding: utf-8 -*-

#    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.
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
Generate config_items.[ch] context from config.ods file
"""
import argparse
import json
import re
from pyexcel_odsr import get_data

SOURCE = "../../doc/specs/config.ods"
BASEDIR = "../../fw/sources/app/"
COL_NAME = 0
COL_TYPE = 1
COL_DEFAULT = 2
COL_DESC = 3

desc = "Generate config_items.[ch] from %s" % (SOURCE)
parser = argparse.ArgumentParser(description=desc)
parser.add_argument('-d, --dry-run', dest="dry", action='store_true',
        help="Don't modify the file, just print to console")
parser.add_argument('-m, --module', dest="module",
        help="Module name to generate files for (e.g. ECU, HMI)")
args = parser.parse_args()

def parse_module(data, module):
    # Parse sheet from the table into dictionary
    sheet = module.upper()+"_config"
    items = []

    for line in data[sheet][1:]:
        if len(line) < 2:
            continue
        if len(line) != 4:
            print("Incorrect line length, some required options are missing, skipping\n"+str(line))
            continue

        items.append({ "type" : line[COL_TYPE].strip().upper(),
                        "name" : line[COL_NAME],
                        "defval" : line[COL_DEFAULT],
                        "desc" : line[COL_DESC]})

    return items

def update_file(module, search, replace, header = True):
    # Update the selected module with generated data
    ending = 'h' if header else 'c'
    target = BASEDIR+module.lower()+"/config_items."+ending
    with open(target) as f:
        data = f.read()

    for i in range(len(search)):
        search_pattern = re.compile(search[i], flags=re.MULTILINE)
        data = search_pattern.sub(replace[i], data)

    if args.dry:
        print(data)
        return
    else:
        with open(target, "w") as f:
            print("Writing to", target)
            f.write(data)

def gen_gcc(items, module):
    uintempty = True
    boolempty = True
    floatempty = True

    # generate required data for selected module
    enum_uint = "/** Config items list - autogenerated by sw/generators/config_items.py */\n"
    enum_uint += "typedef enum {\n"
    enum_bool = enum_uint
    enum_float = enum_uint
    defuint = "/** Default uint values - autogenerated by sw/generators/config_items.py */\n"
    defuint += "const uint32_t config_default_uint[CONFIG_UINT_COUNT] = {\n"
    defbool = "/** Default bool values - autogenerated by sw/generators/config_items.py */\n"
    defbool += "const uint8_t config_default_bool[CONFIG_BOOL_COUNT/8+1] = {\n"
    deffloat = "/** Default float values - autogenerated by sw/generators/config_items.py */\n"
    deffloat += "const float config_default_float[CONFIG_FLOAT_COUNT] = {\n"
    boolvalues = 0
    boolcount = 0

    for i in items:
        name = "CONFIG_"+i['type']+"_"+i['name'].upper()
        enumline = "    "+name+", /* "+i['desc']+" */\n"
        if i['type'] == "UINT":
            uintempty = False
            enum_uint += enumline
            defuint += "    "+str(int(i['defval']))+", /* "+name+" */\n"
        elif i['type'] == "FLOAT":
            floatempty = False
            enum_float += enumline
            deffloat += "    "+str(float(i['defval']))+", /* "+name+" */\n"
        elif i['type'] == "BOOL":
            boolempty = False
            enum_bool += enumline
            boolvalues = (boolvalues >> 1) | ((int(i['defval']) & 0x01) << 7)
            boolcount += 1
            if boolcount == 8:
                boolcount = 0
                defbool += "    0x%x,\n" % boolvalues
        else:
            print("Unknown type ", i['type'])

    if boolcount != 0:
        defbool += "    0x%02x,\n" % (boolvalues >> (8-boolcount))

    if uintempty:
        enum_uint += "    CONFIG_UINT_DUMMY,\n"
        defuint += "    0, /* DUMMY */\n"
    if floatempty:
        enum_float += "    CONFIG_FLOAT_DUMMY,\n"
        deffloat += "    0, /* DUMMY */\n"
    if boolempty:
        enum_bool += "    CONFIG_BOOL_DUMMY,\n"
        defbool += "    0, /* DUMMY */\n"

    enum_uint += "    CONFIG_UINT_COUNT,\n"
    enum_uint += "} config_item_uint_t;"
    defuint += "};"

    enum_float += "    CONFIG_FLOAT_COUNT,\n"
    enum_float += "} config_item_float_t;"
    deffloat += "};"

    enum_bool += "    CONFIG_BOOL_COUNT,\n"
    enum_bool += "} config_item_bool_t;"
    defbool += "};"

    #TODO insert dummy values when list is empty

    regex1 = '(\/\*.*$\n)*typedef enum {[^}]*} config_item_uint_t;'
    regex2 = '(\/\*.*$\n)*typedef enum {[^}]*} config_item_bool_t;'
    regex3 = '(\/\*.*$\n)*typedef enum {[^}]*} config_item_float_t;'
    update_file(module, [regex1, regex2, regex3], [enum_uint, enum_bool, enum_float])

    regex1 = '(\/\*.*$\n)*const .* config_default_uint\[.* {[^}]*};'
    regex2 = '(\/\*.*$\n)*const .* config_default_bool\[.* {[^}]*};'
    regex3 = '(\/\*.*$\n)*const .* config_default_float\[.* {[^}]*};'
    update_file(module, [regex1, regex2, regex3], [defuint, defbool, deffloat], False);

def gen_python(items):
    # generate code for python
    pass

if args.module is not None:
    modules = [args.module]
else:
    modules = ["ECU", "HMI"]

data = get_data(SOURCE);
for m in modules:
    items = parse_module(data, m)
    gen_gcc(items, m)


