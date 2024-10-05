import subprocess
import sys
import os
import re

def compile_to_asm(filename):
    # Ensure the file has a .c extension
    if not filename.endswith(".c"):
        print("Error: Please provide a valid C file with a .c extension.")
        return
    
    # Extract the base name (without extension) for output file
    base_name = os.path.splitext(filename)[0]
    
    # Construct the command
    if sys.platform != "win32":
        command = [
            "wine",
            "compiler/powerpc-gcc/bin/powerpc-eabi-gcc.exe",
            "-mcpu=powerpc",         # Target PowerPC
            "-S",                    # Generate assembly output
            "-fno-asynchronous-unwind-tables",  # Remove unnecessary unwind tables
            "-fno-ident",            # Remove .ident metadata
            "-fno-common",           # Remove common symbols
            "-O0",                   # Optimize with level 2 optimization
            "-fno-optimize-sibling-calls",  # Disable tail call optimization
            filename,                # Input file
            "-o", f"{base_name}.s"   # Output assembly file
        ]
    else:
        command = [
            "compiler/powerpc-gcc/bin/powerpc-eabi-gcc",
            "-mcpu=powerpc",         # Target PowerPC
            "-S",                    # Generate assembly output
            "-fno-asynchronous-unwind-tables",  # Remove unnecessary unwind tables
            "-fno-ident",            # Remove .ident metadata
            "-fno-common",           # Remove common symbols
            "-O0",                   # Optimize with level 2 optimization
            "-fno-optimize-sibling-calls",  # Disable tail call optimization
            filename,                # Input file
            "-o", f"{base_name}.s"   # Output assembly file
        ]
    try:
        # Run the compilation command
        subprocess.run(command, check=True)
        print(f"Compiled {filename} to {base_name}.s successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error during compilation: {e}")

def parse_lst_file(lst_file_path):
    codewrite_lst = []
    output_filename = os.path.splitext(lst_file_path)[0] + '.codewrite.lst'

    try:
        with open(lst_file_path, 'r') as lst_file:
            for line in lst_file:
                # Split the line into address and symbol name
                line = line.strip()
                if not line or ':' not in line:
                    continue
                address, symbol = line.split(':')
                
                # Skip symbols that start with '@'
                if symbol.startswith('@'):
                    continue

                # Format the symbol in `.set` format
                formatted_line = f".set {symbol},0x{address.upper()}"
                codewrite_lst.append(formatted_line)
        
        # Write the formatted output to a .codewrite.lst file
        with open(output_filename, 'w') as output_file:
            output_file.write('\n'.join(codewrite_lst) + '\n')
        
        print(f"Parsed {lst_file_path} and wrote to {output_filename} successfully.")
    
    except FileNotFoundError:
        print(f"Error: File {lst_file_path} not found.")
    except Exception as e:
        print(f"Error while parsing {lst_file_path}: {e}")

    return output_filename

def append_codewrite_to_asm(asm_filename, codewrite_lst_filename):
    try:
        # Read the contents of the .codewrite.lst file
        with open(codewrite_lst_filename, 'r') as codewrite_file:
            codewrite_content = codewrite_file.read()

        # Read the contents of the assembly (.s) file
        with open(asm_filename, 'r') as asm_file:
            asm_content = asm_file.read()

        # Combine the .codewrite.lst content at the top of the .s file content
        combined_content = codewrite_content + "\n" + asm_content

        # Write the combined content back to the .s file
        with open(asm_filename, 'w') as asm_file:
            asm_file.write(combined_content)

        print(f"Appended {codewrite_lst_filename} to the top of {asm_filename}.")
    
    except FileNotFoundError as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"Error while appending {codewrite_lst_filename} to {asm_filename}: {e}")

def remove_gnu_attribute(asm_filename):
    try:
        with open(asm_filename, 'r') as asm_file:
            lines = asm_file.readlines()

        # Remove any line containing `.gnu_attribute`
        filtered_lines = [line for line in lines if '.gnu_attribute' not in line]

        # Write the filtered content back to the assembly file
        with open(asm_filename, 'w') as asm_file:
            asm_file.writelines(filtered_lines)

        print(f"Removed '.gnu_attribute' from {asm_filename}.")
    
    except FileNotFoundError as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"Error while removing '.gnu_attribute' from {asm_filename}: {e}")

def replace_bl_calls(asm_filename):
    try:
        with open(asm_filename, 'r') as asm_file:
            content = asm_file.read()

        # Replace instances of `bl func` with the new assembly structure
        def replacement(match):
            func_name = match.group(1)
            return (f"mr r0, r13\n"
                    f"	lis r13, {func_name}@ha\n"
                    f"	addi r13, r13, {func_name}@l\n"
                    f"	mtctr r13\n"
                    f"	mr r13, r0\n"
                    f"	bctrl\n")

        # Updated regex to match `bl` followed by exactly one space and then the function name
        new_content = re.sub(r'\bbl (\w+)', replacement, content)

        # Write the modified content back to the assembly file
        with open(asm_filename, 'w') as asm_file:
            asm_file.write(new_content)

        print(f"Replaced 'bl' calls in {asm_filename}.")
    
    except FileNotFoundError as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"Error while replacing 'bl' calls in {asm_filename}: {e}")

if __name__ == "__main__":
    # Check if a file was passed as an argument
    if len(sys.argv) < 2:
        print("Usage: python c_to_codewrite.py <filename.c> [<filename.lst>]")
        sys.exit(1)
    
    # Get the filename from the command-line argument
    filename = sys.argv[1]
    
    # Compile the file to assembly
    compile_to_asm(filename)

    # If a .lst file is passed as an argument, process it
    if len(sys.argv) == 3:
        lst_file_path = sys.argv[2]
        if lst_file_path.endswith(".lst"):
            # Parse the .lst file and get the path to the generated .codewrite.lst file
            codewrite_lst_filename = parse_lst_file(lst_file_path)
            
            # Append the .codewrite.lst content to the top of the generated .s file
            asm_filename = os.path.splitext(filename)[0] + ".s"
            append_codewrite_to_asm(asm_filename, codewrite_lst_filename)
        else:
            print("Error: The second argument must be a .lst file.")
    
    # Remove .gnu_attribute from the generated .s file
    asm_filename = os.path.splitext(filename)[0] + ".s"
    remove_gnu_attribute(asm_filename)

    # Replace `bl` calls in the generated .s file
    replace_bl_calls(asm_filename)
