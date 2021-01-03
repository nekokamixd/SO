import time
import threading

class mesa:
    def __init__(self):
        self.bandeja = None
    
    def insert_bandeja(self, bandeja):
        self.bandeja = bandeja

    def release_bandeja(self):
        aux = self.bandeja
        self.bandeja = None
        return aux

    def get_bandeja(self):
        return self.bandeja

class bandeja:
    def __init__(self, numero):
        self.comida = False
        self.id = numero

    def comer(self):
        self.comida = False

    def llenar(self):
        time.sleep(3)
        self.comida = True

    def getId(self):
        return self.id

def juan_funcion(lock_fila, cv_fila_aCliente, cv_fila_aJuan, bandejas, servidos_actual, cocina):
    while (servidos_actual[0] < numero_atendiendo):
        lock_fila.acquire()
        while (cocina.get_bandeja() == None):
            cv_fila_aJuan.wait()
        aux_bandeja = mesa.get_bandeja()
        aux_bandeja.llenar()
        time.sleep(3)
        mesa.release_bandeja()
        servidos_actual[0] += 1
        cv_fila_aCliente.notify_all()
        lock_fila.release()

def cliente_funcion(numero_atendiendo, bandeja, lock_fila, cv_fila_aJuan, cv_fila_aCliente, servidos_actual, cocina):
    print(numero_atendiendo)
    lock_fila.acquire()
    while (cocina.get_bandeja() != None):
        cv_fila_aCliente.wait()
    mesa.insert_bandeja(bandeja)
    print("Ya comi uwu")
    servidos_actual[1] += 1
    cv_fila_aJuan.notify()
    lock_fila.release()

lock_fila= threading.Lock()
cv_fila_aJuan = threading.Condition(lock_fila)
cv_fila_aCliente = threading.Condition(lock_fila)

servidos_actual = [0, 0]

cant_bandejas = input("Cantidad de bandejas: ")
cant_clientes = input("Cantidad de clientes: ")

bandejaId = 0
bandejas = []
for b in range(int(cant_bandejas)):
    bandejas.append(bandeja(b))

cocina = mesa()

numero_atendiendo = 1

f1 = open("juan.txt", "w")
f2 = open("clientes.txt", "w")

thread_juan = threading.Thread(name="hilo"+"Juan", target=juan_funcion, args=(lock_fila, cv_fila_aCliente, cv_fila_aJuan, bandejas, servidos_actual, cocina))
thread_juan.start()

for cliente in range(int(cant_clientes)):
    thread = threading.Thread(name="hilo"+str(cliente), target=cliente_funcion, args=(numero_atendiendo, bandejas.pop(0), lock_fila, cv_fila_aJuan, cv_fila_aCliente, servidos_actual, cocina))
    thread.start()
    numero_atendiendo += 1

f1.close()
f2.close()