from tkinter import *
from tkinter import ttk
from tkinter import messagebox
import numpy as np
import serial
import os
from serial.tools.list_ports import comports
import tk_tools

available_ports = []
data = ''
TxBuffer_length = 150
ReadBlock = 0

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
   global RMS, Ampl, data
   if(ReadBlock == 0):
      if(ser.in_waiting > 0):
         serialString = ser.readline().decode('utf-8')
         if(data):
            ser.write(bytearray(data,'ascii'))
            print(data)
            data = ''
         #print(serialString)
         RMS = serialString.split('/')[2]
         Ampl = serialString.split('/')[1]
         RMS = float(RMS)
         Ampl = float(Ampl)
         ShowNumbers()
   win.after(50, ReadFromSerial)

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

def Send_Pin():
   global data, ReadBlock, serialCoeff
   ReadBlock = 1
   serialData = ''
   serialCoeff = ''
   Pin = Code.get()
   data = f'25/{Pin}'
   data = data + "*" * (TxBuffer_length-len(data))
   #print(data)
   ser.write(bytearray(data,'ascii'))
   data = ''
   #if(ser.in_waiting > 0):
   serialData = ser.readline().decode('utf-8')
   print(serialData)
   serialCoeff = ser.readline()#.decode('utf-8')
   #print(serialCoeff[4:])
   serialCoeff = serialCoeff[4:].decode('utf-8')
   serialData = serialData.split('/')[0]
   ReadBlock = 0
   if(serialData[-4] == '0'):
      messagebox.showerror(title="Klaida", message="Netinkamas PIN kodas")
   elif(serialData[-4] == '1'):
      #serialData = ser.readline().decode('utf-8')
      print(serialCoeff)
      NewWindow()

def Send_Coefficients():
   global DataToSend, ReadBlock
   ReadBlock = 1
   DataToSend = [0]*16
   for i in range(1, 9):
      DataToSend[i-1] = coeff_RMS[f'coeff_RMS{i}'].get()
      DataToSend[i-1] = round(float(DataToSend[i-1]), 5)
      DataToSend[i-1] = str(DataToSend[i-1])
      DataToSend[i+7] = coeff_Ampl[f'coeff_Ampl{i}'].get()
      DataToSend[i+7] = round(float(DataToSend[i+7]), 5)
      DataToSend[i+7] = str(DataToSend[i+7])
   print(DataToSend)
   DataToSend = "/".join(DataToSend)
   DataToSend = '#/' + DataToSend
   DataToSend = DataToSend + "*" * (TxBuffer_length-len(DataToSend))
   print(DataToSend)
   ser.write(bytearray(DataToSend,'ascii'))
   ReadBlock = 0
   Entry_PIN.delete(0, END)
   win2.destroy()

def NewWindow():
   global win2, ranges, coeff_Ampl, coeff_RMS
   win2 = Toplevel(win)
   win2.title("Koeficientai")
   win2.geometry("400x325")
   y_var = 40
   i = 1
   ranges = ['1-3 mV', '3-10 mV', '10-30 mV', '30-100 mV', '100-300 mV', '300-1000 mV', '1-3 V', '3-10 V']
   coeff_RMS = {var: StringVar() for var in ['coeff_RMS1', 'coeff_RMS2', 'coeff_RMS3', 'coeff_RMS4', 'coeff_RMS5', 'coeff_RMS6', 'coeff_RMS7', 'coeff_RMS8']}
   coeff_Ampl = {var: StringVar() for var in ['coeff_Ampl1', 'coeff_Ampl2', 'coeff_Ampl3', 'coeff_Ampl4', 'coeff_Ampl5', 'coeff_Ampl6', 'coeff_Ampl7', 'coeff_Ampl8']}
   for range in ranges:
      coeff_RMS[f'coeff_RMS{i}'].set(float(serialCoeff.split('/')[i]))
      coeff_Ampl[f'coeff_Ampl{i}'].set(float(serialCoeff.split('/')[i+8]))
      i = i+1
   i = 1
   Label(win2, text ="RMS koeficientai").place(x=10, y=10)
   Label(win2, text ="Amplitudės koeficientai").place(x=210, y=10)
   for range in ranges:
      Label(win2, text =range).place(x=10, y=y_var)
      Entry(win2, textvariable=coeff_RMS[f'coeff_RMS{i}'], justify='center', width=15).place(x=85, y=y_var)
      Label(win2, text =range).place(x=210, y=y_var)
      Entry(win2, textvariable=coeff_Ampl[f'coeff_Ampl{i}'], justify='center', width=15).place(x=285, y=y_var)
      y_var = y_var + 30
      i = i+1
   Button(win2, text='Baigti', command=Send_Coefficients).place(x=200, y=300)
   

for port in comports():
   available_ports.append(str(port))
print(available_ports)

win = Tk()
win.title("Multimeter User Interface")
variable = StringVar(win)
Value_RMS = StringVar(win)
Value_Ampl = StringVar(win)
Code = StringVar(win)
win.geometry("600x400")
variable.set(available_ports[0])
Value_RMS.set('0.00 V')
Value_Ampl.set('0.00 V')
Code.set('')
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
CoefficientsButton = Button(win, text="Keisti koeficientus", command=Send_Pin, width=20)
Entry_PIN = Entry(win, textvariable=Code, justify='center', width=10)
Text_PIN = Label(win, text="PIN kodas:", justify='right')

camera_view = Label(win)
tekstas.place(x=10, y=10)
Pasirinkimas.place(x=110, y=10)
Entry_RMS.place(x=100,y=300)
Entry_Ampl.place(x=390,y=300)
connect_button.place(x=200,y=40)
End_button.place(x=500, y=350)
gauge_RMS.place(x=10, y=100)
gauge_Ampl.place(x=300, y=100)
CoefficientsButton.place(x=160, y=347)
Entry_PIN.place(x=80, y=350)
Text_PIN.place(x=10, y=347)

win.mainloop()