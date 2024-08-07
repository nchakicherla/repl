fn test_fn (int num, str base) > str {
	var output = ""
	for 0 to num by 1:
		output = output + base
	return output
}

a = 3
b = test_fn (3, "test")

print("a was: " + a + ", b is now: " + b)
