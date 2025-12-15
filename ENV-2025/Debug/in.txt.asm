.586
.model flat, stdcall
OPTION CASEMAP:NONE
includelib libucrt.lib
includelib kernel32.lib
includelib ENV2025Lib.lib
ExitProcess PROTO :DWORD 
.stack 65536

 outnum PROTO : DWORD

 outstr PROTO : DWORD

 outstrline PROTO : DWORD

 outnumline PROTO : DWORD

 system_pause PROTO

 random PROTO : DWORD, : DWORD

 lenght PROTO : DWORD

 power PROTO : DWORD, : DWORD

 compare PROTO : DWORD, : DWORD
.const
 null_division BYTE 'ERROR: DIVISION BY ZERO', 0
 overflow BYTE 'ERROR: VARIABLE OVERFLOW', 0 
	L1 SWORD 1
	L2 BYTE '=== Testing Function ===', 0
	L3 SWORD 5
	L4 BYTE 'factorial(5) = ', 0
	L5 SWORD 26
	L6 BYTE 'x (hex) = ', 0
	L7 SWORD -27
	L8 BYTE 'q (negative hex) = ', 0
	L9 SWORD 10
	L10 BYTE 'y (binary) = ', 0
	L11 SWORD -13
	L12 BYTE 'z (negative binary) = ', 0
	L13 BYTE 'a (boolean true) = ', 0
	L14 SWORD 0
	L15 BYTE 'b (boolean false) = ', 0
	L16 BYTE 'c (boolean yes) = ', 0
	L17 BYTE 'd (boolean no) = ', 0
	L18 BYTE 'e (boolean 1) = ', 0
	L19 BYTE 'f (boolean 0) = ', 0
	L20 SWORD 2
	L21 BYTE 'result = x + y * 2 = ', 0
	L22 SWORD 20
	L23 SWORD 3
	L24 BYTE 'divresult = 20 / 3 = ', 0
	L25 BYTE 'modresult = 20 % 3 = ', 0
	L26 BYTE 'A', 0
	L27 BYTE 'sym = ', 0
	L28 BYTE 'Hello', 0
	L29 BYTE 'World', 0
	L30 BYTE 'str1 = ', 0
	L31 BYTE 'str2 = ', 0
	L32 BYTE 'Cycle example (repeat 5 times):', 0
	L33 BYTE ' ', 0
	L34 BYTE 'Iteration ', 0
	L35 BYTE 'Cycle finished. Final i = ', 0
	L36 BYTE '=== Testing Library Functions ===', 0
	L37 BYTE 'pow(2, 3) = 2 to power 3 = ', 0
	L38 BYTE 'pow(5, 2) = 5 to power 2 = ', 0
	L39 BYTE 'pow(10, 0) = 10 to power 0 = ', 0
	L40 BYTE 'Testing random function:', 0
	L41 BYTE 'random(1, 10) = ', 0
	L42 SWORD 50
	L43 SWORD 100
	L44 BYTE 'random(50, 100) = ', 0
	L45 BYTE 'Testing lenght function:', 0
	L46 BYTE 'Hello World', 0
	L47 BYTE 'lenght(teststr) where teststr = ', 0
	L48 BYTE ' = ', 0
	L49 BYTE 'lenght(str1) where str1 = ', 0
	L50 BYTE 'lenght(str2) where str2 = ', 0
	L51 BYTE 'Testing compare function:', 0
	L52 BYTE 'compare(str1, str2) = compare(Hello, World) = ', 0
	L53 BYTE 'compare(str1, str3) = compare(Hello, Hello) = ', 0
	L54 BYTE 'XYZ', 0
	L55 BYTE 'ABC', 0
	L56 BYTE 'compare(str4, str5) = compare(XYZ, ABC) = ', 0
	L57 BYTE '=== All tests completed! ===', 0
	L58 BYTE 'Program finished!', 0
.data
	_factorialresult SWORD 0
	_factoriali SWORD 0
	mainnum SWORD 0
	mainfact SWORD 0
	mainx SWORD 0
	mainq SWORD 0
	mainy SWORD 0
	mainz SWORD 0
	maina SWORD 0
	mainb SWORD 0
	mainc SWORD 0
	maind SWORD 0
	maine SWORD 0
	mainf SWORD 0
	mainresult SWORD 0
	maindivresult SWORD 0
	mainmodresult SWORD 0
	mainsym DWORD ?
	mainstr1 DWORD ?
	mainstr2 DWORD ?
	maini SWORD 0
	mainpowresult SWORD 0
	mainpowresult2 SWORD 0
	mainpowresult3 SWORD 0
	mainrand1 SWORD 0
	mainrand2 SWORD 0
	mainteststr DWORD ?
	mainstrlen SWORD 0
	mainstrlen1 SWORD 0
	mainstrlen2 SWORD 0
	maincmp1 SWORD 0
	mainstr3 DWORD ?
	maincmp2 SWORD 0
	mainstr4 DWORD ?
	mainstr5 DWORD ?
	maincmp3 SWORD 0

