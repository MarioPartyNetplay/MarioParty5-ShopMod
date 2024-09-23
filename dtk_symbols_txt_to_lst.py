import re

def parse_input_file(input_file, output_file):
    pattern = re.compile(r'(\S+)\s*=\s*(?:\S+:)?(0x[0-9A-Fa-f]+);')

    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            match = pattern.match(line)
            if match:
                label, address = match.groups()
                outfile.write(f'{address[2:]}:{label}\n')

input_file = 'mp5_symbols.txt'
output_file = 'include/mp5.us.lst'
parse_input_file(input_file, output_file)
