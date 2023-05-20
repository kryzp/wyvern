import os

def parse_file(path, fname):
    ll = []
    newll = []
    with open(path) as inf:
        ll = inf.read().split("\n")
    capitalized_name = fname.split(".")[0].upper() + "_" + fname.split(".")[1].upper()
    for l in ll:

        if l.startswith("#ifndef " + capitalized_name):
            newll.append("#ifndef WVN_" + capitalized_name)
        elif l.startswith("#define " + capitalized_name):
            newll.append("#define WVN_" + capitalized_name)
        elif l.startswith("#endif // " + capitalized_name):
            newll.append("#endif // WVN_" + capitalized_name)
        else:
            newll.append(l)
    with open(path, mode='wt') as of:
        of.write('\n'.join(newll))

def loop_dir(dirstr):
    directory = os.fsencode(dirstr)
    for file in os.listdir(directory):
        fn = os.fsdecode(file)
        path = os.path.join(dirstr, fn)
        if path.endswith(".h") or path.endswith("hpp"):
            print(path)
            parse_file(path, fn)
            continue
        elif os.path.isdir(path):
            loop_dir(path)

loop_dir("private/")
loop_dir("public/")
