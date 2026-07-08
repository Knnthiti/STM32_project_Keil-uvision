import socket

# Set the Server IP address (0.0.0.0 means accept connections from any network interface)
# You can also use your computer's specific IP, like "192.168.3.10"
SERVER_IP = "0.0.0.0"
SERVER_PORT = 5000

print(f"Starting TCP Server on Port {SERVER_PORT}. Waiting for STM32 to connect...")

# 1. Create a TCP Socket
server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Add this setting to allow reusing the port immediately after restarting the server
# (Prevents the "Port already in use" error)
server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the IP and Port
server_sock.bind((SERVER_IP, SERVER_PORT))

# 2. Wait and listen for incoming connections (allow 1 connection in the queue)
server_sock.listen(1)

try:
    while True:
        # 3. Accept the connection when STM32 tries to connect
        client_sock, client_addr = server_sock.accept()
        print(f"\n🎉 Connection successful! STM32 connected from IP: {client_addr}")

        try:
            while True:
                # 4. Wait to receive data from STM32 (read up to 1024 bytes at a time)
                data = client_sock.recv(1024)

                # If the data is empty, it means the STM32 has disconnected
                if not data:
                    print("🛑 STM32 has disconnected.")
                    break

                # 5. Process the received data
                
                # Show the raw bytes (to check exactly what the computer received)
                print(f"🔍 Raw Data: {data}")

                # Convert the raw bytes into a normal string
                # Use errors="replace" so the program does not crash if it receives garbage data
                # (Bad characters will show as '?' instead of causing an error)
                safe_text = data.decode("utf-8", errors="replace")
                
                # Show the final readable message
                print(f"📥 Received Message: {safe_text}")

        except ConnectionResetError:
            # Handle unexpected disconnections
            print("⚠️ Connection was reset by the client.")
            
        finally:
            # Always close the client socket when done
            client_sock.close()
            print("Waiting for a new connection...\n")

except KeyboardInterrupt:
    # Handle the user pressing Ctrl+C to stop the program
    print("\nShutting down the Server.")
    
finally:
    # Always close the main server socket before exiting
    server_sock.close()