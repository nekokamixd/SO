# An example python program using semaphore provided by the python threading module
import threading
import time

parkRequests = 0
removeRequests = 0

parked = 0
removed = 0

parkedLock = threading.Lock()
removedLock = threading.Lock()

availableParkings = threading.Semaphore(10)

def ParkCar():
        availableParkings.acquire()
        global parkedLock
        parkedLock.acquire()

        global parked
        parked = parked+1
        parkedLock.release()

        print("Parked: " + str(parked))

def RemoveCar():
        availableParkings.release()
        global removedLock
        removedLock.acquire()

        global removed
        removed = removed+1
        removedLock.release()
        print("Removed: " + str(removed))       

# Thread that simulates the entry of cars into the parking lot
def parkingEntry():
    # Creates multiple threads inside to simulate cars that are parked
    while(True):
        time.sleep(1)
        incomingCar = threading.Thread(target=ParkCar)
        incomingCar.start()

        global parkRequests
        parkRequests = parkRequests+1
        print("Parking Requests: " + str(parkRequests))

# Thread that simulates the exit of cars from the parking lot
def parkingExit():
    # Creates multiple threads inside to simulate cars taken out from the parking lot
    while(True):
        time.sleep(3)
        outgoingCar = threading.Thread(target=RemoveCar)
        outgoingCar.start()

        global removeRequests
        removeRequests = removeRequests+1
        print("Remove Requests: " + str(removeRequests))

# Start the parking eco-system
parkingEntryThread = threading.Thread(target=parkingEntry)
parkingExitThread = threading.Thread(target=parkingExit)

parkingEntryThread.start()
parkingExitThread.start()