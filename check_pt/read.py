import serial
import time

# XBee setting
serdev = '/dev/ttyUSB1'
s = serial.Serial(serdev, 9600)

s.write("+++".encode())
char = s.read(2)
print("Enter AT mode.")
print(char.decode())

s.write("ATMY 0x415\r\n".encode())
char = s.read(3)
print("Set MY 41.")
print(char.decode())

s.write("ATDL 0x405\r\n".encode())
char = s.read(3)
print("Set DL 40.")
print(char.decode())

s.write("ATID 0x12\r\n".encode())
char = s.read(3)
print("Set PAN ID 12.")
print(char.decode())

s.write("ATWR\r\n".encode())
char = s.read(3)
print("Write config.")
print(char.decode())

s.write("ATMY\r\n".encode())
char = s.read(4)
print("MY :")
print(char.decode())

s.write("ATDL\r\n".encode())
char = s.read(4)
print("DL : ")
print(char.decode())

s.write("ATCN\r\n".encode())
char = s.read(3)
print("Exit AT mode.")
print(char.decode())

print("start sending RPC")
i = 0
#s.write("1\r\n".encode())
while True:
    # send RPC to remote
    s.write("/Acc/run\r".encode())
    print(i)
    i = i + 1
    time.sleep(1)
    line=s.readline()
   
    print(line)
    
s.close()