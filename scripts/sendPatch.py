from ftplib import FTP
import os
import sys

titleId = "01003D200BAA2000"

def listdirs(connection,_path):
    file_list, dirs, nondirs = [], [], []
    try:
        connection.cwd(_path)
    except:
        return []

    connection.retrlines('LIST', lambda x: file_list.append(x.split()))
    for info in file_list:
        ls_type, name = info[0], info[-1]
        if ls_type.startswith('d'):
            dirs.append(name)
        else:
            nondirs.append(name)
    return dirs


def ensuredirectory(connection,root,path):
    print(f"Ensuring {os.path.join(root, path)} exists...")
    if path not in listdirs(connection, root):
        connection.mkd(f'{root}/{path}')


consoleIP = sys.argv[1]
if '.' not in consoleIP:
    print(sys.argv[0], "ERROR: Please specify with `IP=[Your console's IP]`")
    sys.exit(-1)

consolePort = 5000

if len(sys.argv) < 3:
    version = '102'
else:
    version = sys.argv[2]

curDir = os.curdir

ftp = FTP()
print(f'Connecting to {consoleIP}... ', end='')
ftp.connect(consoleIP, consolePort)
ftp.login(user=sys.argv[3], passwd=sys.argv[4])
print('Connected!')

patchDirectories = []

root, dirs, _ = next(os.walk(curDir))
for dir in dirs:
    if dir.startswith("starlight_patch_"):
        patchDirectories.append((os.path.join(root, dir), dir))

ensuredirectory(ftp, '', 'atmosphere')
ensuredirectory(ftp, '/atmosphere', 'exefs_patches')

for patchDir in patchDirectories:
    dirPath = patchDir[0]
    dirName = patchDir[1]
    ensuredirectory(ftp, '/atmosphere/exefs_patches', patchDir[1])
    _, _, files = next(os.walk(dirPath))
    for file in files:
        fullPath = os.path.join(dirPath, file)
        if os.path.exists(fullPath):
            sdPath = f'/atmosphere/exefs_patches/{dirName}/{file}'
            print(f'Sending {sdPath}')
            ftp.storbinary(f'STOR {sdPath}', open(fullPath, 'rb'))

ensuredirectory(ftp, '/atmosphere', 'titles')
ensuredirectory(ftp, '/atmosphere/contents', titleId)
ensuredirectory(ftp, f'/atmosphere/contents/{titleId}', 'exefs')

binaryPath = f'{os.path.basename(os.getcwd())}{version}.nso'
if os.path.isfile(binaryPath):
    sdPath = f'/atmosphere/contents/{titleId}/exefs/subsdk1'
    print(f'Sending {sdPath}')
    ftp.storbinary(f'STOR {sdPath}', open(binaryPath, 'rb'))

# Custom files
ensuredirectory(ftp, f'/atmosphere/contents/{titleId}', 'romfs')
ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs', 'Data')
ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs/Data', 'StreamingAssets')
ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets', 'custom_data')

custom_data_path = "data/custom_data"
if os.path.exists(custom_data_path):
    _, _, files = next(os.walk(custom_data_path))
    for file in files:
        fullPath = os.path.join(custom_data_path, file)
        if os.path.exists(fullPath):
            sdPath = f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/custom_data/{file}'
            print(f'Sending {sdPath}')
            ftp.storbinary(f'STOR {sdPath}', open(fullPath, 'rb'))

ab_path = "data/ab"
if os.path.exists(ab_path):
    _, _, files = next(os.walk(ab_path))
    for file in files:
        fullPath = os.path.join(ab_path, file)
        if os.path.exists(fullPath):
            sdPath = f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/ab/{file}'
            print(f'Sending {sdPath}')
            ftp.storbinary(f'STOR {sdPath}', open(fullPath, 'rb'))

if os.path.exists('data/native_data/'):
    ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/native_data/script', 'event')
    ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/native_data/script/event', 'usual')
    ensuredirectory(ftp, f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/native_data/script/event/usual', 'talk_action_gr')
    sdPath = f'/atmosphere/contents/{titleId}/romfs/Data/StreamingAssets/native_data/script/event/usual/talk_action_gr/talk_gr@m02_010.lua'
    ftp.storbinary(f'STOR {sdPath}', open('data/native_data/script/event/usual/talk_action_gr/talk_gr@m02_010.lua', 'rb'))

