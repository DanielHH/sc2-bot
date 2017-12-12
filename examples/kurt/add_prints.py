import fileinput

name = None
ln = 0

for line in fileinput.input():
    ln += 1
    if name is None:
        name = line.replace('#include', '').replace('"', '').replace('.h', '').strip()
    if len(line) > 0 and line[0] == ' ':
        print('std::cout << "' + name + ': ' + str(ln) + '" << std::endl;')
    print(line, end = '')
