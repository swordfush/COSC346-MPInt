import subprocess
import sys
import random

testCount = int(sys.argv[1])
maxBits = 32 * 5

def debug(num):
	if num < 0:
		num = -num
	elif num == 0:
		return "0\n"
	max = 2**32 - 1
	desc = ""
	while num != 0:
		desc += str(num % max) + "\n"
		num = num / max
	return desc

def tryOperation(a, b, op, expect):
	output = subprocess.check_output(["./test", str(a), op, str(b)], stderr=subprocess.STDOUT)

	# Strip newline
	output = int(output[:-1])

	if output != expect:
		print "Failed", a, op, b
		print "\tExpected:", expect
		print "\tGot:", output
		print "\tExpected internal representation:"
		print debug(expect)
		print "\tReceived internal representation:"
		print debug(output)
		sys.exit(0)

for i in xrange(0, testCount):
	aBits = random.randint(1, maxBits)
	bBits = random.randint(1, maxBits)

	a = int(random.getrandbits(aBits))
	if random.randint(0, 1) == 0:
		a = -a
	b = int(random.getrandbits(bBits))
	if random.randint(0, 1) == 0:
		b = -b


	tryOperation(a, b, "+", a + b)
	tryOperation(a, b, "-", a - b)
	tryOperation(a, b, "*", a * b)
	if b != 0:
		tryOperation(a, b, "/", a / b)
		tryOperation(a, b, "%", a % b)
	print "Passed", i+1
	print "\t", a
	print "\t", b
