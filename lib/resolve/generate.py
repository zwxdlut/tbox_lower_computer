#! /usr/bin/env python3
# -*- coding: UTF-8 -*-

import xlwings as xw
import sys
import os
import time
import math
import binascii
import collections

space1 = 64
space2 = 80
space3 = 74

upstream_map = collections.OrderedDict()
downstream_map = collections.OrderedDict()

def main():
    wb = xw.Book.caller()
    dir = os.path.dirname(__file__)

    generate_can_matrix_h(f"{dir}/gen_can_matrix.h", wb)
    generate_upstream_h(f"{dir}/gen_upstream.h", wb)
    generate_downstream_h(f"{dir}/gen_downstream.h", wb)
    generate_resolve_h(f"{dir}/gen_resolve.h", wb)
    generate_resolve_c(f"{dir}/gen_resolve.c", wb)

def generate_can_matrix_h(path, wb):
    global upstream_map

    print(f"generate {path}...")

    with open(path, "w") as file:
        generate_file_description(file)
        generate_file_header(file)
        generate_seperator(file, "Definitions")

        sheet = wb.sheets["introduction"]

        content = "#define" + " " * (space1 - len("#define")) + sheet["F13"].value
        file.write(content + " " * (space2 - len(content)) +  f"/**< {sheet['F12'].value} */\n")

        content = "#define" + " " * (space1 - len("#define")) + sheet["G13"].value
        file.write(content + " " * (space2 - len(content)) +  f"/**< {sheet['G12'].value} */\n")

        byte_order = sheet["G13"].value
        names = sheet["H13:H15"].value
        diff_ids = set()

        for name in names:
            if not name:
                break;
            
            file.write(f"\n/**\n * @name {name} messages." + "\n * @{\n */\n")

            ids = []
            i = 6
            sheet = wb.sheets[name]
            row = sheet.range((i, 3), (i, 9)).value

            while None != row[0]:
                if None != row[1] and None != row[2]:
                    diff = row[2] not in diff_ids

                    if diff:
                        file.write(f"/* Message {row[1]} */\n")

                        content = f"#define MSG_ID_{row[1]}"
                        file.write(content + " " * (space1 - len(content)) + f"{row[2]}\n")

                        ids.append(row[2])
                        diff_ids.add(row[2])

                        msg_id = content.lstrip("#define ")
                        upstream_map[msg_id] = []

                if diff and None != row[4] and None != row[5]:
                    if None != row[3]:
                        signal = f"SIGNAL_{row[3]}"
                    elif None != row[1]:
                        signal = f"MSG_{row[1]}"

                    start_bit = int(row[4])
                    bit_len = int(row[5])

                    if "INTEL" == byte_order:
                        start_bit = 64 - start_bit - bit_len
                    elif "MOTOROLA" == byte_order:
                        start_bit = 8 * int(start_bit / 8) + 8 - (start_bit % 8) - bit_len

                    content = f"#define {signal}_START_BIT"
                    file.write(content + " " * (space1 - len(content)) + f"{start_bit}\n")

                    content = f"#define {signal}_BIT_LEN"
                    file.write(content + " " * (space1 - len(content)) + f"{bit_len}\n")

                    if None != row[6]:
                        upstream_map[msg_id].append([signal, row[6]])

                i += 1
                row = sheet.range((i, 3), (i, 9)).value

            content = f"#define {name}_MSG_ID_NUM" 
            file.write("\n" + content + " " * (space1 - len(content)) + f"{len(ids)}\n")

            content = ""
            for id in ids:
                content += f" {id},"
            file.write("\n" + f"#define {name}_MSG_ID_LIST{content.rstrip(',')}\n")

            file.write("/** @} */ " + f"/* {name} messages. */\n")

        generate_file_tail(file)

