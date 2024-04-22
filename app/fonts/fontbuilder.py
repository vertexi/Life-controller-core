import subprocess
import os
import sys
import re

build_ttf = False

# get current directory
current_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(current_dir)

def get_ttf_files(directory):
    return [file for file in os.listdir(directory) if file.endswith("ttf")]

ttf_files = get_ttf_files(current_dir)
print(ttf_files, '\n')

for ttf_file in ttf_files:
    font_var_name = re.sub(r"-", "_", ttf_file)
    font_var_name = re.sub(r"\.ttf", "", font_var_name)
    font_family_name = font_var_name[:font_var_name.find("_")]
    if os.path.isfile(f"{font_family_name}.cpp"):
        os.remove(f"{font_family_name}.cpp")
    if os.path.isfile(f"{font_family_name}.hpp"):
        os.remove(f"{font_family_name}.hpp")

for ttf_file in ttf_files:
    if build_ttf:
        response = subprocess.run(["hb-subset.exe", \
            "--unicodes=0x20-0xFF", \
            "--unicodes+=0x2000-0x206F", \
            "--unicodes+=0x3000-0x30FF", \
            '--text-file=./character_set/GB2312withSymbols.txt', \
            f"--output-file=../assets/fonts/{ttf_file}", \
            f"--font-file={ttf_file}"],
            stdout=sys.stdout.buffer,
            stderr=sys.stderr.buffer,
            check=True, cwd=current_dir)
    else:
        if os.path.isfile(f"../assets/fonts/{ttf_file}"):
            os.remove(f"../assets/fonts/{ttf_file}")
    response = subprocess.run(["hb-subset.exe", \
        "--unicodes=0x20-0xFF", \
        "--unicodes+=0x2000-0x206F", \
        "--unicodes+=0x3000-0x30FF", \
        '--text-file=./character_set/GB2312withSymbols.txt', \
        "--output-file=temp.font", \
        f"--font-file={ttf_file}"],
        stdout=sys.stdout.buffer,
        stderr=sys.stderr.buffer,
        check=True, cwd=current_dir)
    font_var_name = re.sub(r"-", "_", ttf_file)
    font_var_name = re.sub(r"\.ttf", "", font_var_name)
    font_family_name = font_var_name[:font_var_name.find("_")]
    compress_process = subprocess.run(["binary_to_compressed_c", \
        "temp.font", \
        f"{font_var_name}"],
        check=True,
        cwd=current_dir,
        capture_output=True)
    compress_process.stdout.decode('utf-8')
    with open(f"{font_family_name}.cpp", "a", encoding="utf-8") as f:
        f.write(compress_process.stdout.decode('utf-8'))

cpp_impl_dict = {}

for ttf_file in ttf_files:
    font_var_name = re.sub(r"-", "_", ttf_file)
    font_var_name = re.sub(r"\.ttf", "", font_var_name)
    font_family_name = font_var_name[:font_var_name.find("_")]
    if cpp_impl_dict.get(font_family_name) is None:
        cpp_impl_dict[font_family_name] = {\
            "header": \
f"""
#pragma once

typedef const unsigned int* fontMemory;
""", \
            "impl": \
f"""
#include "{font_family_name}.hpp"
""", \
            "header_file_name": f"{font_family_name}.hpp", \
            "impl_file_name": f"{font_family_name}.cpp"
        }
    cpp_impl_dict[font_family_name]["header"] += \
f"""
fontMemory get_{font_var_name}();
int get_{font_var_name}_size();
"""

    cpp_impl_dict[font_family_name]["impl"] += \
f"""
fontMemory get_{font_var_name}() {{
    return {font_var_name}_compressed_data;
}}
int get_{font_var_name}_size() {{
    return {font_var_name}_compressed_size;
}}
"""

for cpp_impl in cpp_impl_dict:
    with open(cpp_impl_dict[cpp_impl]["header_file_name"], "a", encoding="utf-8") as f:
        f.write(cpp_impl_dict[cpp_impl]["header"])
    with open(cpp_impl_dict[cpp_impl]["impl_file_name"], "a", encoding="utf-8") as f:
        f.write(cpp_impl_dict[cpp_impl]["impl"])

os.remove("temp.font")
