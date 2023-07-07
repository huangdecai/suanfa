
import netifaces
import hashlib
def get_mac_addresses():
    mac_addresses = []
    try:
        interfaces = netifaces.interfaces()
        for interface in interfaces:
            addresses = netifaces.ifaddresses(interface)
            if netifaces.AF_LINK in addresses:
                mac_address = addresses[netifaces.AF_LINK][0]['addr']
                mac_addresses.append(mac_address)
    except Exception as e:
        print("获取 MAC 地址时出现错误:", e)
    return mac_addresses



if __name__ == '__main__':
    mac_addresses = get_mac_addresses()
    if mac_addresses:
        #print("本机的 MAC 地址:")
        for mac_address in mac_addresses:
            md5_hash = hashlib.md5(mac_address.encode())
            encrypted_code = md5_hash.hexdigest().upper()  # 将加密结果转换为大写形式
            print(mac_address.encode(),encrypted_code)
            break
    else:
        print("无法获取 MAC 地址")
    input("复制上面的机器码给我...")