.code

_factorial PROC _factorialn :  SWORD 
	mov word ptr _factorialresult, 1
	mov word ptr _factoriali, 1
	movsx esi, word ptr _factorialn
cycle1:
	movsx eax, word ptr _factorialresult
	push eax
	movsx eax, word ptr _factoriali
	push eax
	pop ebx
	pop eax
	imul ebx
	push eax
	pop eax
	mov word ptr _factorialresult, ax

	movsx eax, word ptr _factoriali
	push eax
	movsx eax, word ptr L1
	push eax
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov word ptr _factoriali, ax

	dec esi
	cmp esi, 0
	jne cycle1
continue1:
	movsx eax, word ptr _factorialresult
	ret

SOMETHINGWRONG:
push offset null_division
call outstrline
call system_pause
push -1
call ExitProcess

EXIT_OVERFLOW:
push offset overflow
call outstrline
call system_pause
push -2
call ExitProcess
_factorial ENDP
main PROC

push offset L2
call outstrline
	mov word ptr mainnum, 5
	movsx eax, word ptr mainnum
	push eax
		call _factorial
	push eax
	pop eax
	mov word ptr mainfact, ax


push offset L4
call outstr

	movsx eax, word ptr mainfact
	push eax
call outnumline
	mov word ptr mainx, 26

push offset L6
call outstr

	movsx eax, word ptr mainx
	push eax
call outnumline
	mov word ptr mainq, -27

push offset L8
call outstr

	movsx eax, word ptr mainq
	push eax
call outnumline
	mov word ptr mainy, 10

push offset L10
call outstr

	movsx eax, word ptr mainy
	push eax
call outnumline
	mov word ptr mainz, -13

push offset L12
call outstr

	movsx eax, word ptr mainz
	push eax
call outnumline
	mov word ptr maina, 1

push offset L13
call outstr

	movsx eax, word ptr maina
	push eax
call outnumline
	mov word ptr mainb, 0

push offset L15
call outstr

	movsx eax, word ptr mainb
	push eax
call outnumline
	mov word ptr mainc, 1

push offset L16
call outstr

	movsx eax, word ptr mainc
	push eax
call outnumline
	mov word ptr maind, 0

push offset L17
call outstr

	movsx eax, word ptr maind
	push eax
call outnumline
	mov word ptr maine, 1

push offset L18
call outstr

	movsx eax, word ptr maine
	push eax
call outnumline
	mov word ptr mainf, 0

push offset L19
call outstr

	movsx eax, word ptr mainf
	push eax
call outnumline
	movsx eax, word ptr mainx
	push eax
	movsx eax, word ptr mainy
	push eax
	movsx eax, word ptr L20
	push eax
	pop ebx
	pop eax
	imul ebx
	push eax
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov word ptr mainresult, ax


push offset L21
call outstr

	movsx eax, word ptr mainresult
	push eax
call outnumline
	movsx eax, word ptr L22
	push eax
	movsx eax, word ptr L23
	push eax
	pop ebx
	pop eax
	cmp ebx,0
	je SOMETHINGWRONG
	cdq
	idiv ebx
	push eax
	pop eax
	mov word ptr maindivresult, ax


push offset L24
call outstr

	movsx eax, word ptr maindivresult
	push eax
call outnumline
	movsx eax, word ptr L22
	push eax
	movsx eax, word ptr L23
	push eax
	pop ebx
	pop eax
	cmp ebx,0
	je SOMETHINGWRONG
	cdq
	idiv ebx
	push edx
	pop eax
	mov word ptr mainmodresult, ax


push offset L25
call outstr

	movsx eax, word ptr mainmodresult
	push eax
call outnumline
	push offset L26
	pop mainsym


push offset L27
call outstr

push mainsym
call outstrline
	push offset L28
	pop mainstr1

	push offset L29
	pop mainstr2


push offset L30
call outstr

push mainstr1
call outstrline

push offset L31
call outstr

push mainstr2
call outstrline

push offset L32
call outstr

