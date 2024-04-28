from tkinter import*
from tkinter import ttk
from tkinter.messagebox import askquestion, showinfo
import numpy as np
import serial
import os
from serial.tools.list_ports import comports
import tk_tools

available_ports = []

def Baigti():
   win.destroy()
   ser.close()

def on_enter_end(e):
   End_button.config(background='red', foreground= "white")

def on_enter_connect(e):
   connect_button.config(background='green', foreground= "white")

def on_leave(e):
   End_button.config(background= 'SystemButtonFace', foreground= 'black')
   connect_button.config(background= 'SystemButtonFace', foreground= 'black')

def ReadFromSerial():
   global RMS, Ampl
   if(ser.in_waiting > 0):
      serialString = ser.readline().decode('utf-8')
      RMS = serialString.split('/')[2]
      Ampl = serialString.split('/')[1]
      print(serialString)
      RMS = float(RMS)
      Ampl = float(Ampl)
      ShowNumbers()
      print(Ampl, 'c', RMS)
   win.after(100, ReadFromSerial)

def ConnectToSerial():
   global ser
   Com_port = variable.get().split()[0]
   print(Com_port)
   ser = serial.Serial(port=str(Com_port),baudrate=115200,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=8,timeout=2)
   print("Connected to: " + ser.portstr)
   ReadFromSerial()

def ShowNumbers():
   global gauge_RMS, gauge_Ampl
   if(RMS>=1000.00):
      gauge_RMS = tk_tools.Gauge(win, max_value=15.00, unit='V', bg='SystemButtonFace', label='RMS', yellow=100, red=100, height=200, width=290)
      gauge_RMS.place(x=10, y=100)
      gauge_RMS.set_value(round(RMS/1000.00, 2))
      Value_RMS.set(f'{RMS/1000} V')
   elif(RMS<1.00):
      gauge_RMS = tk_tools.Gauge(win, max_value=999.99, unit='μV', bg='SystemButtonFace', label='RMS', yellow=100, red=100, height=200, width=290)
      gauge_RMS.place(x=10, y=100)
      gauge_RMS.set_value(round(RMS*1000, 2))
      Value_RMS.set(f'{RMS*1000} μV')
   elif(RMS<1000.00):
      gauge_RMS = tk_tools.Gauge(win, max_value=999.99, unit='mV', bg='SystemButtonFace', label='RMS', yellow=100, red=100, height=200, width=290)
      gauge_RMS.place(x=10, y=100)
      gauge_RMS.set_value(round(RMS, 2))
      Value_RMS.set(f'{RMS} mV')
   if(Ampl>=1000.00):
      gauge_Ampl = tk_tools.Gauge(win, max_value=15.00, unit='V', bg='SystemButtonFace', label='Amplitudė', yellow=100, red=100, height=200, width=290)
      gauge_Ampl.place(x=300, y=100)
      gauge_Ampl.set_value(round(Ampl/1000.00, 2))
      Value_Ampl.set(f'{Ampl/1000} V')
   elif(Ampl<1.00):
      gauge_Ampl = tk_tools.Gauge(win, max_value=999.99, unit='μV', bg='SystemButtonFace', label='Amplitudė', yellow=100, red=100, height=200, width=290)
      gauge_Ampl.place(x=300, y=100)
      gauge_Ampl.set_value(round(Ampl*1000, 2))
      Value_Ampl.set(f'{Ampl*1000} μV')
   elif(Ampl<1000.00):
      gauge_Ampl = tk_tools.Gauge(win, max_value=999.99, unit='mV', bg='SystemButtonFace', label='Amplitudė', yellow=100, red=100, height=200, width=290)
      gauge_Ampl.place(x=300, y=100)
      gauge_Ampl.set_value(round(Ampl, 2))
      Value_Ampl.set(f'{Ampl} mV')

for port in comports():
   available_ports.append(str(port))
print(available_ports)

win = Tk()
win.title("Multimeter User Interface")
variable = StringVar(win)
Value_RMS = StringVar(win)
Value_Ampl = StringVar(win)
win.geometry("600x400")
variable.set(available_ports[0])
Value_RMS.set('0.00 V')
Value_Ampl.set('0.00 V')
tekstas = Label(win, text="Pasirinkti portą:")
connect_button = Button(win, text="Prisijungti", command=ConnectToSerial, width=20, activebackground='green')
connect_button.bind('<Enter>', on_enter_connect)
connect_button.bind('<Leave>', on_leave)
Pasirinkimas = ttk.Combobox(win, textvariable=variable, values = available_ports, width=55)
End_button = Button(win, text="Baigti", command=Baigti, width=10)
End_button.bind('<Enter>', on_enter_end)
End_button.bind('<Leave>', on_leave)
Entry_RMS = Entry(win, textvariable=Value_RMS, justify='center')
Entry_Ampl = Entry(win, textvariable=Value_Ampl, justify='center')
gauge_RMS = tk_tools.Gauge(win, max_value=999.99, unit='mV', bg='SystemButtonFace', label='RMS', yellow=100, red=100, height=200, width=290)
gauge_Ampl = tk_tools.Gauge(win, max_value=999.99, unit='mV', bg='SystemButtonFace', label='Amplitudė', yellow=100, red=100, height=200, width=290)

camera_view = Label(win)
tekstas.place(x=10, y=10)
Pasirinkimas.place(x=110, y=10)
Entry_RMS.place(x=100,y=300)
Entry_Ampl.place(x=390,y=300)
connect_button.place(x=200,y=40)
End_button.place(x=500, y=350)
gauge_RMS.place(x=10, y=100)
gauge_Ampl.place(x=300, y=100)

win.mainloop()