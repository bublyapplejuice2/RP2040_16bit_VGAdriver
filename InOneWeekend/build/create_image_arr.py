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
        f.write(f"const unsigned short arr[{n}] = {{ ")
        for j in range(300):
            for i in range(400):
                r = i / 399
                g = j / 299
                b = (i+j) / 698

                ir = int(r * 31.999)
                ig = int(g * 63.999)
                ib = int(b * 31.999)

                color = (ib << 11) | (ig << 5) | ir
                if i == 399:
                    color = 0
                f.write(f"{color}, ")
        f.write(f"}};\n")
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