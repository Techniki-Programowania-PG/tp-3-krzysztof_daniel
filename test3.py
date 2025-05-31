import signal_analyzer as sa
import random

signal = [random.randint(0, 50) for i in range(15)]

kernel = sa.generateGaussianKernel1D(5, 2/3)
blurred = sa.filter1D(signal, kernel)

sa.showSimpleSignal([i for i in range(15)], signal)
sa.showSimpleSignal([i for i in range(15)], blurred)


image = [[random.randint(0, 50) for i in range(15)] for j in range(15)]

kernel = sa.generateGaussianKernel2D(5, 2/3)
blurred = sa.filter2D(image, kernel)

sa.showImage(image)
sa.showImage(blurred)