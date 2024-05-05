import socket
import threading

def client_thread(host, port, message):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(message.encode())
        data = s.recv(1024)
        print('Received', repr(data))

threads = []
for i in range(10):  # Creates 10 concurrent clients
    t = threading.Thread(target=client_thread, args=('127.0.0.1', 8605, f'Hello from client {i}'))
    threads.append(t)
    t.start()

for t in threads:
    t.join()
