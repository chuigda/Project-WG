from compdec import *
import time

start_time = time.time()
compressed = compress(Example_Data)
end_time = time.time()

print("Compressed %d -> %d bytes in %f seconds" % (len(Example_Data), len(compressed), end_time - start_time))