push offset L33
call outstrline
	mov word ptr maini, 0
	mov esi, 5
cycle2:

push offset L34
call outstr

	movsx eax, word ptr maini
	push eax
call outnumline
	movsx eax, word ptr maini
	push eax
	movsx eax, word ptr L1
	push eax
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov word ptr maini, ax

	dec esi
	cmp esi, 0
	jne cycle2
continue2:

push offset L35
call outstr

	movsx eax, word ptr maini
	push eax
call outnumline

push offset L36
call outstrline
	movsx eax, word ptr L20
	push eax
	movsx eax, word ptr L23
	push eax
	pop edx
	pop ecx
	push edx
	push ecx
		call power
	push eax
	pop eax
	mov word ptr mainpowresult, ax


push offset L37
call outstr

	movsx eax, word ptr mainpowresult
	push eax
call outnumline
	movsx eax, word ptr L3
	push eax
	movsx eax, word ptr L20
	push eax
	pop edx
	pop ecx
	push edx
	push ecx
		call power
	push eax
	pop eax
	mov word ptr mainpowresult2, ax


push offset L38
call outstr

	movsx eax, word ptr mainpowresult2
	push eax
call outnumline
	movsx eax, word ptr L9
	push eax
	movsx eax, word ptr L14
	push eax
	pop edx
	pop ecx
	push edx
	push ecx
		call power
	push eax
	pop eax
	mov word ptr mainpowresult3, ax


push offset L39
call outstr

	movsx eax, word ptr mainpowresult3
	push eax
call outnumline

push offset L33
call outstrline

push offset L40
call outstr
	movsx eax, word ptr L1
	push eax
	movsx eax, word ptr L9
	push eax
	pop edx
	pop ecx
	push edx
	push ecx
		call random
	push eax
	pop eax
	mov word ptr mainrand1, ax


push offset L41
call outstr

	movsx eax, word ptr mainrand1
	push eax
call outnumline
	movsx eax, word ptr L42
	push eax
	movsx eax, word ptr L43
	push eax
	pop edx
	pop ecx
	push edx
	push ecx
		call random
	push eax
	pop eax
	mov word ptr mainrand2, ax


push offset L44
call outstr

	movsx eax, word ptr mainrand2
	push eax
call outnumline

push offset L33
call outstrline

push offset L45
call outstr
	push offset L46
	pop mainteststr

	push mainteststr
		call lenght
	push eax
	pop eax
	mov word ptr mainstrlen, ax


push offset L47
call outstr

push mainteststr
call outstr

push offset L48
call outstr

	movsx eax, word ptr mainstrlen
	push eax
call outnumline
	push mainstr1
		call lenght
	push eax
	pop eax
	mov word ptr mainstrlen1, ax


push offset L49
call outstr

push mainstr1
call outstr

push offset L48
call outstr

	movsx eax, word ptr mainstrlen1
	push eax
call outnumline
	push mainstr2
		call lenght
	push eax
	pop eax
	mov word ptr mainstrlen2, ax


push offset L50
call outstr

push mainstr2
call outstr

push offset L48
call outstr

	movsx eax, word ptr mainstrlen2
	push eax
call outnumline

push offset L33
call outstrline

push offset L51
call outstr
	push mainstr1
	push mainstr2
	pop edx
	pop ecx
	push edx
	push ecx
		call compare
	push eax
	pop eax
	mov word ptr maincmp1, ax


push offset L52
call outstr

	movsx eax, word ptr maincmp1
	push eax
call outnumline
	push offset L28
	pop mainstr3

	push mainstr1
	push mainstr3
	pop edx
	pop ecx
	push edx
	push ecx
		call compare
	push eax
	pop eax
	mov word ptr maincmp2, ax


push offset L53
call outstr

	movsx eax, word ptr maincmp2
	push eax
call outnumline
	push offset L54
	pop mainstr4

	push offset L55
	pop mainstr5

	push mainstr4
	push mainstr5
	pop edx
	pop ecx
	push edx
	push ecx
		call compare
	push eax
	pop eax
	mov word ptr maincmp3, ax


push offset L56
call outstr

	movsx eax, word ptr maincmp3
	push eax
call outnumline

push offset L33
call outstrline

push offset L57
call outstrline

push offset L58
call outstrline
call system_pause
push 0
call ExitProcess
SOMETHINGWRONG:
push offset null_division
call outstrline
call system_pause
push -1
call ExitProcess
EXIT_OVERFLOW:
push offset overflow
call outstrline
call system_pause
push -2
call ExitProcess
main ENDP
end main
