def Diamond(n):
    rows = n
    n = 0

    character = 65

    for i in range(1, rows + 1):
        for j in range (1, (rows - i) + 1):
            print(end = " ")

        itr = 0
        M = 0
        N = 2 * i - 1

        while n != (2 * i - 1):
            print(chr(character - itr), end = "")
            n = n + 1
            if(M < N/2 - 1):
                itr = itr + 1
            else:
                itr = itr - 1
            M = M + 1
        n = 0
        print()
        character = character + 1

    character = character - 2

    k = 1
    n = 1
    for i in range(1, rows):
        for j in range (1, k + 1):
            print(end = " ")
        k = k + 1

        itr = 0
        M = 0
        N = 2 * (rows - i) - 1

        while n <= (2 * (rows - i) - 1):
            print(chr(character - itr), end = "")
            n = n + 1
            if(M < N/2 - 1):
                itr = itr + 1
            else:
                itr = itr - 1
            M = M + 1
        n = 1
        print()
        character = character - 1



rows = 5
Diamond(rows)

