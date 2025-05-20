import signal_analyzer as sa
#sa.showSignal([1, 2, 3, 4, 5], [3, 3, 2, 1, 4])

frequency = 5
duration = 1
sampling_rate = -1

if(sampling_rate==-1):
    sampling_rate = frequency*100

# print(f"sampling_rate: {sampling_rate}")

#generowanie sinusa
y = sa.generate_signal(sa.SignalType.Sawtooth, frequency, sampling_rate, duration)
##oś czasu
#x=[i for i in range(len(y))]
N = len(y)
#czas w sekudnach dla osi poziomej
x = [n / sampling_rate for n in range(N)]
# sa.showSimpleSignal(x, y)

# print(f"y:\n{y}")

sa.DFT(y, sampling_rate, True)
sa.FFT(y, sampling_rate, True)