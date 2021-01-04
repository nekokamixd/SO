from threading import Thread
from threading import Lock
from threading import Condition
import time

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

class mesa:
    def __init__(self):
        self.bandeja = None
    
    def insert_bandeja(self, b):
        self.bandeja = b

    def release_bandeja(self):
        self.bandeja = None

    def get_bandeja(self):
        return self.bandeja

class Cliente(Thread):
    def __init__(self, boletito, bandeja):
        Thread.__init__(self)
        self.boletito = boletito
        self.bandeja = bandeja

    def run(self):
        global lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa
        with self.lock_fila:
            print("Mi boletito es: "+str(self.boletito))

            while (mesa.get_bandeja() != None):
                cv_fila_aCliente.wait()
            mesa.insert_bandeja(self.bandeja)
            cv_fila_aJuan.notify()

            while (not(mesa.get_bandeja().hayComida())):
                cv_fila_aCliente.wait()
            mesa.release_bandeja()
            cv_fila_aJuan.notify()
            print("Ya comi uwu" + " Boletito: " + str(self.boletito))

class Juan(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa
        while (True):
            with lock_fila:
                while (mesa.get_bandeja() == None):
                    cv_fila_aJuan.wait()
                time.sleep(3)
                mesa.get_bandeja().llenar()
                cv_fila_aCliente.notify()
                while (mesa.get_bandeja() != None):
                    cv_fila_aJuan.wait()

def main():
    lock_fila= Lock()
    cv_fila_aJuan = Condition(lock_fila)
    cv_fila_aCliente = Condition(lock_fila)

    servidos_actual = [0, 0]

    cant_bandejas = input("Cantidad de bandejas: ")
    cant_clientes = input("Cantidad de clientes: ")

    bandejaId = 0
    bandejas = []
    for b in range(int(cant_bandejas)):
        bandejas.append(bandeja(b))

    cocina = mesa()

    boletito = 1
    
    thread_juan = Juan()
    thread_juan.start()

    for cliente in range(int(cant_clientes)):
        thread = Cliente(boletito, bandejas.pop(0))
        thread.start()
        boletito += 1
   
if __name__ == '__main__':
    main()