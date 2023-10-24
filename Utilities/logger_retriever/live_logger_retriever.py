import socket
import struct
import datetime

UDP_SERVER_ADDRESS = '192.168.1.10'
UDP_SERVER_PORT = 7

print(f"""
██╗      ██████╗  ██████╗  ██████╗ ███████╗██████╗ 
██║     ██╔═══██╗██╔════╝ ██╔════╝ ██╔════╝██╔══██╗
██║     ██║   ██║██║  ███╗██║  ███╗█████╗  ██████╔╝
██║     ██║   ██║██║   ██║██║   ██║██╔══╝  ██╔══██╗
███████╗╚██████╔╝╚██████╔╝╚██████╔╝███████╗██║  ██║
╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝
                                                   
██████╗ ███████╗████████╗██████╗ ██╗███████╗██╗   ██╗███████╗██████╗ 
██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝██║   ██║██╔════╝██╔══██╗
██████╔╝█████╗     ██║   ██████╔╝██║█████╗  ██║   ██║█████╗  ██████╔╝
██╔══██╗██╔══╝     ██║   ██╔══██╗██║██╔══╝  ╚██╗ ██╔╝██╔══╝  ██╔══██╗
██║  ██║███████╗   ██║   ██║  ██║██║███████╗ ╚████╔╝ ███████╗██║  ██║
╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝
 Trego LTD. Luanch-Computer Info: {UDP_SERVER_ADDRESS}:{UDP_SERVER_PORT}   
 Using live mode, press Ctrl+C to exit.                                       
    """)

def build_live_request():
    # Define the struct format
    struct_format = "5s9s"  # 5 bytes for sync and bytes for the word
    sync = bytes([0xAA, 0xBB, 0xCC, 0xDD, 0xEE])
    word = b"LIVE_LOG"  # Ensure it's bytes, not a string
    request_packet = struct.pack(struct_format, sync, word)
    return request_packet

def build_live_log(data):
    struct_format = f'{4}I {4}B {4}B {4}B {4}B'
    unpacked_data = struct.unpack(struct_format, data)

    # Extract individual fields
    bit_status = list(unpacked_data[:4])
    battery_percentage = list(unpacked_data[4:8])
    is_ready_to_launch = list(unpacked_data[8:12])
    elev_go_up = list(unpacked_data[12:16])
    elev_is_up = list(unpacked_data[16:20])

    return bit_status, battery_percentage, is_ready_to_launch, elev_go_up, elev_is_up

def live():

    # Create a UDP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # Send the message to the server
    client_socket.sendto(build_live_request(), (UDP_SERVER_ADDRESS, UDP_SERVER_PORT))
    
    while True:
        # Receive data from the server
        data, server_address = client_socket.recvfrom(1024)
        if server_address[0] != UDP_SERVER_ADDRESS:
            continue
        bit_status, battery_percentage, is_ready_to_launch, elev_go_up, elev_is_up = build_live_log(data)

        print(datetime.datetime.now())

        for i in range(0, 4):
            print ('Spike ', i + 1, ':')
            print('bit_status: ', bit_status[i])
            print('battery_percentage: ', battery_percentage[i])
            print('is_ready_to_launch: ', is_ready_to_launch[i])
            print('elev_go_up: ', elev_go_up[i])
            print('elev_is_up: ', elev_is_up[i])
            

if __name__ == '__main__':
    live()