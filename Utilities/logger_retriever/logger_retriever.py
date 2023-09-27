import socket
import struct
import threading
from time import sleep
import signal

# Define UDP server address and port
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
    """)


pkt_list = None
pkt_log = None
is_running = True
sock_first_lock = threading.Lock()
sock_second_lock = threading.Lock()

def smart_recv(sock, pkt_kind):
    sock.settimeout(0.5)
    pkt = None
    while pkt is None and is_running:
        try:
            pkt, server_address = sock.recvfrom(4096)
            if pkt_kind == 'list':
                global pkt_list
                pkt_list = pkt.decode()
            elif pkt_kind == 'log':
                global pkt_log
                pkt_log = pkt.decode()
            return
        except socket.timeout:
            if not is_running:
                exit(0)
            continue

def recv_pkt(sock):
    sock_first_lock.acquire()
    smart_recv(sock, 'list')
    sock_first_lock.release()
    sock_second_lock.acquire()
    smart_recv(sock, 'log')
    sock_second_lock.release()
    

# Define a signal handler function
def signal_handler(sig, frame):
    global is_running
    is_running = False
    print("\nCtrl+C pressed. Exiting...")
    # cleanup operations
    if sock_first_lock.locked():
        sock_first_lock.release()
    if sock_second_lock.locked():
        sock_second_lock.release()
    exit(0)

# Register the signal handler for SIGINT (Ctrl+C)
signal.signal(signal.SIGINT, signal_handler)

# Create a UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock_first_lock.acquire() # acquire the lock for the thread to use the socket later

# start the receive thread
threading.Thread(target=recv_pkt, args=(udp_socket,)).start()

# Step 1: Send a request packet

# Define the struct format
struct_format = "5s128s"  # 5 bytes for sync and bytes for the word

# Create the struct with values
sync = bytes([0xAA, 0xBB, 0xCC, 0xDD, 0xEE])
word = b"REQUEST_LIST_LOGS"  # Ensure it's bytes, not a string

# Pack the struct into bytes
request_packet = struct.pack(struct_format, sync, word)
udp_socket.sendto(request_packet, (UDP_SERVER_ADDRESS, UDP_SERVER_PORT))
sock_first_lock.release() 
sock_second_lock.acquire() # acquire the lock for the thread to use the socket later

# Step 2: Retrieve available log files
# Receive the available logs

while pkt_list is None:
    sleep(0.5)
available_logs = pkt_list.split('\n')


print("Available logs:")
for i, log in enumerate(available_logs):
    if log == '' and i == 0:
        print("No logs available")
        exit(1)
    if log == '':
        continue
    print(f"{i + 1}. {log}")

# Step 3: Wait for user input to select a log
selected_log_index = int(input("Enter the number of the log you want to retrieve: ")) - 1

selected_log_name = available_logs[selected_log_index]

# Step 4: Send the selected log request
# Define the struct format
struct_format = "5s21s"  # 5 bytes for sync and 21 bytes for the word
selected_log_request = b"REQUEST_LOG" + selected_log_index.to_bytes(4, 'little')
selected_log_request_pkt = struct.pack(struct_format, sync, selected_log_request)

udp_socket.sendto(selected_log_request_pkt, (UDP_SERVER_ADDRESS, UDP_SERVER_PORT))
sock_second_lock.release() # release the lock for the thread to use the socket later

# Step 5: Wait for the selected log
while pkt_log is None:
    sleep(0.5)
selected_log_content = pkt_log.encode()

# Step 6: Save the selected log as a file
log_file_name = f"{selected_log_name}"
with open(log_file_name, 'w') as log_file:
    log_file.write(selected_log_content.decode())

print(f"Log '{selected_log_name}' saved as '{log_file_name}'")

# Close the UDP socket
udp_socket.close()
