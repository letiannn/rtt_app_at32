import sys
import os

def add_ff_to_file(filename, ff_count=32):
    """
    在文件末尾添加指定数量的0xFF字节
    
    Args:
        filename (str): 文件名
        ff_count (int): 要添加的0xFF字节数量，默认为32
    """
    try:
        # 检查文件是否存在
        if not os.path.exists(filename):
            print(f"Error: File '{filename}' does not exist")
            return False
            
        # 获取文件原始大小
        original_size = os.path.getsize(filename)
        
        # 以二进制追加模式打开文件
        with open(filename, 'ab') as f:
            # 创建指定数量的0xFF字节并写入
            ff_bytes = b'\xff' * ff_count
            f.write(ff_bytes)
        
        # 获取文件新大小
        new_size = os.path.getsize(filename)
        
        print(f"[OK] Successfully processed: {filename}")
        print(f"  Original size: {original_size:,} bytes")
        print(f"  Added {ff_count} bytes of 0xFF")
        print(f"  New size: {new_size:,} bytes")
        print(f"  Last {ff_count} bytes are now 0xFF")
        
        return True
        
    except PermissionError:
        print(f"Error: No write permission for file '{filename}'")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False

def main():
    """
    主函数：处理命令行参数
    """
    # 设置输出编码为UTF-8，避免Windows编码问题
    if sys.platform.startswith('win'):
        import io
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
    
    # 检查命令行参数
    if len(sys.argv) < 2:
        print("Usage: python add_ff.py <filename> [byte_count]")
        print("Examples:")
        print("  python add_ff.py firmware.bin        # Add 32 bytes of 0xFF")
        print("  python add_ff.py firmware.bin 64     # Add 64 bytes of 0xFF")
        print("  python add_ff.py firmware.bin 0      # Show file info only")
        return
    
    # 获取文件名
    filename = sys.argv[1]
    
    # 获取字节数（可选参数）
    ff_count = 32  # 默认值
    
    if len(sys.argv) >= 3:
        try:
            ff_count = int(sys.argv[2])
            if ff_count < 0:
                print("Error: Byte count cannot be negative")
                return
        except ValueError:
            print("Error: Byte count must be an integer")
            return
    
    # 如果字节数为0，只显示文件信息
    if ff_count == 0:
        if os.path.exists(filename):
            file_size = os.path.getsize(filename)
            print(f"File info: {filename}")
            print(f"Size: {file_size:,} bytes")
        else:
            print(f"Error: File '{filename}' does not exist")
        return
    
    # 执行添加操作
    add_ff_to_file(filename, ff_count)

if __name__ == "__main__":
    main()