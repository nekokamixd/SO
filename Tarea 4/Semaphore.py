from threading import Thread
from threading import Lock
from threading import Condition
from threading import Semaphore
import time

class bandeja:
    def __init__(self, numero):
        self.comida = False
        self.id = numero

    def comer(self):
        self.comida = False

    def llenar(self):
        self.comida = True

    def getId(self):
        return self.id

class Cliente(Thread):
    def __init__(self, boletito, bandeja, lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa):
        Thread.__init__(self)
        self.boletito = boletito
        self.bandeja = bandeja
        self.lock_fila = lock_fila
        self.cv_fila_aCliente = cv_fila_aCliente
        self.cv_fila_aJuan = cv_fila_aJuan
        self.mesa = mesa

    def run(self):
        
        with self.lock_fila:
            print("Mi boletito es: "+str(self.boletito))
            while (self.mesa.get_bandeja() != None):
                self.cv_fila_aCliente.wait()
            self.mesa.insert_bandeja(self.bandeja)
            self.cv_fila_aJuan.notify()
            print("Ya comi uwu" + " Boletito: " + str(self.boletito))

def main():
    cant_bandejas = input("Cantidad de bandejas: ")
    cant_clientes = input("Cantidad de clientes: ")

    bandejas = Semaphore(30)
    bandejero = Semaphore(int(cant_bandejas)/2)

    fila_clientes = Semaphore(int(cant_clientes))

    tc1 = Cliente(boletito, bandeja, lock_fila, cv_fila_aCliente, cv_fila_aJuan, mesa)
   
if __name__ == '__main__':
    main()