def generate_upstream_h(path, wb):
    print(f"generate {path}...")

    with open(path, "w") as file:
        generate_file_description(file)
        generate_file_header(file)
        file.write("\n#include <stdint.h>\n")
        generate_seperator(file, "Definitions")

        name = ""
        ivalue = 0
        ivalues = ""
        start_bit = 0
        temp_bit_len = 0
        i = 6
        sheet = wb.sheets["upstream"]
        row = sheet.range((i, 3), (i, 11)).value

        while None != row[0]:
            if None != row[1] and None != row[2]:
                if "" != name:
                    content = f"#define {name}_DATA_LEN"
                    file.write(content + " " * (space1 - len(content)) + f"{int(start_bit / 8)}\n")

                name = row[1]
                id = row[2]

                file.write(f"/**\n * @name {row[1]}." + "\n * @{\n */\n")

                content = f"#define {name}_ID"
                content += " " * (space1 - len(content)) + f"{id}"
                file.write(content + " " * (space3 - len(content)) + f"/**< {name} */\n")

            if None != row[3] and None != row[4] and None != row[5] and None != row[6] and None != row[7]:
                content = f"#define {row[3]}_START_BIT"
                content += " " * (space1 - len(content)) + f"{start_bit}"
                file.write(content + " " * (space3 - len(content)) + f"/**< {row[8]}. */\n")

                bit_len = int(row[4])
                content = f"#define {row[3]}_BIT_LEN"
                file.write(content + " " * (space1 - len(content)) + f"{bit_len}\n")

                minv = row[6]
                if float == type(minv):
                    minv = int(minv)
                content = f"#define {row[3]}_MIN"
                file.write(content + " " * (space1 - len(content)) + f"{minv}\n")

                maxv = row[7]
                if float == type(maxv):
                    maxv = int(maxv)
                content = f"#define {row[3]}_MAX"
                file.write(content + " " * (space1 - len(content)) + f"{maxv}\n")

                temp_bit_len += bit_len
                if str == type(row[5]):
                    ivalue += int(row[5], 16)
                else:
                    ivalue += int(row[5])
                if 0 == temp_bit_len % 8:
                    temp = "%X" %(ivalue)
                    n = 2 * temp_bit_len / 8 - len(temp)
                    if n:
                        temp = '0' * int(n) + temp
                    while len(temp):
                        ivalues += f"0x{temp[0:2].upper()},"
                        temp = temp[2:]
                    ivalue = 0
                    temp_bit_len = 0

                start_bit += bit_len
            i += 1
            row = sheet.range((i, 3), (i, 11)).value

        ivalues = ivalues.rstrip(',')

        content = f"#define {name}_DATA_LEN"
        file.write(content + " " * (space1 - len(content)) + f"{int(start_bit / 8)}\n")

        file.write(f"#define {name}_INIT_VALUES {ivalues}\n")

        file.write("/* Data on UART as follow */ \n")
        start_bit = "%04X" %(int(start_bit / 8 + 2))
        start_bit = byteswap(start_bit).upper()
        start_bit = start_bit[0:2] + " " + start_bit[2:]
        id = id[2:]
        id = byteswap(id).upper()
        id = id[0:2] + " " + id[2:]
        ivalues = ivalues[2:]
        ivalues = ivalues.replace(",0x", " ")
        file.write(f"/* AA 55 {start_bit} {id} {ivalues} */\n")

        file.write("/** @} */" + f" /* {name}. */\n")

        generate_file_tail(file)

def generate_downstream_h(path, wb):
    global downstream_map

    print(f"generate {path}...")

    with open(path, "w") as file:
        generate_file_description(file)
        generate_file_header(file)
        generate_seperator(file, "Definitions")

        i = 6
        sheet = wb.sheets["downstream"]
        row = sheet.range((i, 3), (i, 11)).value

        while None != row[0]:
            if None != row[1]:
                file.write(f"/* {row[1]} */\n")

                if None != row[2]:
                    content = f"#define {row[1]}_ID"
                    file.write(content + " " * (space1 - len(content)) + f"{row[2]}\n")

                    name = content.lstrip("#define ")
                    downstream_map[name] = []

                if None != row[3]:
                    content = f"#define {row[1]}_LEN"
                    file.write(content + " " * (space1 - len(content)) + f"{int(row[3])}\n")

                    content = content.lstrip("#define ")
                    downstream_map[name].append(content)

            if None != row[4] and None != row[5] and None != row[6]:
                bit_len = int(row[6])
                start_bit = 64 - int(row[5]) - bit_len

                content = f"#define {row[4]}_START_BIT"
                file.write(content + " " * (space1 - len(content)) + f"{start_bit}\n")

                content = f"#define {row[4]}_BIT_LEN"
                file.write(content + " " * (space1 - len(content)) + f"{bit_len}\n")

                if None != row[7] and None != row[8]:
                     downstream_map[name].append([row[4], row[7], row[8]])

            i += 1
            row = sheet.range((i, 3), (i, 11)).value

        generate_file_tail(file)

