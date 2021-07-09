// program hello

byte loopt[] = {STRING, 't', 'e', 's', 't', 0,
                PRINT,
               };

byte prog1[] = {STRING, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 0,
                PRINTLN,
               };

byte prog2[] = {STRING, 'p', 'a', 's', 's', 'e', 'd', 0, SET, 's',
                CHAR, 'a', SET, 'c',
                INT, 'l', 7, SET, 'i', // 263
                FLOAT, 66, 246, 230, 102, SET, 'f', //123.45
                GET, 's', PRINTLN,
                GET, 'c', PRINTLN,
                GET, 'i', PRINTLN,
                GET, 'f', PRINTLN,
                STOP
               };

byte prog3[] = {INT, 0, 50, SET, 'i',
                LOOP,
                GET, 'i', INCREMENT, SET, 'i',
                GET, 'i', PRINTLN,
                MILLIS, INT, 3, 232, PLUS, // 1000
                DELAYUNTIL,
                ENDLOOP
               };

// program test_if
byte prog4[] = {INT, 0, 3, SET, 'a',
                CHAR, 5, SET, 'b',
                GET, 'a', GET, 'b', EQUALS,
                IF, 7,
                STRING, 'T', 'r', 'u', 'e', 0,
                PRINTLN,
                ELSE, 8,
                STRING, 'F', 'a', 'l', 's', 'e', 0,
                PRINTLN,
                ENDIF,
                CHAR, 3, SET, 'b',
                GET, 'a', GET, 'b', EQUALS,
                IF, 7,
                STRING, 'T', 'r', 'u', 'e', 0,
                PRINTLN,
                ELSE, 8,
                STRING, 'F', 'a', 'l', 's', 'e', 0,
                PRINTLN,
                ENDIF,
                STOP
               };
