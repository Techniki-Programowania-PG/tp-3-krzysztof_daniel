import signal_analyzer as sa
#sa.showSignal([1, 2, 3, 4, 5], [3, 3, 2, 1, 4])

sampling_rate = 200
frequency = 5
duration = 1

#generowanie sinusa
y = sa.generate_signal(sa.SignalType.Sin, frequency, sampling_rate, duration)
##oś czasu
#x=[i for i in range(len(y))]
N = len(y)
#czas w sekudnach dla osi poziomej
x = [n / sampling_rate for n in range(N)]
sa.showSignal(x, y)

sa.DFT(y, sampling_rate, True)