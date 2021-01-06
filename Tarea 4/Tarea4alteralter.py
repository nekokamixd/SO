import threading
from threading import Thread
from threading import Semaphore
import time
from datetime import datetime

cant_bandejas = input("Cantidad de bandejas: ")
cant_clientes = input("Cantidad de clientes: ")
bandejas = [] # Lista de bandejas

# Semaforo para que solo una persona use el recurso
s_usarBandejeroFila = Semaphore(1)
s_usarBandejero = Semaphore(1)
s_usarMesa = Semaphore(1)
s_JuanSirviendo = Semaphore(0)

# Semaforo con la cantidad de bandejas que hay en cada bandejero
s_cantidadBandejasFila = Semaphore(int(cant_bandejas))
s_cantidadBandejasBandejero = Semaphore(0)

class bandeja:
    def __init__(self):
        self.comida = False

    def hayComida(self):
        return self.comida

    def comer(self):
        self.comida = False

    def llenar(self):
        self.comida = True

class cocina:
    def __init__(self):
        self.bandeja = None
        self.cliente = None
    
    def insert_bandeja(self, b, c):
        self.bandeja = b
        self.cliente = c

    def release_bandeja(self):
        self.bandeja = None

    def get_bandeja(self):
        return self.bandeja
    
    def get_idCliente(self):
        return self.cliente

mesa = cocina()

class Cliente(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_usarBandejeroFila, s_usarBandejero
        global s_cantidadBandejasFila, s_cantidadBandejasBandejero
        global cant_bandejas, cant_clientes, bandejas, mesa
        id_cliente = threading.get_ident()

        # SACAR BANDEJAS

        s_usarBandejeroFila.acquire()
        # Escribir acceso a zona critica + time
        bandeja = bandejas.pop(0)
        now = datetime.now()
        print("Hora: "+now.strftime("%H:%M:%S")+" Cliente: "+str(id_cliente)+": Saqué una bandeja")
        s_cantidadBandejasFila.acquire()
        s_usarBandejeroFila.release()

        # BUSCAR ALMUERZO

        s_usarMesa.acquire()
        now = datetime.now()
        print("Hora: "+now.strftime("%H:%M:%S")+" Cliente: "+str(id_cliente)+": Hola me sirve almuerzo")
        # Escribir acceso a zona critica + time
        mesa.insert_bandeja(bandeja, id_cliente)
        s_JuanSirviendo.acquire()
        s_usarMesa.release()
        time.sleep(5) # Cliente come su almuerzo
        now = datetime.now()
        print("Hora: "+now.strftime("%H:%M:%S")+" Cliente: "+str(id_cliente)+": Estuvo rico")
        s_cantidadBandejasBandejero.release()

        # AYUDAR AL SIGUIENTE

        # DEJAR BANDEJA

class Juan(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_usarBandejeroFila, s_usarBandejero
        global s_cantidadBandejasFila, s_cantidadBandejasBandejero
        global cant_bandejas, cant_clientes, bandejas, mesa
        global f_juan

        while (True):
            # RELLENAR BANDEJEROS
        
            # SERVIR ALMUERZO
            
            if (mesa.get_bandeja() != None):
                cliente_servir = mesa.get_idCliente()
                time.sleep(3) # Juan sirve almuerzo
                mesa.get_bandeja().llenar()
                s_JuanSirviendo.release()
                now = datetime.now()
                print("Hora: "+now.strftime("%H:%M:%S")+" Juan: Ahi le servi su almuerzo a "+str(cliente_servir))
                mesa.release_bandeja()
                s_usarMesa.release()

def main(): 
    global cant_bandejas, cant_clientes, bandejas, mesa

    for b in range(int(cant_bandejas)):
        bandejas.append(bandeja())

    thread_juan = Juan()
    thread_juan.start()
    
    for cliente in range(int(cant_clientes)):
        thread = Cliente()
        thread.start()
   
if __name__ == '__main__':
    main()