import os, pwd, sys, grp

PROC_DIR = "/proc"


with open('dummy.txt', 'w') as details:
    ps_infos = []
    for pid in os.listdir(PROC_DIR):
        if pid.isdigit():
            # print(pid)
            status_file = os.path.join(PROC_DIR, pid, 'status')
            with open(status_file, 'r') as f:
                lines = f.readlines()
                process_info = {}
                for line in lines:
                    if line.startswith('Pid'):
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
                        process_info['CommandName'] = line.split()[1]
                ps_infos.append(process_info)

    details.write(str(ps_infos))
print(ps_infos[:4])



