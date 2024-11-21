import pwd
import sys

val = sys.argv
print(val)

# Replace with the UID you want to look up
uid = 1000

# Get the username associated with the UID
user_info = pwd.getpwuid(uid)

# Print the username
print(user_info.pw_name)
