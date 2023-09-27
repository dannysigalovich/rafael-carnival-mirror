import socket
import struct
import threading
from time import sleep

# Define UDP server address and port
UDP_SERVER_ADDRESS = '192.168.1.10'
UDP_SERVER_PORT = 7

pkt_list = None
pkt_log = None
sock_first_lock = threading.Lock()
sock_second_lock = threading.Lock()

def recv_pkt(sock):
    
    global pkt_list
    sock_first_lock.acquire()
    pkt, server_address = sock.recvfrom(4096)
    pkt_list = pkt.decode()
    sock_first_lock.release()

    global pkt_log
    sock_second_lock.acquire()
    pkt, server_address = sock.recvfrom(4096)
    pkt_log = pkt.decode()
    sock_second_lock.release()

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
        exit()
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
