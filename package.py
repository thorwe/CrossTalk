import os
import shutil
import zlib
import zipfile

archs = ['32', '64']

BUILD_PATH_PRE = "build-CrossTalk-Desktop_Qt_5_12_8_MSVC2017_"
BUILD_PATH_POST = "bit_using_2017_build_chain-RelWithDebInfo"

def copy_file(src, dest):
    try:
        shutil.copy(src, dest)
    # eg. src and dest are the same file
    except shutil.Error as e:
        print('Error: %s' % e)
    # eg. source or destination doesn't exist
    except IOError as e:
        print('Error: %s' % e.strerror)


def compress(file_names, arch):
    print("File Paths:")
    print(file_names)

    path = "./"

    # Select the compression mode ZIP_DEFLATED for compression
    # or zipfile.ZIP_STORED to just store the file
    compression = zipfile.ZIP_DEFLATED

    # create the zip file first parameter path/name, second mode
    zf = zipfile.ZipFile("crosstalk_win" + arch + ".ts3_plugin", mode="w")
    try:
        for file_name in file_names:
            # Add file to the zip file
            # first parameter file to zip, second filename in zip
            zf.write(path + file_name,
                     file_names[file_name], compress_type=compression)

    except OSError:
        print("An error occurred")
    finally:
        # Don't forget to close the file!
        zf.close()


def updatePackageIni(packageIniPath, build_path, arch):
    copy_file(packageIniPath, build_path)
    f = open(os.path.join(build_path, "package.ini"), "a")
    try:
        f.write('Platforms = win{}\r\n'.format(arch))
    except OSError:
        print("An error occurred")
    finally:
        # Don't forget to close the file!
        f.close()


for arch in archs:
    build_path = os.path.join("..", BUILD_PATH_PRE + arch + BUILD_PATH_POST)
    updatePackageIni(os.path.join("res", "package.ini"), build_path, arch)
    file_names = {
        os.path.join(build_path, "package.ini"): "package.ini",
        os.path.join("res", "ct_16x16.png"): os.path.join("plugins", "crosstalk_plugin", "ct_16x16.png"),
        os.path.join("res", "radar_16.png"): os.path.join("plugins", "crosstalk_plugin", "radar_16.png"),
        os.path.join(build_path, "CrossTalk.dll"): os.path.join("plugins", "crosstalk_win" + arch + ".dll")
    }
    compress(file_names, arch)
