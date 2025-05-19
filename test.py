import signal_analyzer as sa
#sa.showSignal([1, 2, 3, 4, 5], [3, 3, 2, 1, 4])

#generowanie sinusa
y = sa.generate_signal(sa.SignalType.Sin, 3, 500, 1)
#oś czasu
x=[]
for i, el in enumerate(y):
    x.append(i)

sa.showSignal(x, y)