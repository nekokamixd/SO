import threading
from threading import Thread
from threading import Semaphore
import time
from datetime import datetime

cant_bandejas = input("Cantidad de bandejas: ")
cant_clientes = input("Cantidad de clientes: ")
bandejas = [] # Lista de bandejas
existeClienteAyudando = 0

# Semaforo para que solo una persona use el recurso
s_usarBandejeroFila = Semaphore(1)
s_usarBandejero = Semaphore(1)
s_usarMesa = Semaphore(1)
s_JuanSirviendo = Semaphore(0)
s_clienteAyudando = Semaphore(1)

# Semaforo con la cantidad de bandejas que hay en cada bandejero
s_cantidadBandejasFila = Semaphore(int(cant_bandejas))
s_cantidadBandejasBandejero = Semaphore(0)

"""
Nombre: mesaAuxiliar
Parametros: ninguno
Descripcion: Simula al cliente que ayuda a otro cliente a comer, siendo la mesa auxiliar sus manos que mantienen la bandeja.
"""
class mesaAuxiliar:
    def __init__(self):
        self.bandeja = None
        self.cliente = None
    
    def sostener_bandeja(self, b, c):
        self.bandeja = b
        self.cliente = c

    def release_bandeja(self):
        self.bandeja = None

    def get_bandeja(self):
        return self.bandeja
    
    def get_idCliente(self):
        return self.cliente

"""
Nombre: bandeja
Parametros: ninguno
Descripcion: Bandeja en donde se pondrá el almuerzo.
"""
class bandeja:
    def __init__(self):
        self.comida = False

    def hayComida(self):
        return self.comida

    def comer(self):
        self.comida = False

    def llenar(self):
        self.comida = True

"""
Nombre: cocina
Parametros: ninguno
Descripcion: Cocina es donde se dejará la bandeja para que Juan sirva el almuerzo.
"""
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

"""
Nombre: Cliente
Parametros: ninguno
Descripcion: Cliente que quiere almorzar.
"""
class Cliente(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_usarBandejeroFila, s_cantidadBandejasFila, s_cantidadBandejasBandejero
        global bandejas, mesa, existeClienteAyudando
        id_cliente = threading.get_ident()

        # SACAR BANDEJAS

        s_usarBandejeroFila.acquire()
        now = datetime.now()
        f_clientes = open("clientes.txt", "a")
        f_clientes.write("Cliente " + str(id_cliente) + " sacando bandeja en la fila, hora: " + now.strftime("%H:%M:%S") + "\n")
        f_clientes.close()
        bandeja = bandejas.pop(0)
        s_cantidadBandejasFila.acquire()
        s_usarBandejeroFila.release()

        # BUSCAR ALMUERZO

        s_usarMesa.acquire()
        now = datetime.now()
        f_clientes = open("clientes.txt", "a")
        f_clientes.write("Cliente " + str(id_cliente) + " esperando a que le Juan le sirva almuerzo, hora: " + now.strftime("%H:%M:%S") + "\n")
        f_clientes.close()
        mesa.insert_bandeja(bandeja, id_cliente)
        s_JuanSirviendo.acquire()
        s_usarMesa.release()

        # COMER ALMUERZO
        #if ():
        #else:

        time.sleep(5) # Cliente come su almuerzo
        s_cantidadBandejasBandejero.release()
        now = datetime.now()
        f_clientes = open("clientes.txt", "a")
        f_clientes.write("Cliente " + str(id_cliente) + " termino de comer, hora: " + now.strftime("%H:%M:%S") + "\n")
        f_clientes.close()

        # AYUDAR AL SIGUIENTE

        # DEJAR BANDEJA

"""
Nombre: Cliente
Parametros: ninguno
Descripcion: Juan, el encargado de servir almuerzos.
"""
class Juan(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_JuanSirviendo
        global mesa
        sirviendo_almuerzos = True

        while (sirviendo_almuerzos):
        
            # SERVIR ALMUERZO
            
            if (mesa.get_bandeja() != None):
                cliente_servir = mesa.get_idCliente()
                f_juan = open("juan.txt", "a")
                now = datetime.now()
                f_juan.write("Sirviendo al cliente " + str(cliente_servir) + ", hora: " + now.strftime("%H:%M:%S") + "\n")
                f_juan.close()
                time.sleep(3) # Juan sirve almuerzo
                mesa.get_bandeja().llenar()
                s_JuanSirviendo.release()
                mesa.release_bandeja()

            # RELLENAR BANDEJEROS

def main(): 
    global cant_bandejas, cant_clientes, bandejas, mesa

    for b in range(int(cant_bandejas)):
        bandejas.append(bandeja())

    f_Juan = open("juan.txt", "w")
    f_Clientes = open("clientes.txt", "w")
    f_Juan.close()
    f_Clientes.close()

    thread_juan = Juan()
    thread_juan.start()
    
    for cliente in range(int(cant_clientes)):
        thread = Cliente()
        thread.start()
   
if __name__ == '__main__':
    main()