import socket

# ตั้งค่า IP ของคอมพิวเตอร์ (0.0.0.0 คือรับจากทุกวง LAN)
# หรือจะระบุเป็น "192.168.3.10" (IP ของคอมคุณ) ก็ได้
SERVER_IP = "0.0.0.0"
SERVER_PORT = 5000

print(f"กำลังเปิด TCP Server ที่พอร์ต {SERVER_PORT} รอ STM32 มาเชื่อมต่อ...")

# 1. สร้าง Socket และ Bind พอร์ต
server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# วางคำสั่งนี้เพื่อให้สามารถเปิดปิด Server ซ้ำได้โดยไม่ติด Error "Port already in use"
server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_sock.bind((SERVER_IP, SERVER_PORT))

# 2. สั่งให้รอฟังการเชื่อมต่อ (Listen)
server_sock.listen(1)

try:
    while True:
        # 3. ยอมรับการเชื่อมต่อเมื่อ STM32 วิ่งเข้ามา (Accept)
        client_sock, client_addr = server_sock.accept()
        print(f"\n🎉 จับมือสำเร็จ! STM32 เชื่อมต่อมาจาก IP: {client_addr}")

        try:
            while True:
                # 4. รอรับข้อมูลจาก STM32 (ครั้งละไม่เกิน 1024 ไบต์)
                # 4. รอรับข้อมูลจาก STM32
                data = client_sock.recv(1024)

                if not data:
                    print("🛑 STM32 ตัดการเชื่อมต่อแล้ว")
                    break

                # ------ แก้ไขส่วนนี้ ------
                # 1. โชว์ข้อมูลดิบ (ดูว่าคอมพิวเตอร์ได้รับ Byte อะไรมาบ้าง)
                print(f"🔍 ข้อมูลดิบ (Raw): {data}")

                # 2. แปลงเป็นข้อความ (ใช้ errors='replace' เพื่อป้องกันโปรแกรมแครช)
                # ตัวอักษรไหนที่พัง มันจะถูกเปลี่ยนเป็นเครื่องหมาย  อัตโนมัติ
                safe_text = data.decode("utf-8", errors="replace")
                print(f"📥 ได้รับข้อความ: {safe_text}")
                # -----------------------

        except ConnectionResetError:
            print("⚠️ การเชื่อมต่อถูกรีเซ็ต")
        finally:
            client_sock.close()
            print("รอการเชื่อมต่อครั้งใหม่...\n")

except KeyboardInterrupt:
    print("\nปิด Server")
finally:
    server_sock.close()
