import threading

def func1():
    while True: 
        print(1)

def func2():
    while True: 
        print(2)


t1 = threading.Thread(target=func1)
t2 = threading.Thread(target=func2)
t1.start()
t2.start()