def generate_resolve_h(path, wb):
    print(f"generate {path}...")

    with open(path, "w") as file:
        generate_file_description(file)
        generate_file_header(file)
        file.write("\n#include <stdint.h>\n")
        generate_extern_C_start(file)
        generate_seperator(file, "Definitions")
        generate_seperator(file, "Function prototypes")
        file.write(
"""/**
 * @brief  Resolve message to upstream.
 *
 * @param  [in] _id   Message ID.
 * @param  [in] _data Message data.
 * @param  [in] _len  Message data length.
 */
void gen_resolve_msg_to_upstream(const uint32_t _id, const uint8_t *const _data, const uint8_t _len);

/**
 * @brief  Resolve downstream to message.
 *
 * @param  [in] _id   Downstream ID.
 * @param  [in] _data Downstream data.
 * @param  [in] _len  Downstream data length.
 */
void gen_resolve_downstream_to_msg(const uint16_t _id, const uint8_t *const _data, const uint16_t _len);

/**
 * @name Callback functions.
 * @{
 */
/**
 * @brief Resolve message to upstream callback function.
 *
 * @param [in] _signal_start_bit Message signal start bit.
 * @param [in] _signal_bit_len   Message signal item bit length.
 * @param [in] _data             Message data.
 * @param [in] _len              Messge  data length.
 * @param [in] _from_list        Covert from table list.
 * @param [in] _to_list          Covert to table list.
 * @param [in] _list_num         Table list number.
 * @param [in] _resolution       Covert resolution.
 * @param [in] _offset           Covert offset.
 * @param [in] _item_start_bit   Upstream item start bit.
 * @param [in] _item_bit_len     Upstream item bit length.
 */
void gen_resolve_msg_to_upstream_callback(const uint8_t _signal_start_bit,
                                          const uint8_t _signal_bit_len,
                                          const uint8_t *const _data,
                                          const uint8_t _len,
                                          const uint32_t *const _from_list,
                                          const uint32_t *const _to_list,
                                          const uint8_t _list_num,
                                          const float _resolution,
                                          const int32_t _offset,
                                          const uint16_t _item_start_bit,
                                          const uint8_t _item_bit_len);

/**
 * @brief Resolve downstream to message callback function.
 *
 * @param _item_start_bit   Downstream item start bit.
 * @param _item_bit_len     Downstream item bit length.
 * @param _data             Downstream data.
 * @param _len              Downstream data length.
 * @param _from_list        Covert from table list.
 * @param _to_list          Covert to table list.
 * @param _list_num         Table list number.
 * @param _resolution       Covert resolution.
 * @param _offset           Covert offset.
 * @param _id               Message ID.
 * @param _signal_start_bit Message signal start bit.
 * @param _signal_bit_len   Message signal bit length.
 */
void gen_resolve_downstream_to_msg_callback(const uint8_t _item_start_bit,
                                            const uint8_t _item_bit_len,
                                            const uint8_t *const _data,
                                            const uint16_t _len,
                                            const uint32_t *const _from_list,
                                            const uint32_t *const _to_list,
                                            const uint8_t _list_num,
                                            const float _resolution,
                                            const int32_t _offset,
                                            const uint32_t _id,
                                            const uint8_t _signal_start_bit,
                                            const uint8_t _signal_bit_len);
/** @} */ /* Callback functions. */\n""")

        generate_extern_C_end(file)
        generate_file_tail(file)

