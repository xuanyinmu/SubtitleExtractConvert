import os
import chardet
import locale

def PathEndWith(path : str, filter_list : list = ['.cpp', '.h']):
    path_format = path[path.rfind('.') :]
    if path_format in filter_list:
        return True
    return False

def GB2312ToUTF8(dir : str, filter_list : list = ['.cpp', '.h']):
    
    file_list = os.listdir(dir)
    for file_path in file_list:
        if PathEndWith(file_path,filter_list):
            full_path = os.path.join(dir,file_path)
            print(full_path)
            
            newData = ''
            with open(file=full_path, mode='r', encoding='gbk') as file:
                for line in file:
                    newData += line
            # newData = newData.encode()
            # encoding_dict = chardet.detect(newData)
            # print(type(newData))
            # print(encoding_dict.get('encoding'))
            # if(encoding_dict.get('encoding') != 'utf-8'):
            with open(file = full_path, mode='w', encoding='utf-8') as file:
                file.write(newData)

def main(dir : str):
    print('系统编码为：', locale.getpreferredencoding())
    GB2312ToUTF8(dir)
    print('所有文件编码转换完成')

main(r'D:\Code\VsCode\SubtitleExtractConvert\SubtitleExtractConvert\tmp')