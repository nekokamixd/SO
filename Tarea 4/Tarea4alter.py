from threading import Thread
from threading import Lock
from threading import Condition
from threading import Semaphore
import time

lock_fila= Lock()
cv_fila_aJuan = Condition(lock_fila)
cv_fila_aCliente = Condition(lock_fila)
servidos_actual = [0, 0]
cant_bandejas = input("Cantidad de bandejas: ")
cant_clientes = input("Cantidad de clientes: ")
clientes_esperando = Semaphore(int(cant_clientes))
bandejas = []

class bandeja:
    def __init__(self, numero):
        self.comida = False
        self.id = numero

    def hayComida(self):
        return self.comida

    def comer(self):
        self.comida = False

    def llenar(self):
        self.comida = True


    def getId(self):
        return self.id

class cocina:
    def __init__(self):
        self.bandeja = None
    
    def insert_bandeja(self, b):
        self.bandeja = b

    def release_bandeja(self):
        self.bandeja = None

    def get_bandeja(self):
        return self.bandeja

mesa = cocina()

class Cliente(Thread):
    def __init__(self, boletito, bandeja):
        Thread.__init__(self)
        self.boletito = boletito
        self.bandeja = bandeja

    def run(self):
        global lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa
        lock_fila.acquire()
        print("Mi boletito es: "+str(self.boletito))

        while (mesa.get_bandeja() != None):
            cv_fila_aCliente.wait()
        mesa.insert_bandeja(self.bandeja)
        cv_fila_aJuan.notify()

        print("Ya comi uwu" + " Boletito: " + str(self.boletito))
        lock_fila.release()

class Juan(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa, clientes_esperando
        while (True):
            with lock_fila:
                while (mesa.get_bandeja() == None):
                    cv_fila_aJuan.wait()
                time.sleep(3)
                mesa.get_bandeja().llenar()
                mesa.release_bandeja()
                clientes_esperando.release()
                cv_fila_aCliente.notify_all()
                

def main():
    # servidos_actual = [almuerzos servidos, almuerzo que se debe servir]
    # bandejas = Lista de bandejas
    # boletito = Boletito para hacer la fila
    global lock_fila, cv_fila_aJuan, cv_fila_aCliente
    global servidos_actual, cant_bandejas, cant_clientes, bandejas, mesa

    for b in range(int(cant_bandejas)):
        bandejas.append(bandeja(b))

    thread_juan = Juan()
    thread_juan.start()

    boletito = 1
    
    for cliente in range(int(cant_clientes)):
        thread = Cliente(boletito, bandejas.pop(0))
        thread.start()
        boletito += 1
   
if __name__ == '__main__':
    main()