def generate_resolve_c(path, wb):
    print(f"generate {path}...")

    with open(path, "w") as file:
        generate_file_description(file)
        file.write("""
#include \"gen_resolve.h\"\n
#include <string.h>\n
#include \"gen_upstream.h\"
#include \"gen_downstream.h\"
#include \"gen_can_matrix.h\"\n""")
        generate_seperator(file, "Definitions")

        sheet = wb.sheets["resolve"]

        for j in (3, 10):
            if 3 == j:
                title = "Upstream"
            else:
                title = "Downstream"
                file.write("\n")

            file.write(f"/**\n * @name {title} convert table.\n * @" + "{\n */\n")

            lfrom = ""
            lto = ""
            num = 0
            i = 6
            row = sheet.range((i, j), (i, j + 5)).value

            while None != row[0]:
                if None != row[1]:
                    name = row[1]
                    if None != row[4]:
                        resolution = row[4]
                        if not resolution % 1:
                            resolution = int(resolution)
                    else:
                        resolution = 1

                    if None != row[5]:
                        offset = int(row[5])
                    else:
                        offset = 0

                if None != row[2]:
                    lfrom += f"{row[2]}, "
                    num += 1

                if None != row[3]:
                    lto += f"{row[3]}, "

                if None == row[1] and None == row[2] and None == row[3]:
                    if "" != lfrom:
                        file.write(f"static const uint32_t {name}_FROM_LIST[] = " + "{" + lfrom.rstrip(", ") + "};\n")
                    else:
                        file.write(f"#define {name}_FROM_LIST NULL\n")

                    if "" != lto:
                        file.write(f"static const uint32_t {name}_TO_LIST[] = " + "{" + lto.rstrip(", ") + "};\n")
                    else:
                        file.write(f"#define {name}_TO_LIST NULL\n")

                    file.write(f"#define {name}_LIST_NUM {num}\n")
                    file.write(f"#define {name}_RESOLUTION {resolution}\n")
                    file.write(f"#define {name}_OFFSET {offset}\n\n")

                    lfrom = ""
                    lto = ""
                    num = 0

                i += 1
                row = sheet.range((i, j), (i, j + 5)).value

            file.write("/** @} */" + f" /* {title} convert table. */\n")

        generate_seperator(file, "Functions")

        file.write(
"""void gen_resolve_msg_to_upstream(const uint32_t _id, const uint8_t *const _data, const uint8_t _len)
{
    switch(_id)
    {""")
        for key in upstream_map:
            file.write(f"""
        case {key}:""")
            for value in upstream_map[key]:
                file.write(f"""
            gen_resolve_msg_to_upstream_callback({value[0]}_START_BIT, {value[0]}_BIT_LEN, _data, _len, {value[1]}_FROM_LIST, {value[1]}_TO_LIST, {value[1]}_LIST_NUM, {value[1]}_RESOLUTION, {value[1]}_OFFSET, {value[1]}_START_BIT, {value[1]}_BIT_LEN);""")
            file.write("""
            break;""")
        file.write("""
        default:
            break;""")
        file.write("""
    }
}\n""")

        file.write("""
void gen_resolve_downstream_to_msg(const uint16_t _id, const uint8_t *const _data, const uint16_t _len)
{
    switch(_id)
    {""")
        for key in downstream_map:
            file.write(f"""
        case {key}:""")
            it = iter(downstream_map[key])
            try:
                value = next(it)
                file.write(f"""
            if({value} != _len) return;""")
                while True:
                    value = next(it)
                    file.write(f"""
            gen_resolve_downstream_to_msg_callback({value[0]}_START_BIT, {value[0]}_BIT_LEN, _data, _len, {value[0]}_FROM_LIST, {value[0]}_TO_LIST, {value[0]}_LIST_NUM, {value[0]}_RESOLUTION, {value[0]}_OFFSET, MSG_ID_{value[1]}, SIGNAL_{value[2]}_START_BIT, SIGNAL_{value[2]}_BIT_LEN);""")               
            except StopIteration:
                file.write("""
            break;""")
        file.write("""
        default:
            break;""")
        file.write("""
    }
}\n""")

        file.write("""
__attribute__((weak)) void gen_resolve_msg_to_upstream_callback(const uint8_t _signal_start_bit,
                                                                const uint8_t _signal_bit_len,
                                                                const uint8_t *const _data,
                                                                const uint8_t _len,
                                                                const uint32_t *const _from_list,
                                                                const uint32_t *const _to_list,
                                                                const uint8_t _list_num,
                                                                const float _resolution,
                                                                const int32_t _offset,
                                                                const uint16_t _item_start_bit,
                                                                const uint8_t _item_bit_len)
{ 
}

__attribute__((weak)) void gen_resolve_downstream_to_msg_callback(const uint8_t _item_start_bit,
                                                                  const uint8_t _item_bit_len,
                                                                  const uint8_t *const _data,
                                                                  const uint16_t _len,
                                                                  const uint32_t *const _from_list,
                                                                  const uint32_t *const _to_list,
                                                                  const uint8_t _list_num,
                                                                  const float _resolution,
                                                                  const int32_t _offset,
                                                                  const uint32_t _id,
                                                                  const uint8_t _signal_start_bit,
                                                                  const uint8_t _signal_bit_len)
{ 
}\n""")
        generate_seperator(file, "Local Functions")

def generate_file_description(file):
        name = os.path.basename(file.name)

        file.write(
f"""/**
 * @file   {name}
 * @author Auto generated
 * @note   Created on {time.strftime("%Y/%m/%d", time.localtime())}
 */\n""")

def generate_file_header(file):
        name = os.path.basename(file.name)

        file.write(f"""
#ifndef __{name.replace('.', '_').upper()}__
#define __{name.replace('.', '_').upper()}__\n""")

def generate_file_tail(file):
        file.write(f"\n#endif /* __{os.path.basename(file.name).replace('.', '_').upper()}__ */")

def generate_seperator(file, name):
        file.write(f"""
/******************************************************************************
 * {name}
 ******************************************************************************/\n""")

def generate_extern_C_start(file):
    file.write("\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n")

def generate_extern_C_end(file):
    file.write("\n#ifdef __cplusplus\n}\n#endif\n")
    
def byteswap(data):
    return bytes.decode(binascii.hexlify(binascii.unhexlify(data)[::-1]))

@xw.func
def hello(name):
    return f"Hello {name}!"

if __name__ == "__main__":
    if 2 > len(sys.argv):
        xw.Book("auto_gen_yema.xlsm").set_mock_caller()
    else:
        xw.Book(sys.argv[1]).set_mock_caller()

    main()
