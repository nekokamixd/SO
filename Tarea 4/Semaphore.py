import threading
from threading import Thread
from threading import Semaphore
import time

cant_bandejas = input("Cantidad de bandejas: ")
cant_clientes = input("Cantidad de clientes: ")

s_bandejasFila = Semaphore(1)
s_bandejasBandejero = Semaphore(1)
s_JuanSirviendo = Semaphore(1)

s_cantidadBandejasFila = Semaphore(int(cant_bandejas))
s_cantidadBandejasBandejero = Semaphore(0)

s_esperandoParaAyudar = Semaphore(1)
s_necesitoAyuda = Semaphore(1)

class Cliente(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_bandejasFila, s_bandejasBandejero, s_JuanSirviendo, s_cantidadBandejasFila, s_cantidadBandejasBandejero
        id_cliente = threading.get_ident()

        # SACAR BANDEJAS

        # Hola, vengo a sacar una bandeja
        # Hay una persona sacando una bandeja :c espero noma
        s_bandejasFila.acquire() # Por fin puedo sacar una bandeja
        print(str(id_cliente)+": Estoy sacando una bandeja ;)")
        s_cantidadBandejasFila.acquire() # *sacando bandeja*
        print(str(id_cliente)+": Ya saque la bandeja, grax, pase mi rey")
        s_bandejasFila.release() # *deja que el siguiente cliente saque bandeja

        # BUSCAR ALMUERZO

        print(str(id_cliente)+": Toy comiendo")
        time.sleep(5) # Tarda 5 segundos en comerse un canape wtf
        print(str(id_cliente)+": Ya comi")
        s_cantidadBandejasBandejero.release()

        # AYUDAR AL HERMANO

class Juan(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global s_bandejasFila, s_bandejasBandejero, s_JuanSirviendo
        print("Hola soy Juan c:")
        
def main():
    global s_bandejasFila, s_bandejasBandejero, s_JuanSirviendo
    global cant_bandejas, cant_clientes, bandejas, mesa

    thread_juan = Juan()
    thread_juan.start()
    
    for cliente in range(int(cant_clientes)):
        thread = Cliente()
        thread.start()
   
if __name__ == '__main__':
    main()