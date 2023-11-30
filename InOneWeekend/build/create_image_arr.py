import os

# Initialize an empty list to store the lines
numbers = []

FILENAME = "out.ppm"

# Open the file and read line by line
with open(FILENAME, 'r', encoding='utf-16') as file:
    for line in file:
        # You can use line.strip() to remove newline characters
        stripped_line = line.strip()
        try:
            # Convert to a float or int as needed
            number = int(stripped_line)
            # If successful, append the number to the list
            numbers.append(number)
        except ValueError:
            # If the line is not a number, just continue to the next line
            continue

# Now 'lines' contains all the lines from the file
n = len(numbers)

def write_values():
    with open('const_arr.cpp', 'w') as f:
        f.write("#ifndef CONST_ARR_H\n")
        f.write("#define CONST_ARR_H\n")
        f.write(f"volatile unsigned short vga_data_array[{n}] = {{ ")
        for i in range(n - 1):
            f.write(f"{numbers[i]}, ")
        f.write(f"{numbers[-1]} }};\n")
        f.write("#endif")

def write_const():
    with open('const_arr.cpp', 'w') as f:
        f.write("#ifndef CONST_ARR_H\n")
        f.write("#define CONST_ARR_H\n")
        f.write(f"unsigned short vga_data_array[{n}] = {{ ")
        for i in range(n - 1):
            f.write(f"BLUE, ")
        f.write(f"BLUE }};\n")
        f.write("#endif")

if __name__ == "__main__":
    write_values()