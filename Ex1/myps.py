import os, pwd, sys, grp

PROC_DIR = "/proc"
FIELDS= ['Pid', 'PPid', 'Uid', 'Uname', 'Gid', 'Gname', 'State', 'Name']

def get_pcb(raw):
    process_info = {}
    for line in raw:
        if line.startswith('Pid:'):
            process_info['Pid'] = line.split()[1]
        elif line.startswith('PPid:'):
            process_info['PPid'] = line.split()[1]
        elif line.startswith('Uid:'):
            process_info['Uid'] = line.split()[1]
            uname = pwd.getpwuid(int(process_info['Uid'])).pw_name
            process_info['Uname'] = uname
        elif line.startswith('Gid:'):
            process_info['Gid'] = line.split()[1]
            gname = grp.getgrgid(int(process_info['Gid'])).gr_name
            process_info['Gname'] = gname
        elif line.startswith('State:'):
            process_info['State'] = line.split()[1]
        elif line.startswith('Name'):
            process_info['Name'] = line.split()[1]
    return process_info

def pcb_to_entry(pcb):
    return '\t'.join([pcb[field] for field in FIELDS])


def main():
    if not (len(sys.argv) == 1 or (len(sys.argv) == 2 and sys.argv[1] == '-a')):
        raise Exception("Invalid argument. The script should be run with no arguments or with '-a'.")

    argv = sys.argv
    curr_uid = os.getuid()
    
        
    col = list(FIELDS)
    col[-1] = 'CommandName'
    col_str = '\t'.join(col)
    print(col_str)
    
    for pid in os.listdir(PROC_DIR):
        if pid.isdigit():
            status_file = os.path.join(PROC_DIR, pid, 'status')
            with open(status_file, 'r') as f:
                lines = f.readlines()
                process_info = get_pcb(lines)
                if len(argv) == 2 or curr_uid == int(process_info['Uid']):
                    print(pcb_to_entry(process_info))

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(e)
